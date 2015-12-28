%module indexer
%{
	#include "indexer.h"
%}

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