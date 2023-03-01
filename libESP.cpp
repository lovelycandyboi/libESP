/*
* author: lovelycandyboi
* github: https://github.com/lovelycandyboi
*/
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "./include/cli.h"

using namespace std;

int main() {
    CLI_BLOCK_INIT();
    printf("libESP\n");
    while (!TERMINAL_CONTROL_CLI()) {
        printf("\n");
    }
}
