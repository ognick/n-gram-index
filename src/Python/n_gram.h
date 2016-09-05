#include <Python.h>
#include <list>
#include "NGram_p.h"
#include <string>

class NGram
{
	public:
        NGram(unsigned int n) : pimpl_(n, *this) { };

		const int size();
		void addLine(PyObject *index, PyObject *str);
		PyObject * search(PyObject *pattern);
		PyObject * search(PyObject *pattern, const bool isStrict);
		void delLine(PyObject *index);

		const int get_c_string(PyObject * str, char* &ref) const;
		void incr_refs(PyObject *str);
		void decr_refs(PyObject *str);
	private:
        typedef Impl::n_gram<PyObject, NGram> Pimpl;
		Pimpl pimpl_;
};
