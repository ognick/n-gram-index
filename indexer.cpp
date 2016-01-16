#include "indexer.h"
#include <iostream>

void 
n_gramm::add_line(PyObject *index, PyObject *str)
{
	if (!PyString_Check(str)) {
		throw std::string("Expected a string");
	}

	pimpl_.add_line(index, str);
}

PyObject * 
n_gramm::search(PyObject *pattern)
{
	PyObject *result = PyList_New(0);
	const auto &r = pimpl_.search(pattern);
	for (auto &p : r) {
        PyObject *index = p.first;
		PyList_Append(result, index);
	}
	return result;
}

void 
n_gramm::del_line(PyObject *index)
{
	return pimpl_.del_line(index);
}

const int 
n_gramm::size()
{
	return pimpl_.size();
}

const int 
n_gramm::get_c_string(PyObject * str, char* &ref) {
	ref = PyString_AsString(str);
	return PyString_Size(str);
};