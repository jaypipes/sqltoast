/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_STATEMENTS_DROP_SCHEMA_H
#define SQLTOAST_STATEMENTS_DROP_SCHEMA_H

#include <string>

#include "sqltoast.h"
#include "statement.h"

namespace sqltoast {
namespace statements {

typedef enum drop_behaviour {
    DROP_BEHAVIOUR_CASCADE,
    DROP_BEHAVIOUR_RESTRICT
} drop_behaviour_t;

typedef struct drop_schema : statement_t {
    lexeme_t schema_name;
    drop_behaviour_t drop_behaviour;
    drop_schema(lexeme_t& schema_name, drop_behaviour_t drop_behaviour) :
        statement_t(STATEMENT_TYPE_DROP_SCHEMA),
        schema_name(schema_name),
        drop_behaviour(drop_behaviour)
    {}
    virtual const std::string to_string();
} drop_schema_t;

} // namespace sqltoast::statements
} // namespace sqltoast

#endif /* SQLTOAST_STATEMENTS_DROP_SCHEMA_H */
