#include <stdint.h>

uint64_t mulmod(uint64_t a, uint64_t b, uint64_t mod) {
    __uint128_t res = ( __uint128_t ) a * b;
    return (uint64_t)(res % mod);
}

uint64_t powmod(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;

    while (exp > 0) {
        if (exp & 1)
            result = mulmod(result, base, mod);
        base = mulmod(base, base, mod);
        exp >>= 1;
    }
    return result;
}

uint64_t gcd(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t r = a % b;
        a = b;
        b = r;
    }
    return a;
}

uint64_t abs_my(int64_t x) {
    return (x >= 0)
        ? (uint64_t)x
        : (uint64_t)(-( __int128_t )x);
}

uint64_t try_div(uint64_t n, uint64_t p) {
    if (n < 2)
        return 0;

    if ((n & 1) == 0)
        return (n == 2 ? 0 : 2);

    for (uint64_t d = 3; d <= p; d += 2) {
        if (n % d == 0)
            return d;
    }

    return 0;  
}

int legendre(uint64_t n, uint64_t p) {
    if (p < 2 || (p & 1) == 0)
        return 0;  

    n %= p;
    if (n == 0)
        return 0;

    uint64_t t = powmod(n, (p - 1) >> 1, p);

    if (t == 1)
        return 1;
    if (t == p - 1)
        return -1;

    return 0;
}
