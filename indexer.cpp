#include <iostream>
#include "indexer.h"
#include <memory>
#include <algorithm>
#include <iterator>

const int get_c_string(PyObject *str, char* &ref);
const unsigned long hash_of_string(const char *c_str, const unsigned int len);

PyObject *
n_gramm::search(PyObject *pattern)
{
	char *c_str;
	const int size = get_c_string(pattern, c_str);
	CSTRList substrs;
	CSTRList n_gramms;
	select_substrs(n_gramms, substrs, c_str, size);

	PyObject *result = PyList_New(0);
	if (n_gramms.empty()) {
		return result;
	}

	std::sort(n_gramms.begin(), n_gramms.end(), [](const CSTR &lhs, const CSTR &rhs) {
		return lhs.second < rhs.second;
	});

	IndexResultSet intersection;
	bool is_init_intersection = true;

	for (auto &p : n_gramms) {
		const char *c = p.first;
		const unsigned int len = p.second;
		auto range = indexes_[len - 1].equal_range(hash_of_string(c, len));

		if (!std::distance(range.first, range.second)) {
			return result;
		}

		for (auto it = range.first; it != range.second; it++) {
			auto s = it->second.second;
			char *c_str;
			get_c_string(s, c_str);
		}

		if (is_init_intersection) {
			for (auto it = range.first; it != range.second; it++) {
				const IndexResult &el = *it;
				intersection.insert(el);
			}
			is_init_intersection = false;
			continue;
		}

		for (auto it = intersection.begin(); it != intersection.end(); it++) {
			const IndexResult &el = *it;
			bool is_there = false;
			for (auto it = range.first; it != range.second; it++)
				if (it->second == el.second) {
					is_there = true;
					break;
				}
			if (!is_there)
				intersection.erase(el);
		}


		if (intersection.empty()) {
			return result;
		}
	}

	unsigned int i = 0;
	for (auto &p : intersection) {
		PyObject *str = p.second.second;
		char * c_str;
		const unsigned int size = get_c_string(str, c_str);
		if (!is_real_substrs(substrs, c_str, size))
			continue;

		PyObject * tuple = PyTuple_New(2);
		PyTuple_SetItem(tuple, 0, p.second.first);
		PyTuple_SetItem(tuple, 1, str);
		PyList_Insert(result, i++, tuple);
	}

	return result;
}


bool n_gramm::is_real_substrs(CSTRList &substrs, char * c_str, const unsigned int size)
{
	bool is_there = true;
	if (substrs.size() > 1) {
		is_there = true;
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
	}

	return is_there;
}

void 
n_gramm::add_line(PyObject *index, PyObject *str)
{
	if (!PyString_Check(str)) {
		throw std::exception("Expected a string");
	}

	add_to_index(str, std::make_pair(index, str));
}

void 
n_gramm::add_to_index(PyObject *str, IndexValue &value)
{
	using namespace std;
	char *c_str;
	const unsigned int size = get_c_string(str, c_str);
	for (unsigned int pos = 0; pos < size; pos++)
		for (unsigned int len = 1; pos + len <= size && len <= n_count_; len++) {
			HashLongPyObject &hash_map = indexes_[len - 1];
			IndexKey hash = hash_of_string(&c_str[pos], len);
			auto range = hash_map.equal_range(hash);
			bool is_duplicate = false;
			for (auto it = range.first; it != range.second; ++it) {
				if (it->second.first == value.first && it->second.second == value.second)
					is_duplicate = true;
			}
			if (!is_duplicate)
				hash_map.insert(make_pair(hash, value));
		}
}

void
n_gramm::create_n_gramms(CSTRList &n_gramms, char *c_str, const unsigned int size)
{
	const unsigned int len = std::min(size, n_count_);
	unsigned int pos = 0;
	while (true) {
		n_gramms.push_back(std::make_pair(&c_str[pos], len));
		const unsigned int end = pos + len;
		if (end == size)
			break;
		pos = std::min(end, size - len);
	}
}

void 
n_gramm::select_substrs(CSTRList &n_gramms, CSTRList &substr, char *c_str, const unsigned int size)
{
	bool prev_is_start = true;
	char *cur_c = nullptr;
	int len = 0;
	for (unsigned int i = 0; i < size; i++) {
		if (c_str[i] == '*') {
			if (!prev_is_start) {
				substr.push_back(std::make_pair(cur_c, len));
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
		substr.push_back(std::make_pair(cur_c, len));
		create_n_gramms(n_gramms, cur_c, len);
	}
}