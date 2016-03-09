#include "indexer.h"
#include <iostream>


void 
NGramm::addLine(PyObject *index, PyObject *str)
{
	if (!(PyInt_Check(index) && PyString_Check(str))) {
		throw std::string("Wrong type");
	}

	pimpl_.add_line(PyInt_AsUnsignedLongMask(index), str);
}

PyObject * 
NGramm::search(PyObject *pattern)
{
	if (!PyString_Check(pattern)) {
		throw std::string("Wrong type");
	}

	const Pimpl::IndexValueList &result = pimpl_.search(pattern);
	return convert_search_result(result);
}


PyObject *
NGramm::strictSearch(PyObject *str)
{
	if (!PyString_Check(str)) {
		throw std::string("Wrong type");
	}

	const Pimpl::IndexValueList &result = pimpl_.strict_search(str);
	return convert_search_result(result);
}

PyObject *
NGramm::convert_search_result(const Pimpl::IndexValueList &c_result)
{
	PyObject *result = PyList_New(0);
	for (auto &p : c_result) {
		PyObject *index = PyInt_FromLong(p.first);
		PyList_Append(result, index);
	}
	return result;
}

void 
NGramm::delLine(PyObject *index)
{
	if (!PyInt_Check(index)) {
		throw std::string("Wrong type");
	}

	return pimpl_.del_line(PyInt_AsUnsignedLongMask(index));
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

void
NGramm::incr_refs(PyObject *str) {
	Py_INCREF(str);
};

void
NGramm::decr_refs(PyObject *str) {
	Py_XDECREF(str);
};