#include <unordered_map>
#include <utility>
#include <set>
#include <vector>

struct StringBuf {
public:
    StringBuf(const char* ptr, int len);
    StringBuf(const std::string& str);
    StringBuf(const StringBuf&) = default;
    bool operator< (const StringBuf& other) const {
        return true;
    }
public:
    const char* Ptr;
    int Len;
};


typedef std::vector<std::pair<void*, StringBuf>> Results;

class n_gramm_impl
{
public:
    n_gramm_impl(unsigned int n) : n_count_(n), indexes_(n) { }
    void add_line(void *document, StringBuf text);
    void del_line(void *document);
    Results search(StringBuf query);

private:
    typedef unsigned long IndexKey;
    typedef std::pair<void *, StringBuf> IndexValue;
    typedef std::set<IndexValue> IndexValueSet;
    typedef std::unordered_map<IndexKey, IndexValueSet> NIndex;
    typedef std::vector<NIndex> Indexes;
    typedef std::pair<const char*, unsigned int> CSTR;
    typedef std::vector<CSTR> CSTRList;
    typedef std::unordered_map<void *, StringBuf> Storage;

    void add_del_index(void *document, StringBuf text, bool is_add);
    void select_substrs(CSTRList &, CSTRList &, const char *c_str, const unsigned int len);
    void create_n_gramms(CSTRList &, const char *c_str, const unsigned int len);
    bool is_real_substrs(CSTRList &, const char * c_str, const unsigned int size);

    const unsigned int n_count_;
    Indexes indexes_;
    Storage storage_;
};
