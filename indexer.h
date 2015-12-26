#include <unordered_map>
#include <utility>
#include <Python.h>
#include <set>
#include <vector>

#include "indexer_impl.h"

class n_gramm
{
public:
    n_gramm(unsigned int n);
	void add_line(PyObject *, PyObject *);
	void del_line(PyObject *);

	PyObject * search(PyObject *);
private:
    n_gramm_impl Impl;
};
