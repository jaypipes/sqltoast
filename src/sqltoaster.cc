// A demonstration of the sqltoast library API

#include <iostream>

#include <sqltoast.h>
#include <parse.h>

using namespace std;

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
    sqltoast::parser_input_t p_in(input.cbegin(), input.cend());

    auto res = sqltoast::parse(p_in);
    if (res.code == sqltoast::SUCCESS) {
        cout << "OK" << endl;
    } else {
        cout << "Parsing failed." << endl;
        for (auto e : res.errors) {
            cout << e << endl;
        }
    }
    return 0;
}
