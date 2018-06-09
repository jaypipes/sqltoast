/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOASTER_PRINTER_H
#define SQLTOASTER_PRINTER_H

#include <iostream>

#include <sqltoast/sqltoast.h>

#include "node.h"

namespace sqltoaster {

typedef enum output_format {
    OUTPUT_FORMAT_DEFAULT,
    OUTPUT_FORMAT_YAML
} output_format_t;

typedef struct printer {
    int iomanip_indexes[1];
    sqltoast::parse_result_t& res;
    output_format_t output_format;
    size_t indent_level;
    bool list_item;
    std::unique_ptr<node_t> statements;
    printer(
            sqltoast::parse_result_t& res,
            std::ostream& out) :
        res(res),
        output_format(OUTPUT_FORMAT_DEFAULT),
        indent_level(0),
        list_item(false),
        statements(std::make_unique<sequence_t>())
    {}
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
        list_item = true;
    }
    inline void end_list(std::ostream& out) {
        list_item = false;
    }
    inline bool in_list(std::ostream& out) const {
        return list_item;
    }
    inline void add_statement_node(std::unique_ptr<node_t>& node) {
        sequence_t& stmt_seq = static_cast<sequence_t&>(*statements);
        stmt_seq.elements.emplace_back(std::move(node));
    }
    inline size_t statement_node_count() const {
        sequence_t& stmt_seq = static_cast<sequence_t&>(*statements);
        return stmt_seq.elements.size();
    }
    void process_statements();
} printer_t;

std::ostream& operator<< (std::ostream& out, printer_t& ptr);

} // namespace sqltoaster

#endif /* SQLTOASTER_PRINTER_H */
