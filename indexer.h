#include <Python.h>
#include <list>
#include "NGramm.h"
#include <string>

class NGramm
{
	public:
		NGramm(unsigned int n) : pimpl_(n, *this) { };

		const int size();
		void addLine(PyObject *index, PyObject *str);
		PyObject * strictSearch(PyObject *str);
		PyObject * search(PyObject *pattern);
		void delLine(PyObject *index);

		const int get_c_string(PyObject * str, char* &ref) const;
		void incr_refs(PyObject *str);
		void decr_refs(PyObject *str);
	private:
		typedef Impl::n_gramm<PyObject, NGramm> Pimpl;
		PyObject * convert_search_result(const Pimpl::IndexValueList &);
		Pimpl pimpl_;
};
