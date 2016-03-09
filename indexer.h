#include <Python.h>
#include <list>
#include "NGramm.h"
#include <string>

class NGramm
{
	public:
		NGramm(unsigned int n) : pimpl_(n, *this) { };

		const int size();
        const bool hasValue(PyObject *str);
		void addLine(PyObject *index, PyObject *str);
		PyObject * search(PyObject *pattern);
		void delLine(PyObject *index);
    
    
        const int get_c_string(PyObject * str, char* &ref) const;
        void incr_refs(PyObject *str);
        void decr_refs(PyObject *str);
	private:
		Impl::n_gramm<PyObject, NGramm>  pimpl_;
};
