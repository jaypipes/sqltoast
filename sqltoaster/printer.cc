/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sqltoast/print.h>

#include "node.h"
#include "fill.h"
#include "printer.h"

namespace sqltoaster {

std::ostream& operator<< (std::ostream& out, printer_t& ptr) {
    if (ptr.output_format == OUTPUT_FORMAT_DEFAULT) {
        unsigned int x = 0;
        for (auto stmt_ptr_it = ptr.res.statements.cbegin();
                stmt_ptr_it != ptr.res.statements.cend();
                stmt_ptr_it++) {
            out << std::endl << "statements[" << x++ << "]:" << std::endl;
            out << "  " << *(*stmt_ptr_it);
        }
    } else {
        ptr.process_statements();
        if (ptr.statement_node_count()) {
            mapping_t statements;
            statements.setattr("statements", ptr.statements);
            print_map(out, statements, 0, false);
        }
    }
    return out;
}

void printer_t::process_statements() {
    std::unique_ptr<node_t> statement_node;
    for (std::unique_ptr<sqltoast::statement_t>& stmt : res.statements) {
        std::unique_ptr<node_t> statement_node = std::make_unique<mapping_t>();
        mapping_t& statement_map =
            static_cast<mapping_t&>(*statement_node);
        fill(statement_map, *stmt);
        add_statement_node(statement_node);
    }
}

} // namespace sqltoaster
