#include <iostream>
#include "indexer.h"
#include <memory>
#include <algorithm>
#include <iterator>

#include <time.h>

const int
get_c_string(PyObject *str, char* &ref) {
    ref = PyString_AsString(str);
    return PyString_Size(str);
}

n_gramm::n_gramm(unsigned int n)
    : Impl(n)
{
}

void
n_gramm::add_line(PyObject *index, PyObject *str)
{
	if (!PyString_Check(str)) {
        throw std::string("Expected a string");
	}

    char* textPtr;
    int textLen = get_c_string(str, textPtr);
    StringBuf text(textPtr, textLen);
    Impl.add_line(index, text);
}

void
n_gramm::del_line(PyObject *index) 
{
    Impl.del_line(index);
}


PyObject *
n_gramm::search(PyObject *pattern)
{
    char* textPtr;
    int textLen = get_c_string(pattern, textPtr);

    Results res = Impl.search(StringBuf(textPtr, textLen));

    PyObject *result = PyList_New(0);
    for (size_t i = 0; i < res.size(); ++i) {
        PyList_Insert(result, i, (PyObject *)res[i].first);
    }

	return result;
}
