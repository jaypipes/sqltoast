// A demonstration of the sqltoast library API

#include <iostream>

#include <sqltoast.h>

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
    sqltoast::parse_input_t p_in(input.cbegin(), input.cend());

    auto res = sqltoast::parse(p_in);
    if (res.code == sqltoast::PARSE_SUCCESS) {
        cout << "OK" << endl;
    } else if (res.code == sqltoast::PARSE_INPUT_ERROR) {
        cout << "Input error: " << res.error << endl;
    } else {
        cout << "Syntax error." << endl;
        cout << res.error << endl;
    }
    return 0;
}
