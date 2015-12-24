#include <iostream>
#include "indexer.h"
#include <memory>
#include <algorithm>
#include <iterator>
#include <set>

const int get_c_string(PyObject *str, char* &ref);
const unsigned long hash_of_string(const char *c_str, const unsigned int len);

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
		for (unsigned int len = 1; pos + len <= size && len <= n_count_; len++)
			indexes_[len].insert(make_pair(hash_of_string(&c_str[pos], len), value));
}

PyObject *
n_gramm::search(PyObject *pattern)
{
	char *c_str;
	const int size = get_c_string(pattern, c_str);
	CSTRList substrs;
	CSTRList n_gramms;
	select_substrs(n_gramms, substrs, c_str, size);

	if (n_gramms.empty()) {
		return Py_None;
	}

	std::sort(n_gramms.begin(), n_gramms.end(), [](const CSTR &lhs, const CSTR &rhs) {
		return lhs.second < rhs.second;
	});

	IndexResultList intersection;
	bool is_init_intersection = true;

	for (auto &p : n_gramms) {
		const char *c = p.first;
		const unsigned int len = p.second;
		auto range = indexes_[len].equal_range(hash_of_string(c, len));

		if (!std::distance(range.first, range.second)) {
			return Py_None;
		}

		if (is_init_intersection) {
			intersection.insert(intersection.end(), range.first, range.second);
			is_init_intersection = false;
			continue;
		}

		std::set_intersection(intersection.begin(), intersection.end(),
			range.first, range.first,
			std::back_inserter(intersection),
			[](const IndexResult &lhs, const IndexResult &rhs) {
				return lhs.second.first == rhs.second.first;
		});

		if (intersection.empty()) {
			return Py_None;
		}
	}

	PyObject *result = PyList_New(intersection.size());
	unsigned int i = 0;
	for (auto &p : intersection) {
		PyObject * tuple = PyTuple_New(2);
		PyList_SetItem(result, i++, tuple);
		PyTuple_SetItem(tuple, 0, p.second.first);
		PyTuple_SetItem(tuple, 1, p.second.second);
	}
	
	return result;
}


void
n_gramm::create_n_gramms(CSTRList &n_gramms, char *c_str, const unsigned int size)
{
	const unsigned int len = std::min(size, n_count_);
	unsigned int pos = 0;
	do {
		n_gramms.push_back(std::make_pair(&c_str[pos], len));
	} while ((++pos) + len <= size);
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