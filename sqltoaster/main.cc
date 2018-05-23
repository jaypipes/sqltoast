// A demonstration of the sqltoast library API

#include <string.h>

#include <iostream>
#include <chrono>

#include <sqltoast/sqltoast.h>

#include "print/printers.h"
#include "printer.h"

using namespace std;

template<typename TimeT = std::chrono::milliseconds>
struct measure
{
    template<typename F, typename ...Args>
    static typename TimeT::rep execution(F&& func, Args&&... args)
    {
        auto start = std::chrono::steady_clock::now();
        std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
        auto duration = std::chrono::duration_cast<TimeT>
                            (std::chrono::steady_clock::now() - start);
        return duration.count();
    }
};

struct parser {
    sqltoast::parse_options_t opts;
    sqltoast::parse_input_t subject;
    sqltoast::parse_result_t res;
    parser(sqltoast::parse_options_t& opts, const std::string &input) :
        opts(opts),
        subject(input.cbegin(), input.cend())
    {}
    void operator()() {
        res = sqltoast::parse(subject, opts);
    }
};

void usage(const char* prg_name) {
    cout << "Usage: " << prg_name << " [--disable-timer] [--yaml] <SQL>" << endl;
    cout << " using libsqltoast version " <<
        SQLTOAST_VERSION_MAJOR << '.' <<
        SQLTOAST_VERSION_MINOR << endl;
}

int main (int argc, char *argv[])
{
    std::string input;
    bool disable_timer = false;
    bool use_yaml = false;

    for (int x = 1; x < argc; x++) {
        if (strcmp(argv[x], "--disable-timer") == 0) {
            disable_timer = true;
            continue;
        }
        if (strcmp(argv[x], "--yaml") == 0) {
            use_yaml = true;
            continue;
        }
        input.assign(argv[x]);
        break;
    }
    if (input.empty()) {
        usage(argv[0]);
        return 1;
    }

    sqltoast::parse_options_t opts = {sqltoast::SQL_DIALECT_ANSI_1992, false};
    parser p(opts, input);

    auto dur = measure<std::chrono::nanoseconds>::execution(p);
    sqltoaster::printer ptr(p.res, cout);
    if (use_yaml)
        ptr.set_yaml(cout);
    if (p.res.code == sqltoast::PARSE_OK) {
        cout << "OK";
        cout << ptr << std::endl;
    } else if (p.res.code == sqltoast::PARSE_INPUT_ERROR) {
        cout << "Input error: " << p.res.error << endl;
    } else {
        cout << "Syntax error." << endl;
        cout << p.res.error << endl;
    }
    if (! disable_timer)
        cout << "(took " << dur << " nanoseconds)" << endl;
    return 0;
}
