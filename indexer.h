#include <unordered_map>
#include <utility>
#include <Python.h>
#include <set>

class n_gramm
{
public:
	n_gramm(unsigned int n) : n_count_(n), indexes_(n) { }
	void add_line(PyObject *, PyObject *);
	void del_line(PyObject *);

	PyObject * search(PyObject *);
private:
	typedef unsigned long IndexKey;
	typedef std::pair<PyObject *, PyObject *> IndexValue;
	typedef std::set<IndexValue> IndexValueSet;
	typedef std::unordered_map<IndexKey, IndexValueSet> NIndex;
	typedef std::vector<NIndex> Indexes;
	typedef std::pair<char*, unsigned int> CSTR;
	typedef std::vector<CSTR> CSTRList;
	typedef std::unordered_map<PyObject *, PyObject *> Storage;

	void add_del_index(PyObject *index, PyObject *str, bool is_add);
	void select_substrs(CSTRList &, CSTRList &, char *c_str, const unsigned int len);
	void create_n_gramms(CSTRList &, char *c_str, const unsigned int len);
	bool is_real_substrs(CSTRList &, char * c_str, const unsigned int size);

	const unsigned int n_count_;
	Indexes indexes_;
	Storage storage_;
};