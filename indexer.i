%module indexer
%{
	#include "indexer.h"
%}

class n_gramm 
{
	public:
		n_gramm(unsigned int n);
		void add_line(PyObject *index, PyObject *str);
		PyObject * search(PyObject *pattern);
		void del_line(PyObject *index);
		const int get_c_string(PyObject * str, char* &ref);
};


%exception n_gramm::add_line {
   try {
      $action
   } catch (std::string &e) {
      PyErr_SetString(PyExc_BaseException, e.c_str());
      return NULL;
   }
}

%exception n_gramm::del_line {
   try {
      $action
   } catch (std::string &e) {
      PyErr_SetString(PyExc_BaseException, e.c_str());
      return NULL;
   }
}

%exception n_gramm::search {
   try {
      $action
   } catch (std::exception &e) {
      PyErr_SetString(PyExc_BaseException, e.what());
      return NULL;
   }
}
 
%include "indexer.h"