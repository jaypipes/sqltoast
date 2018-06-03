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

typedef enum output_format {
    OUTPUT_FORMAT_DEFAULT,
    OUTPUT_FORMAT_YAML
} output_format_t;

const long LIST_ITEM_OFF = 0;
const long LIST_ITEM_ON = 1;
// Informs the printer whether the current item to be printed is a list item
const int LIST_ITEM_XALLOC_INDEX = 0;

typedef struct printer {
    int iomanip_indexes[1];
    sqltoast::parse_result_t& res;
    output_format_t output_format;
    size_t indent_level;
    printer(
            sqltoast::parse_result_t& res,
            std::ostream& out) :
        res(res),
        output_format(OUTPUT_FORMAT_DEFAULT),
        indent_level(0)
    {
        iomanip_indexes[LIST_ITEM_XALLOC_INDEX] = std::ios_base::xalloc();
        int idx_list_item = iomanip_indexes[LIST_ITEM_XALLOC_INDEX];
        out.iword(idx_list_item) = LIST_ITEM_OFF;
    }
    inline void indent_push(std::ostream& out) {
        indent_level++;
    }
    inline void indent_pop(std::ostream& out) {
        indent_level--;
    }
    inline std::ostream& indent(std::ostream& out) {
        out << std::endl;
        if (indent_level > 0)
            out << std::string(indent_level * 2, ' ');
        return out;
    }
    inline void start_list(std::ostream& out) {
        int idx_list_item = iomanip_indexes[LIST_ITEM_XALLOC_INDEX];
        out.iword(idx_list_item) = LIST_ITEM_ON;
    }
    inline void end_list(std::ostream& out) {
        int idx_list_item = iomanip_indexes[LIST_ITEM_XALLOC_INDEX];
        out.iword(idx_list_item) = LIST_ITEM_OFF;
    }
    inline bool in_list(std::ostream& out) const {
        return (out.iword(iomanip_indexes[LIST_ITEM_XALLOC_INDEX]) == LIST_ITEM_ON);
    }
} printer_t;

std::ostream& operator<< (std::ostream& out, printer_t& ptr);

} // namespace sqltoaster

#endif /* SQLTOASTER_PRINTER_H */
