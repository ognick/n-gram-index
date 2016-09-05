%module n_gram_index 
%{
    #include "n_gram.h"
%}

%exception NGram::addLine {
	try {
		$action
	} catch (std::string &e) {
		PyErr_SetString(PyExc_BaseException, e.c_str());
		return NULL;
	}
}

%exception NGram::delLine {
	try {
		$action
	} catch (std::string &e) {
		PyErr_SetString(PyExc_BaseException, e.c_str());
		return NULL;
	}
}

%exception NGram::search {
	try {
		$action
	} catch (std::exception &e) {
		PyErr_SetString(PyExc_BaseException, e.what());
		return NULL;
	}
}


%include "n_gram.h"
