#include <stdint.h>
#include "nta.h"

static inline uint64_t f(uint64_t x, uint64_t n) {
    return (mulmod(x, x, n) + 1) % n;
}

uint64_t pollard(uint64_t n) {
    if (n % 2 == 0)
        return 2;

    if (is_prime(n))
        return 0;

    uint64_t x = 2;
    uint64_t y = 2;
    uint64_t d = 1;

    while (d == 1) {
        x = f(x, n);
        y = f(y, n);
        y = f(y, n);

        uint64_t diff = abs_my((int64_t)x - (int64_t)y);
        d = gcd(diff, n);
    }
    
    if (d == n)
        return 0;
    
    return d;
}
