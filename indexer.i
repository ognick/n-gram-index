%module indexer
 
%{
	#include "indexer.h"
%}


%inline %{
	const int 
	get_c_string(PyObject *str, char* &ref) {
		ref = PyString_AsString(str);
		return PyString_Size(str);
	}

	const unsigned long 
	hash_of_string(const char *c_str, const unsigned int len)
	{
		unsigned long hash = 5381;
		for (unsigned int i = 0; i<len; i++)
			hash = ((hash << 5) + hash) + c_str[i];
		return hash;
	}

%}


%exception n_gramm::add_line {
   try {
      $action
   } catch (std::exception &e) {
      PyErr_SetString(PyExc_BaseException, e.what());
      return NULL;
   }
}

%exception n_gramm::del_line {
   try {
      $action
   } catch (std::exception &e) {
      PyErr_SetString(PyExc_BaseException, e.what());
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
 
%include "std_string.i"
%include "typemaps.i"
%include "indexer.h"