/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_LEXEME_H
#define SQLTOAST_LEXEME_H

namespace sqltoast {

typedef std::vector<char> parse_input_t;

typedef parse_input_t::const_iterator parse_position_t;

// A lexeme_t demarcates some word or phrase within the tokenized input stream
typedef struct lexeme {
    parse_position_t start;
    parse_position_t end;
    lexeme() :start(0), end(0)
    {}
    lexeme(
        parse_position_t start,
        parse_position_t end) :
        start(start), end(end)
    {}
    inline size_t size() const {
        return end - start;
    }
    inline operator bool() const {
        return start != parse_position_t(0);
    }
} lexeme_t;

inline std::ostream& operator<< (std::ostream& out, const lexeme_t& word) {
    out << std::string(word.start, word.end);
    return out;
}

} // namespace sqltoast

#endif /* SQLTOAST_STATEMENT_H */
