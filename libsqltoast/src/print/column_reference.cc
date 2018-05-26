/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/print.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const derived_column_t& dc) {
    if (dc.value)
        out << *dc.value;
    else
        out << "*";
    if (dc.has_alias())
        out << " AS " << dc.alias;
    return out;
}

std::ostream& operator<< (std::ostream& out, const grouping_column_reference_t& gcr) {
    out << gcr.column;
    if (gcr.has_collation())
        out << " COLLATE " << gcr.collation;
    return out;
}

} // namespace sqltoast
