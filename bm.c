#include "nta.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_FB     4096
#define MAX_REL    5120
#define MAX_CF     512

#define FB_WORDS      ((MAX_FB + 63) / 64)
#define MAX_REL_WORDS ((MAX_REL + 63) / 64)

typedef struct {
    int64_t Q;
    uint64_t a;
    uint8_t exp[MAX_FB];         
    uint64_t v_packed[FB_WORDS]; 
} relation_t;

static size_t build_factor_base(uint64_t n, uint64_t *B) {
    size_t k = 0;
    B[k++] = (uint64_t)-1;

    uint64_t bound = 20000;  
    for (uint64_t p = 2; p < bound && k < MAX_FB; p = (p == 2) ? 3 : p + 2) { 
        if (!is_prime(p)) continue;
        if (legendre(n, p) == 1 || (p == 2 && legendre(n, p) >= 0)) {
            B[k++] = p;
        }
    }
    return k;
}

static int factor_over_base(int64_t x, uint64_t *B, size_t k, uint8_t *exp, uint64_t *v_packed) {
    memset(exp, 0, k);
    memset(v_packed, 0, FB_WORDS * sizeof(uint64_t));
    int64_t y = x;

    if (y < 0) {
        exp[0] = 1;
        v_packed[0] |= 1ULL; 
        y = -y;
    }

    for (size_t i = 1; i < k; ++i) {
        uint64_t p = B[i];
        while (y % (int64_t)p == 0) {
            exp[i]++;
            v_packed[i / 64] ^= (1ULL << (i % 64)); 
            y /= (int64_t)p;
        }
        if (y == 1) break; 
    }
    return y == 1;
}

static size_t generate_relations(uint64_t n, uint64_t *B, size_t k, relation_t *R) {
    uint64_t sqrt_n = (uint64_t)sqrt((double)n);
    if (sqrt_n * sqrt_n == n) return 0; 

    uint64_t v = 1;            
    uint64_t a = sqrt_n;       
    uint64_t u = a;            
    
    uint64_t b_prev = 1;       
    uint64_t b = a % n;    
    
    int step = 1; 
    size_t counter = 0;

    while (counter < MAX_REL && step < 10000000UL) {
        uint64_t v_next = (uint64_t)(((__int128)n - (__int128)u * u) / v);
        
        if (v_next == 0) break;

        int64_t Q_val = (step % 2 == 1) ? -(int64_t)v_next : (int64_t)v_next;

        if (factor_over_base(Q_val, B, k, R[counter].exp, R[counter].v_packed)) {
            R[counter].Q = Q_val;
            R[counter].a = b;  
            counter++;
        }

        uint64_t a_next = (sqrt_n + u) / v_next;

        uint64_t u_next = a_next * v_next - u;

        uint64_t b_next = (mulmod(a_next, b, n) + b_prev) % n;

        v = v_next;
        u = u_next;
        a = a_next;
        
        b_prev = b;
        b = b_next;
        
        step++;
    }
    
    return counter;
}

static void gauss(uint64_t A[][FB_WORDS], uint64_t X[][MAX_REL_WORDS], size_t rows, size_t cols) {
    size_t r = 0;
    for (size_t c = 0; c < cols && r < rows; ++c) {
        size_t word_idx = c / 64;
        uint64_t bit_mask = 1ULL << (c % 64);

        size_t piv = r;
        while (piv < rows && !(A[piv][word_idx] & bit_mask)) piv++;
        if (piv == rows) continue;

        if (piv != r) {
            for (size_t j = 0; j < FB_WORDS; ++j) {
                uint64_t t = A[r][j]; A[r][j] = A[piv][j]; A[piv][j] = t;
            }
            for (size_t j = 0; j < MAX_REL_WORDS; ++j) {
                uint64_t t = X[r][j]; X[r][j] = X[piv][j]; X[piv][j] = t;
            }
        }

        for (size_t i = 0; i < rows; ++i) {
            if (i != r && (A[i][word_idx] & bit_mask)) {
                for (size_t j = word_idx; j < FB_WORDS; ++j) A[i][j] ^= A[r][j];
                for (size_t j = 0; j < MAX_REL_WORDS; ++j)   X[i][j] ^= X[r][j];
            }
        }
        r++;
    }
}

uint64_t bm(uint64_t n) {
    uint64_t B[MAX_FB];
    size_t k = build_factor_base(n, B);

    relation_t *R = malloc(MAX_REL * sizeof(relation_t));  
    if (!R) return 0;

    size_t m = generate_relations(n, B, k, R); 

    if (m <= k) {
        free(R);
        return 0;
    }

    uint64_t (*A)[FB_WORDS] = calloc(MAX_REL, sizeof(*A)); 
    uint64_t (*X)[MAX_REL_WORDS] = calloc(MAX_REL, sizeof(*X));
    if (!A || !X) {
        free(A); free(X); free(R);
        return 0;  
    }

    for (size_t i = 0; i < m; ++i) {
        memcpy(A[i], R[i].v_packed, FB_WORDS * sizeof(uint64_t));
        X[i][i / 64] |= (1ULL << (i % 64)); 
    }

    gauss(A, X, m, k);

    for (size_t row = 0; row < m; ++row) {
        int is_zero = 1;
        for (size_t j = 0; j < FB_WORDS; ++j) {
            if (A[row][j] != 0) { is_zero = 0; break; }
        }
        if (!is_zero) continue;

        uint64_t Xv = 1;
        uint64_t Y = 1;

        for (size_t j = 0; j < k; ++j) {
            int sum_e = 0;
            for (size_t i = 0; i < m; ++i) {
                if (X[row][i / 64] & (1ULL << (i % 64))) {
                    sum_e += R[i].exp[j];
                }
            }
            int half_e = sum_e / 2;

            uint64_t p = B[j];
            if (p == (uint64_t)-1) {
                if (half_e % 2 == 1) Y = mulmod(Y, n - 1, n);  
            } else {
                for (int e = 0; e < half_e; ++e) {
                    Y = mulmod(Y, p, n);
                }
            }
        }

        for (size_t i = 0; i < m; ++i) {
            if (X[row][i / 64] & (1ULL << (i % 64))) {
                Xv = mulmod(Xv, R[i].a, n);
            }
        }

        
        uint64_t diff = (Xv > Y) ? (Xv - Y) : (Y - Xv);
        uint64_t d1 = gcd(diff, n);
        if (d1 > 1 && d1 < n) {
            free(A); free(X); free(R);
            return d1;
        }
        
        uint64_t sum = Xv + Y;
        if (sum < Xv || sum >= n) sum -= n; 
        uint64_t d2 = gcd(sum, n);
        if (d2 > 1 && d2 < n) {
            free(A); free(X); free(R);
            return d2;
        }
    }
    
    free(A);
    free(X);
    free(R);
    return 0;
}
