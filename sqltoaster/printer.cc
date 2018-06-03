/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "printer.h"
#include "print/yaml.h"

namespace sqltoaster {

std::ostream& operator<< (std::ostream& out, printer_t& ptr) {
    if (ptr.output_format == OUTPUT_FORMAT_DEFAULT) {
        unsigned int x = 0;
        for (auto stmt_ptr_it = ptr.res.statements.cbegin();
                stmt_ptr_it != ptr.res.statements.cend();
                stmt_ptr_it++) {
            out << "statements[" << x++ << "]:" << std::endl;
            out << "  " << *(*stmt_ptr_it);
        }
    } else {
        out << "statements:";
        ptr.indent_push(out);
        for (auto stmt_ptr_it = ptr.res.statements.cbegin();
                stmt_ptr_it != ptr.res.statements.cend();
                stmt_ptr_it++) {
            ptr.start_list(out);
            print::to_yaml(ptr, out, *(*stmt_ptr_it));
        }
        ptr.indent_pop(out);
    }
    return out;
}

} // namespace sqltoaster
