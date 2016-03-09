#include <unordered_map>
#include <utility>
#include <set>
#include <memory>
#include <algorithm>
#include <iterator>
#include <functional>
#include <vector>

namespace Impl {
	typedef unsigned long IndexKey;
	typedef unsigned long HashKey;
	typedef std::pair<char*, unsigned int> CStrLen;

	inline const HashKey hash_of_string(const char *c_str, const unsigned int len)
	{
		unsigned long hash = 5381;
		for (unsigned int i = 0; i < len; i++)
			hash = ((hash << 5) + hash) + c_str[i];
		return hash;
	}

	template<class V, class Consumer>
	struct n_gramm
	{
		typedef std::pair<IndexKey, V *> IndexValue;
		typedef std::list<IndexValue> IndexValueList;
		typedef std::set<IndexValue> IndexValueSet;

		typedef std::unordered_map<HashKey, IndexValueSet> NIndex;
		typedef std::vector<NIndex> Indexes;

		typedef std::vector<CStrLen> CStrLenList;
		typedef std::unordered_map<IndexKey, V *> Storage;

		const unsigned int n_count_;
		Indexes indexes_;
		Storage storage_;
		Consumer &consumer_;

		n_gramm(unsigned int n, Consumer &consumer) : n_count_(n), indexes_(n), consumer_(consumer){}
		~n_gramm()
		{
			for (auto p: storage_) {
				V *str = p.second;
				consumer_.decr_refs(str);
			}
		}

		const int size()
		{
			return storage_.size();
		}

		void add_line(IndexKey index, V *str)
		{
			auto f = storage_.find(index);
			if (f != storage_.end())
				return;

			consumer_.incr_refs(str);

			storage_.insert({index, str});
			add_del_index(index, str, true);
		}

		void del_line(IndexKey index)
		{
			auto f = storage_.find(index);
			if (f == storage_.end())
				return;

			V *str = f->second;

			add_del_index(index, str, false);
			storage_.erase(f);
			
			consumer_.decr_refs(str);
		}


		IndexValueList strict_search(V *str)
		{
			IndexValueList result;
			char * c_str;
			const unsigned int size = consumer_.get_c_string(str, c_str);
			for (auto &r : search(str)) {
				V *cur_str = r.second;
				char * cur_c_str;
				const unsigned int cur_size = consumer_.get_c_string(cur_str, cur_c_str);
				if (size != cur_size)
					continue;

				if (std::equal(c_str, c_str + size, cur_c_str))
					result.push_back(r);
			}

			return result;
		}

		IndexValueList search(V *pattern)
		{
			char *c_str;
			const int size = consumer_.get_c_string(pattern, c_str);

			CStrLenList substrs;
			CStrLenList n_gramms;
			select_substrs(n_gramms, substrs, c_str, size);

			IndexValueList result;
			if (n_gramms.empty()) {
				copy(storage_.begin(), storage_.end(), back_inserter(result));
				return result;
			}

			IndexValueSet intersection;
			bool is_init_intersection = true;

			for (auto &p : n_gramms) {
				const char *c = p.first;
				const unsigned int len = p.second;

				NIndex &hash_map = indexes_[len - 1];
				const auto hash = hash_of_string(c, len);
				auto f = hash_map.find(hash);
				if (f == hash_map.end()) 
					return result;

				IndexValueSet &resultSet = f->second;

				if (is_init_intersection) {
					intersection = resultSet;
					is_init_intersection = false;
					continue;
				}

				IndexValueSet set_intersection_result;
				std::set_intersection(
					intersection.begin(), intersection.end(),
					resultSet.begin(), resultSet.end(),
					std::inserter(set_intersection_result, set_intersection_result.begin())
					);
				set_intersection_result.swap(intersection);

				if (intersection.empty()) {
					return result;
				}
			}

			for (auto &p : intersection) {
				V *str = p.second;
				char * c_str;
				const unsigned int size = consumer_.get_c_string(str, c_str);
				if (!is_real_substrs(substrs, c_str, size))
					continue;

				result.push_back({ p.first, str });
			}

			return result;
		}

		void add_del_index(IndexKey index, V *str, bool is_add)
		{
			using namespace std;
			char *c_str;
			IndexValue value = std::make_pair(index, str);
			const unsigned int size = consumer_.get_c_string(str, c_str);
			for (unsigned int pos = 0; pos < size; pos++)
				for (unsigned int len = 1; pos + len <= size && len <= n_count_; len++) {
					NIndex &hash_map = indexes_[len - 1];
					HashKey hash = hash_of_string(&c_str[pos], len);
					//if (len == 6) {
					//	auto tmp = c_str[pos + len];
					//	c_str[pos + len] = '\0';
					//	cout << "add:" << &c_str[pos] << " len:" << len << "\n";
					//	cout << "hash :" << hash << endl;
					//	c_str[pos + len] = tmp;
					//}

					auto f = hash_map.find(hash);
					if (f == hash_map.end()) {
						if (!is_add)
							continue;

						IndexValueSet new_value_set;
						new_value_set.insert(value);
						hash_map.insert(std::make_pair(hash, new_value_set));

						continue;
					}

					IndexValueSet &old_value_set = f->second;

					if (is_add) {
						old_value_set.insert(value);
					}
					else {
						old_value_set.erase(value);
						if (old_value_set.empty()) {
							hash_map.erase(hash);
						}
					}
				}
		}

		void select_substrs(CStrLenList &n_gramms, CStrLenList &substr, char *c_str, const unsigned int size)
		{
			bool prev_is_start = true;
			char *cur_c = nullptr;
			int len = 0;
			for (unsigned int i = 0; i < size; i++) {
				if (c_str[i] == '*') {
					if (!prev_is_start) {
						substr.push_back({ cur_c, len });
						create_n_gramms(n_gramms, cur_c, len);
					}
					prev_is_start = true;
				}
				else {
					if (prev_is_start) {
						cur_c = &c_str[i];
						len = 0;
					}
					prev_is_start = false;
					len++;
				}
			}

			if (!prev_is_start) {
				substr.push_back({ cur_c, len });
				create_n_gramms(n_gramms, cur_c, len);
			}
		}

		void create_n_gramms(CStrLenList &n_gramms, char *c_str, const unsigned int size)
		{
			const unsigned int len = std::min(size, n_count_);
			unsigned int pos = 0;
			while (true) {
				n_gramms.push_back({ &c_str[pos], len });
				const unsigned int end = pos + len;
				if (end == size)
					break;
				pos = std::min(end, size - len);
			}
		}

		bool is_real_substrs(CStrLenList &substrs, char * c_str, const unsigned int size)
		{
			bool is_there = true;
			unsigned int offset = 0;
			for (const auto &p : substrs) {
				is_there = false;
				char *c_sub = p.first;
				unsigned int len = p.second;
				while (offset + len <= size) {
					if (std::equal(c_str + offset, c_str + offset + len, c_sub)) {
						is_there = true;
						offset += len;
						break;
					}
					offset++;
				}
				if (!is_there)
					break;
			}

			return is_there;
		}
		
	};
}
