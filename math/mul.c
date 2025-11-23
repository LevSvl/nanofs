#include "types.h"

unsigned long __mulsi3(unsigned long a, unsigned long b) {
    unsigned long result = 0;

    if (a == 0) {
        return 0;
    }
    while (b != 0) {
        if (b & 1) {
            result += a;
        }
        a <<= 1;
        b >>= 1;
    }

    return result;
}

unsigned short __muluhisi3(unsigned short a, unsigned short b) {
    unsigned short result = 0;

    if (a == 0) {
        return 0;
    }
    while (b != 0) {
        if (b & 1) {
            result += a;
        }
        a <<= 1;
        b >>= 1;
    }

    return result;
}
