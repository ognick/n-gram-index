#include <unordered_map>
#include <utility>
#include <Python.h>

class n_gramm
{
public:
	n_gramm(unsigned int n) : n_count_(n), indexes_(n + 1) { }
	void add_line(PyObject *, PyObject *);
	//PyObject *get_line(PyObject *);

	PyObject * search(PyObject *);

private:
	typedef unsigned long IndexKey;
	typedef std::pair<PyObject *, PyObject *> IndexValue;
	typedef std::pair<IndexKey, IndexValue> IndexResult;
	typedef std::vector<IndexResult> IndexResultList;
	typedef std::unordered_multimap<IndexKey, IndexValue> HashLongPyObject;
	typedef std::vector<HashLongPyObject> Indexes;
	typedef std::pair<char*, unsigned int> CSTR;
	typedef std::vector<CSTR> CSTRList;

	void add_to_index(PyObject *str, IndexValue &value);
	void select_substrs(CSTRList &, CSTRList &, char *c_str, const unsigned int len);
	void create_n_gramms(CSTRList &, char *c_str, const unsigned int len);

	const unsigned int n_count_;
	Indexes indexes_;
};