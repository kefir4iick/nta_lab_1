#include "nta.h"
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

void bench(uint64_t n) {
    uint64_t d;
    double t1, t2;


    t1 = now_sec();
    d = pollard(n);
    t2 = now_sec();
    printf("n = %" PRIu64 "\n", n);
    printf("pollard: d = %" PRIu64 ", time = %.6f s\n",
           d, t2 - t1);


    t1 = now_sec();
    d = bm(n);
    t2 = now_sec();
    printf("brillhart–morrison: d = %" PRIu64 ", time = %.6f s\n\n",
           d, t2 - t1);
}

int main(void) {
    uint64_t nums[] = {
        3009182572376191ULL,
        1021514194991569ULL,
        4000852962116741ULL,
        15196946347083ULL,
        499664789704823ULL,
        269322119833303ULL,
        679321846483919ULL,
        96267366284849ULL,
        61333127792637ULL,
        2485021628404193ULL
    };

    for (size_t i = 0; i < sizeof(nums)/sizeof(nums[0]); ++i)
        bench(nums[i]);
}
