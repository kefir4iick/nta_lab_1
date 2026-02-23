#include <stdint.h>
#include <stddef.h>
#include "nta.h"

static int miller_test(uint64_t a, uint64_t s, uint64_t d, uint64_t n) {
    uint64_t x = powmod(a, d, n);

    if (x == 1 || x == n - 1)
        return 1;

    for (uint64_t i = 1; i < s; i++) {
        x = mulmod(x, x, n);
        if (x == n - 1)
            return 1;
    }
    return 0;
}

int is_prime(uint64_t n) {
    if (n < 2)
        return 0;
    if (n < 4)
        return 1;
    if ((n & 1) == 0)
        return 0;

    uint64_t d = n - 1;
    uint64_t s = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        s++;
    }

    static const uint64_t bases[] = {
        2, 3, 5, 7, 11, 13, 17
    };

    for (size_t i = 0; i < sizeof(bases)/sizeof(bases[0]); i++) {
        uint64_t a = bases[i];
        if (a >= n)
            continue;
        if (!miller_test(a, s, d, n))
            return 0;
    }

    return 1;
}
