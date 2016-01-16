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
	unsigned int i = 0;
	const auto &r = pimpl_.search(pattern);
	for (auto &p : r) {
		PyObject *str = p.second;
        Py_INCREF(str);
		PyObject *tuple = PyTuple_New(2);
		PyTuple_SetItem(tuple, 0, p.first);
		PyTuple_SetItem(tuple, 1, str);
		PyList_Insert(result, i++, tuple);
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