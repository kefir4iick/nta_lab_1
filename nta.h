#ifndef NTA_H
#define NTA_H

#include <stdint.h>
#include <stdlib.h> 
#include <stddef.h>
#include <time.h>
#include <stdio.h>


uint64_t mulmod(uint64_t a, uint64_t b, uint64_t mod);
uint64_t powmod(uint64_t base, uint64_t exp, uint64_t mod);
uint64_t gcd(uint64_t a, uint64_t b);
uint64_t abs_my(int64_t x);
uint64_t try_div(uint64_t n, uint64_t p); 
int legendre(uint64_t n, uint64_t p);

int is_prime(uint64_t n);

uint64_t pollard(uint64_t n);

uint64_t bm(uint64_t n);

int factor_main(uint64_t n);
void print_canonical(uint64_t n);

#endif 
