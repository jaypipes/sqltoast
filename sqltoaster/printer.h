/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOASTER_PRINTER_H
#define SQLTOASTER_PRINTER_H

#include <iostream>
#include <iomanip>

#include <sqltoast/sqltoast.h>

namespace sqltoaster {

const long OUTPUT_FORMAT_DEFAULT = 1;
const long OUTPUT_FORMAT_YAML = 2;

const int OUTPUT_FORMAT_XALLOC_INDEX = 0;
const int INDENT_LEVEL_XALLOC_INDEX = 1;

typedef struct printer {
    int iomanip_indexes[2];
    sqltoast::parse_result_t& res;
    printer(sqltoast::parse_result_t& res) : res(res)
    {
        iomanip_indexes[OUTPUT_FORMAT_XALLOC_INDEX] = std::ios_base::xalloc();
        iomanip_indexes[INDENT_LEVEL_XALLOC_INDEX] = std::ios_base::xalloc();
    }
    inline bool use_yaml(std::ostream& out) const {
        return (out.iword(iomanip_indexes[OUTPUT_FORMAT_XALLOC_INDEX]) == OUTPUT_FORMAT_YAML);
    }
} printer_t;

inline std::ostream& operator<< (std::ostream& out, const printer_t& ptr) {
    if (! ptr.use_yaml(out)) {
        unsigned int x = 0;
        for (auto stmt_ptr_it = ptr.res.statements.cbegin();
                stmt_ptr_it != ptr.res.statements.cend();
                stmt_ptr_it++) {
            out << std::endl << "statements[" << x++ << "]:" << std::endl;
            out << "  " << *(*stmt_ptr_it) << std::endl;
        }
    }
    return out;
}

} // namespace sqltoast

#endif /* SQLTOASTER_PRINTER_H */
