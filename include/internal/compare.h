/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_COMPARE_H
#define SQLTOAST_COMPARE_H

#include <locale>
#include <iostream>
#include <algorithm>
#include <string>

namespace sqltoast {

// templated comparator struct to work with both char and/ wchar_t
template<typename charT>
struct cmp_equal {
    cmp_equal(const std::locale& loc) : loc_(loc) {}
    bool operator()(charT ch1, charT ch2) {
        return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
    }
private:
    const std::locale& loc_;
};

// find substring (case insensitive)
template<typename T>
int ci_find_substr(const T& str1, const T& str2, const std::locale& loc = std::locale()) {
    typename T::const_iterator it = std::search(str1.begin(), str1.end(), str2.begin(), str2.end(), cmp_equal<typename T::value_type>(loc));
    if (it != str1.end()) return it - str1.begin();
    else return -1; // not found
}

} // namespace sqltoast

#endif /* SQLTOAST_COMPARE_H */
