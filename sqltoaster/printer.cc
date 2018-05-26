/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "printer.h"
#include "print/yaml/printers.h"

namespace sqltoaster {

std::ostream& operator<< (std::ostream& out, printer_t& ptr) {
    if (! ptr.use_yaml(out)) {
        unsigned int x = 0;
        for (auto stmt_ptr_it = ptr.res.statements.cbegin();
                stmt_ptr_it != ptr.res.statements.cend();
                stmt_ptr_it++) {
            out << "statements[" << x++ << "]:" << std::endl;
            out << "  " << *(*stmt_ptr_it);
        }
    } else {
        out << "statements:";
        for (auto stmt_ptr_it = ptr.res.statements.cbegin();
                stmt_ptr_it != ptr.res.statements.cend();
                stmt_ptr_it++) {
            out << std::endl << "- ";
            ptr.indent_push(out);
            print::to_yaml(ptr, out, *(*stmt_ptr_it));
            ptr.indent_pop(out);
        }
    }
    return out;
}

} // namespace sqltoaster
