#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "header.h"


int main(int argc, char *argv[]) {

    if ((argc > 7) || (argc < 4)) {
        print_help();
        return 1;
    }

    return b64(argv, argc);

}