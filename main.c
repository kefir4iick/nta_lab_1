#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <inttypes.h>
#include "nta.h"

int main(void) {
    uint64_t n;

    printf("enter n:  ");
    if (scanf("%" SCNu64, &n) != 1) {
        printf("error\n");
        return 1;
    }

    struct timeval tv;
    struct tm *tm;
    
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    printf("[%02d:%02d:%02d:%03ld:%06ld] start\n\n", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000, tv.tv_usec);

    if (!factor_main(n)) {
        printf("can't find canonical\n");
        return 1;
    }

    print_canonical(n);

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    printf("\n[%02d:%02d:%02d:%03ld:%06ld] end\n", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000, tv.tv_usec);
    
    

    return 0;
}
