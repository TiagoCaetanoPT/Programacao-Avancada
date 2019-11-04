#include <stdio.h>

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;


    // a) 0...01
    int mask = 1;

    // b) 0...10
    int mask = 1 << 1;

    // c) 0...10101
    int mask = 1 | 1 << 2 | 1 << 4;

    return 0;
}
