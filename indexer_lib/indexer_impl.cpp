#include <iostream>
#include "indexer_impl.h"
#include <memory>
#include <algorithm>
#include <iterator>

#include <time.h>

StringBuf::StringBuf(const char *ptr, int len)
    : Ptr(ptr)
    , Len(len)
{
}

StringBuf::StringBuf(const std::string &str)
    : Ptr(str.data())
    , Len(str.size())
{
}

const unsigned long
hash_of_string(const char *c_str, const unsigned int len)
{
    unsigned long hash = 5381;
    for (unsigned int i = 0; i<len; i++)
        hash = ((hash << 5) + hash) + c_str[i];
    return hash;
}

void
n_gramm_impl::add_line(void *document, StringBuf text)
{
    if (storage_.find(document) != storage_.end())
        return;

    storage_.insert({ document, text });
    add_del_index(document, text, true);
}

void
n_gramm_impl::del_line(void *document)
{
    auto f = storage_.find(document);
    if (f == storage_.end())
        return;

    add_del_index(document, f->second, false);
    storage_.erase(f);
}


Results
n_gramm_impl::search(StringBuf query)
{
    const char *c_str = query.Ptr;
    const int size = query.Len;

    CSTRList substrs;
    CSTRList n_gramms;
    select_substrs(n_gramms, substrs, c_str, size);

    Results results;

    if (n_gramms.empty()) {
        return results;
    }

    std::sort(n_gramms.begin(), n_gramms.end(), [](const CSTR &lhs, const CSTR &rhs) {
        return lhs.second < rhs.second;
    });


    IndexValueSet intersection;
    bool is_init_intersection = true;

    for (auto &p : n_gramms) {
        const char *c = p.first;
        const unsigned int len = p.second;
        NIndex &hash_map = indexes_[len - 1];
        auto f = hash_map.find(hash_of_string(c, len));
        if (f == hash_map.end()) {
            return results;
        }

        IndexValueSet &resultSet = f->second;
        if (is_init_intersection) {
            intersection.swap(resultSet);
            is_init_intersection = false;
            continue;
        }

        IndexValueSet set_intersection_result;
        std::set_intersection(
            intersection.begin(), intersection.end(),
            resultSet.begin(), resultSet.end(),
            std::inserter(set_intersection_result, set_intersection_result.begin())
        );
        set_intersection_result.swap(intersection);

        if (intersection.empty()) {
            return results;
        }
    }

    for (auto &p : intersection) {
        StringBuf str = p.second;
        if (!is_real_substrs(substrs, str.Ptr, str.Len))
            continue;

        results.push_back(std::make_pair(p.first, str));
    }

    return results;
}


bool n_gramm_impl::is_real_substrs(CSTRList &substrs, const char * c_str, const unsigned int size)
{
    bool is_there = true;
    if (substrs.size() > 1) {
        is_there = true;
        unsigned int offset = 0;
        for (const auto &p : substrs) {
            is_there = false;
            const char *c_sub = p.first;
            unsigned int len = p.second;
            while (offset + len <= size) {
                if (std::equal(c_str + offset, c_str + offset + len, c_sub)) {
                    is_there = true;
                    offset += len;
                    break;
                }
                offset++;
            }
            if (!is_there)
                break;
        }
    }

    return is_there;
}

void
n_gramm_impl::add_del_index(void *document, StringBuf text, bool is_add)
{
    using namespace std;
    const char *c_str = text.Ptr;
    IndexValue value = std::make_pair(document, text);
    const unsigned int size = text.Len;
    for (unsigned int pos = 0; pos < size; pos++)
        for (unsigned int len = 1; pos + len <= size && len <= n_count_; len++) {
            NIndex &hash_map = indexes_[len - 1];
            IndexKey hash = hash_of_string(&c_str[pos], len);
            auto f = hash_map.find(hash);
            if (f == hash_map.end()) {
                if (!is_add)
                    continue;

                IndexValueSet new_value_set = { value };
                hash_map.insert({ hash, new_value_set });
                continue;
            }
            IndexValueSet &old_value_set = f->second;
            if (is_add)
                old_value_set.insert(value);
            else
                old_value_set.erase(value);
        }
}

void
n_gramm_impl::create_n_gramms(CSTRList &n_gramms, const char *c_str, const unsigned int size)
{
    const unsigned int len = std::min(size, n_count_);
    unsigned int pos = 0;
    while (true) {
        n_gramms.push_back(std::make_pair(&c_str[pos], len));
        const unsigned int end = pos + len;
        if (end == size)
            break;
        pos = std::min(end, size - len);
    }
}

void
n_gramm_impl::select_substrs(CSTRList &n_gramms, CSTRList &substr, const char *c_str, const unsigned int size)
{
    bool prev_is_start = true;
    const char *cur_c = nullptr;
    int len = 0;
    for (unsigned int i = 0; i < size; i++) {
        if (c_str[i] == '*') {
            if (!prev_is_start) {
                substr.push_back(std::make_pair(cur_c, len));
                create_n_gramms(n_gramms, cur_c, len);
            }
            prev_is_start = true;
        }
        else {
            if (prev_is_start) {
                cur_c = &c_str[i];
                len = 0;
            }
            prev_is_start = false;
            len++;
        }
    }
    if (!prev_is_start) {
        substr.push_back(std::make_pair(cur_c, len));
        create_n_gramms(n_gramms, cur_c, len);
    }
}
