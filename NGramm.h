#include <unordered_map>
#include <utility>
#include <set>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <iterator>
#include <functional>
#include <vector>

namespace Impl {
	typedef unsigned long IndexKey;
	typedef std::pair<char*, unsigned int> CSTR;

	inline const IndexKey hash_of_string(const char *c_str, const unsigned int len)
	{
		unsigned long hash = 5381;
		for (unsigned int i = 0; i < len; i++)
			hash = ((hash << 5) + hash) + c_str[i];
		return hash;
	}
	
	struct CSTRHasher
	{
		inline const IndexKey operator()(const CSTR & p) const
		{
			return hash_of_string(p.first, p.second);
		}
	};

	template<class K, class V, class Consumer>
	struct n_gramm
	{
		typedef std::pair<K *, V *> IndexValue;
		typedef std::list<IndexValue> IndexValueList;
		typedef std::set<IndexValue> IndexValueSet;

		typedef std::unordered_map<IndexKey, IndexValueSet> NIndex;
		typedef std::vector<NIndex> Indexes;

		typedef std::vector<CSTR> CSTRList;
		typedef std::unordered_map<K *, V *> Storage;

		typedef std::unordered_multiset<CSTR, CSTRHasher> CSTRSet;

		const unsigned int n_count_;
		Indexes indexes_;
		Storage storage_;
		CSTRSet values_;
		Consumer &consumer_;

		n_gramm(unsigned int n, Consumer &consumer) : n_count_(n), indexes_(n), consumer_(consumer){}
		~n_gramm()
		{
			for (auto p: storage_) {
				consumer_.decr_refs(p.first, p.second);
			}
		}

		const int size()
		{
			return storage_.size();
		}

		
		const bool has_value(V *str)
		{
			char *c_str;
			const int size = consumer_.get_c_string(str, c_str);
			auto range = values_.equal_range({c_str, size});
			for (auto &f = range.first; f != range.second; f++ ) {
				char *cur_c_str = f->first;
				const int cur_size = f->second;
				if (size == cur_size && std::equal(c_str, c_str + size, cur_c_str))
					return true;
			}
			
			return false;
		}
		
		void add_line(K *index, V *str)
		{
			auto f = storage_.find(index);
			if (f != storage_.end())
				return;

			consumer_.incr_refs(index, str);
			
			char *c_str;
			const int size = consumer_.get_c_string(str, c_str);
			values_.insert({c_str, size});
			
			storage_.insert({index, str});
			add_del_index(index, str, true);
		}

		void del_line(K *index)
		{
			auto f = storage_.find(index);
			if (f == storage_.end())
				return;

			V *str = f->second;
			
			char *c_str;
			const int size = consumer_.get_c_string(str, c_str);
			values_.erase({c_str, size});
			
			
			add_del_index(index, str, false);
			storage_.erase(f);
			
			consumer_.decr_refs(index, str);
		}

		IndexValueList search(V *pattern)
		{
			char *c_str;
			const int size = consumer_.get_c_string(pattern, c_str);

			CSTRList substrs;
			CSTRList n_gramms;
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

		void add_del_index(K *index, V *str, bool is_add)
		{
			using namespace std;
			char *c_str;
			IndexValue value = std::make_pair(index, str);
			const unsigned int size = consumer_.get_c_string(str, c_str);
			for (unsigned int pos = 0; pos < size; pos++)
				for (unsigned int len = 1; pos + len <= size && len <= n_count_; len++) {
					NIndex &hash_map = indexes_[len - 1];
					IndexKey hash = hash_of_string(&c_str[pos], len);
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

					if (is_add)  {
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

		void select_substrs(CSTRList &n_gramms, CSTRList &substr, char *c_str, const unsigned int size)
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

		void create_n_gramms(CSTRList &n_gramms, char *c_str, const unsigned int size)
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

		bool is_real_substrs(CSTRList &substrs, char * c_str, const unsigned int size)
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
