/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "printer.h"
#include "print/printers.h"
#include "print/yaml/printers.h"

namespace sqltoaster {

std::ostream& operator<< (std::ostream& out, printer_t& ptr) {
    if (! ptr.use_yaml(out)) {
        unsigned int x = 0;
        for (auto stmt_ptr_it = ptr.res.statements.cbegin();
                stmt_ptr_it != ptr.res.statements.cend();
                stmt_ptr_it++) {
            out << std::endl << "statements[" << x++ << "]:" << std::endl;
            out << "  " << *(*stmt_ptr_it);
        }
    } else {
        for (auto stmt_ptr_it = ptr.res.statements.cbegin();
                stmt_ptr_it != ptr.res.statements.cend();
                stmt_ptr_it++) {
            out << std::endl << "-";
            to_yaml(ptr, out, *(*stmt_ptr_it));
        }
    }
    return out;
}

} // namespace sqltoaster
