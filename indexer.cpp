#include "indexer.h"
#include <iostream>

void 
NGramm::addLine(PyObject *index, PyObject *str)
{
	if (!PyString_Check(str)) {
		throw std::string("Expected a string");
	}

	pimpl_.add_line(index, str);
}

PyObject * 
NGramm::search(PyObject *pattern)
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
NGramm::delLine(PyObject *index)
{
	return pimpl_.del_line(index);
}

const int 
NGramm::size()
{
	return pimpl_.size();
}

const int
NGramm::get_c_string(PyObject * str, char* &ref) const {
	ref = PyString_AsString(str);
	return PyString_Size(str);
};