// A demonstration of the sqltoast library API

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include <sqltoast.h>
//#include <sqltoast/parse.h>

int main (int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stdout,"%s libsqltoast version %d.%d\n",
            argv[0],
            SQLTOAST_VERSION_MAJOR,
            SQLTOAST_VERSION_MINOR);
        return 1;
    }
    return 0;
}
