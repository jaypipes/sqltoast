/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_DEBUG_H
#define SQLTOAST_DEBUG_H

#if defined(SQLTOAST_DEBUG)

#include <iostream>

#include <sqltoast/print.h>

namespace sqltoast {

// The following code was yanked from
// https://blog.galowicz.de/2016/02/20/short_file_macro/
using cstr = const char* const;

static constexpr cstr past_last_slash(cstr str, cstr last_slash)
{
    return *str == '\0' ? last_slash :
           *str == '/'  ? past_last_slash(str + 1, str + 1) :
           past_last_slash(str + 1, last_slash);
}

static constexpr cstr past_last_slash(cstr str)
{
    return past_last_slash(str, str);
}

#define __SHORT_FILE__ ({constexpr cstr sf__ {past_last_slash(__FILE__)}; sf__;})

template<typename T>
void debug_print(const T& v) {
    std::cerr << v << std::endl;
}
template<typename T, typename...Args>
void debug_print(const T& v, Args... rest) {
    std::cerr << v;
    debug_print(rest...);
}
template<typename... Args>
void debug_print(const char *file, int line_no, const char *fn, Args... args) {
    std::cerr << file << '[' << line_no << "] " << fn << "(): ";
    debug_print(args...);
}

} // namespace sqltoast

#define ST_DBG(...) sqltoast::debug_print(__SHORT_FILE__, __LINE__, __func__, __VA_ARGS__)

#else  // defined(SQLTOAST_DEBUG)

#define ST_DBG(fmt, ...)

#endif // defined(SQLTOAST_DEBUG)

#endif /* SQLTOAST_DEBUG_H */
