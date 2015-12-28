#include <Python.h>
#include <list>
#include "NGramm.h"

class n_gramm 
{
	public:
		n_gramm(unsigned int n) : pimpl_(n, *this) { };

		void add_line(PyObject *index, PyObject *str);
		PyObject * search(PyObject *pattern);
		void del_line(PyObject *index);
		const int get_c_string(PyObject * str, char* &ref);
	private:
		Impl::n_gramm<PyObject, PyObject, n_gramm>  pimpl_;
};