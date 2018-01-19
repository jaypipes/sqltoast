// A demonstration of the sqltoast library API

#include <iostream>
#include <chrono>

#include <sqltoast.h>

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
    sqltoast::parse_input_t subject;
    parser(const std::string &input) : subject(input.cbegin(), input.cend())
    {}
    void operator()() {
        auto res = sqltoast::parse(subject);
        if (res.code == sqltoast::PARSE_SUCCESS) {
            cout << "OK" << endl;
        } else if (res.code == sqltoast::PARSE_INPUT_ERROR) {
            cout << "Input error: " << res.error << endl;
        } else {
            cout << "Syntax error." << endl;
            cout << res.error << endl;
        }
    }
};

int main (int argc, char *argv[])
{
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <SQL>" << endl;
        cout << " using libsqltoast version " <<
            SQLTOAST_VERSION_MAJOR << '.' <<
            SQLTOAST_VERSION_MINOR << endl;
        return 1;
    }
    const std::string input(argv[1]);

    auto dur = measure<std::chrono::nanoseconds>::execution(parser(input));
    cout << "(took " << dur << " nanoseconds)" << endl;
    return 0;
}