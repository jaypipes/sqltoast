/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOASTER_NODE_H
#define SQLTOASTER_NODE_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include <sqltoast/sqltoast.h>

namespace sqltoaster {

typedef enum node_type {
    SCALAR,
    SEQUENCE,
    MAPPING
} node_type_t;

struct printer;
typedef struct node {
    node_type_t type;
    node(node_type_t type) :
        type(type)
    {}
} node_t;

typedef struct scalar : node_t {
    std::string value;
    scalar() :
        node_t(SCALAR)
    {}
    scalar(const char *val) :
        node_t(SCALAR),
        value(val)
    {}
    scalar(const std::string& val) :
        node_t(SCALAR),
        value(val)
    {}
    scalar(const sqltoast::lexeme_t& val) :
        node_t(SCALAR),
        value(std::string(val.start, val.end))
    {}
    inline void assign(const char *val) {
        value.assign(std::string(val));
    }
} scalar_t;

typedef struct sequence : node_t {
    std::vector<std::unique_ptr<node_t>> elements;
    sequence() :
        node_t(SEQUENCE)
    {}
    sequence(std::vector<std::unique_ptr<node_t>>& elements) :
        node_t(SEQUENCE),
        elements(std::move(elements))
    {}
    inline void append(const sqltoast::lexeme_t& value) {
        std::unique_ptr<node_t> val = std::make_unique<scalar_t>(value);
        elements.emplace_back(std::move(val));
    }
    inline void append(const std::string& value) {
        std::unique_ptr<node_t> val = std::make_unique<scalar_t>(value);
        elements.emplace_back(std::move(val));
    }
    inline void append(std::unique_ptr<node_t>& value) {
        elements.emplace_back(std::move(value));
    }
} sequence_t;

typedef struct mapping_value {
    const std::string key;
    std::unique_ptr<node_t> value;
    mapping_value(const std::string& key, std::unique_ptr<node_t>& value) :
        key(key),
        value(std::move(value))
    {}
} mapping_value_t;

typedef struct mapping : node_t {
    std::vector<std::unique_ptr<const mapping_value_t>> elements;
    mapping() :
        node_t(MAPPING)
    {}
    mapping(std::vector<std::unique_ptr<const mapping_value_t>>& elements) :
        node_t(MAPPING),
        elements(std::move(elements))
    {}
    inline void setattr(const char *key, std::unique_ptr<node_t>& value) {
        elements.emplace_back(std::make_unique<mapping_value_t>(std::string(key), value));
    }
    inline void setattr(const char *key, const char *value) {
        std::unique_ptr<node_t> val = std::make_unique<scalar_t>(value);
        setattr(key, val);
    }
    inline void setattr(const char *key, const sqltoast::lexeme_t& value) {
        setattr(key, std::string(value.start, value.end).c_str());
    }
    inline void setattr(const char *key, const std::string& value) {
        setattr(key, value.c_str());
    }
    inline void setattr(const sqltoast::lexeme_t& key, const std::string& value) {
        setattr(std::string(key.start, key.end).c_str(), value);
    }
} mapping_t;

void print_map(std::ostream& out, const mapping_t& mapping, size_t indent_level, bool is_list_item);

} // namespace sqltoaster

#endif /* SQLTOASTER_NODE_H */
