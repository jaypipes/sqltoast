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

const long LIST_ITEM_OFF = 0;
const long LIST_ITEM_ON = 1;

const long OUTPUT_FORMAT_DEFAULT = 1;
const long OUTPUT_FORMAT_YAML = 2;

const int OUTPUT_FORMAT_XALLOC_INDEX = 0;
const int INDENT_LEVEL_XALLOC_INDEX = 1;
// Informs the printer whether the current item to be printed is a list item
const int LIST_ITEM_XALLOC_INDEX = 2;

typedef struct printer {
    int iomanip_indexes[3];
    sqltoast::parse_result_t& res;
    printer(sqltoast::parse_result_t& res, std::ostream& out) : res(res)
    {
        iomanip_indexes[OUTPUT_FORMAT_XALLOC_INDEX] = std::ios_base::xalloc();
        iomanip_indexes[INDENT_LEVEL_XALLOC_INDEX] = std::ios_base::xalloc();
        iomanip_indexes[LIST_ITEM_XALLOC_INDEX] = std::ios_base::xalloc();
        int idx_indent = iomanip_indexes[INDENT_LEVEL_XALLOC_INDEX];
        out.iword(idx_indent) = 0;
        int idx_list_item = iomanip_indexes[LIST_ITEM_XALLOC_INDEX];
        out.iword(idx_list_item) = LIST_ITEM_OFF;
    }
    inline void set_yaml(std::ostream& out) {
        int idx_format = iomanip_indexes[OUTPUT_FORMAT_XALLOC_INDEX];
        out.iword(idx_format) = OUTPUT_FORMAT_YAML;
    }
    inline bool use_yaml(std::ostream& out) const {
        return (out.iword(iomanip_indexes[OUTPUT_FORMAT_XALLOC_INDEX]) == OUTPUT_FORMAT_YAML);
    }
    inline void indent_push(std::ostream& out) {
        int idx_indent = iomanip_indexes[INDENT_LEVEL_XALLOC_INDEX];
        long cur_indent = out.iword(idx_indent);
        out.iword(idx_indent) = ++cur_indent;
    }
    inline void indent_pop(std::ostream& out) {
        int idx_indent = iomanip_indexes[INDENT_LEVEL_XALLOC_INDEX];
        long cur_indent = out.iword(idx_indent);
        out.iword(idx_indent) = --cur_indent;
    }
    inline std::ostream& indent(std::ostream& out) {
        int idx_indent = iomanip_indexes[INDENT_LEVEL_XALLOC_INDEX];
        long cur_indent = out.iword(idx_indent);
        out << std::endl;
        if (cur_indent > 0)
            out << std::string(cur_indent * 2, ' ');
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
