#include "n_gram.h"
#include <iostream>


void 
NGram::addLine(PyObject *index, PyObject *str)
{
	if (!(PyInt_Check(index) && PyString_Check(str))) {
		throw std::string("Wrong type");
	}

	pimpl_.add_line(PyInt_AsUnsignedLongMask(index), str);
}


PyObject *
NGram::search(PyObject *pattern)
{
	return search(pattern, false);
}

PyObject * 
NGram::search(PyObject *pattern, const bool isStrict)
{
	if (!PyString_Check(pattern)) {
		throw std::string("Wrong type");
	}

	PyObject *result = PyList_New(0);
	for (auto &p : pimpl_.search(pattern, isStrict)) {
		PyObject *index = PyInt_FromLong(p.first);
		PyList_Append(result, index);
	}
	return result;
}

void
NGram::delLine(PyObject *index)
{
	if (!PyInt_Check(index)) {
		throw std::string("Wrong type");
	}

	return pimpl_.del_line(PyInt_AsUnsignedLongMask(index));
}

const int 
NGram::size()
{
	return pimpl_.size();
}

const int
NGram::get_c_string(PyObject * str, char* &ref) const {
	ref = PyString_AsString(str);
	return PyString_Size(str);
};

void
NGram::incr_refs(PyObject *str) {
	Py_INCREF(str);
};

void
NGram::decr_refs(PyObject *str) {
	Py_XDECREF(str);
};
