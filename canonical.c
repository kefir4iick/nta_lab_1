#include "nta.h"
#include <sys/time.h>
#define MAX_FACTORS 128

uint64_t factors[MAX_FACTORS];
size_t counter = 0;

static void logs(uint64_t d, const char *method) {    
    struct timeval tv;
    struct tm *tm;
    
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    printf("[%02d:%02d:%02d:%03ld:%06ld] found divisor %lu (%s)\n", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000, tv.tv_usec, d, method);
}

static int factor_after_pollard(uint64_t n); //in advance

int factor_main(uint64_t n) {

    if (is_prime(n)) {
        factors[counter++] = n;
        logs(n, "prime test (1)");
        return 1;
    }

    uint64_t d = try_div(n, 47);
    if (d > 1) {
        logs(d, "trial division");
        factor_main(d);
        return factor_main(n / d);
    }

    d = pollard(n);
    if (d > 1 && d < n) {
        logs(d, "pollard");

        factor_main(d);

        return factor_after_pollard(n / d);
    }

    return factor_after_pollard(n);
}

static int factor_after_pollard(uint64_t n) {

    if (is_prime(n)) {
        factors[counter++] = n;
        logs(n, "prime test (2)");
        return 1;
    }

    uint64_t d = bm(n);
    if (d > 1 && d < n) {
        logs(d, "brillhart–morrison");

        factor_after_pollard(d);
        return factor_after_pollard(n / d);
    }

    return 0;
}
    
static int cmp_u64(const void *a, const void *b) {
  uint64_t x = *(const uint64_t *)a;
  uint64_t y = *(const uint64_t *)b;
  return (x > y) - (x < y);
}

void print_canonical(uint64_t n) {
    qsort(factors, counter, sizeof(uint64_t), cmp_u64);

    printf("\ncanonical:\n%lu = ", n);
    for (size_t i = 0; i < counter; ) {
        uint64_t p = factors[i];
        size_t e = 1;
        while (i + e < counter && factors[i + e] == p)
            e++;
        printf("%lu^%zu", p, e);
        i += e;
        if (i < counter) printf(" * ");
    }
    printf("\n");
}
