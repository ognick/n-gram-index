%module indexer
%{
	#include "indexer.h"
%}

%exception NGramm::addLine {
   try {
      $action
   } catch (std::string &e) {
      PyErr_SetString(PyExc_BaseException, e.c_str());
      return NULL;
   }
}

%exception NGramm::delLine {
   try {
      $action
   } catch (std::string &e) {
      PyErr_SetString(PyExc_BaseException, e.c_str());
      return NULL;
   }
}

%exception NGramm::search {
   try {
      $action
   } catch (std::exception &e) {
      PyErr_SetString(PyExc_BaseException, e.what());
      return NULL;
   }
}


%exception NGramm::hasValue {
    try {
        $action
    } catch (std::exception &e) {
        PyErr_SetString(PyExc_BaseException, e.what());
        return NULL;
    }
}
 
%include "indexer.h"