#include "utils.h"

int toom3(int32_t *r,       /* out - a * b in Z[x], must be length 2n */
          int32_t *t,       /*  in - n coefficients of scratch space */
          int32_t const *a, /*  in - polynomial */
          int32_t const *b, /*  in - polynomial */
          int n)            /*  in - number of coefficients in a and b */
    ;

int toom3__mm256i_SB(int32_t *r, /* out - a * b in Z[x], must be length 2n */
                     int32_t *t, /*  in - n coefficients of scratch space */
                     int32_t const *a, /*  in - polynomial */
                     int32_t const *b, /*  in - polynomial */
                     int n) /*  in - number of coefficients in a and b */
    ;

int __mm256i_toom3__mm256i_SB(
    int32_t *r,       /* out - a * b in Z[x], must be length 2n */
    int32_t *t,       /*  in - n coefficients of scratch space */
    int32_t const *a, /*  in - polynomial */
    int32_t const *b, /*  in - polynomial */
    int n)            /*  in - number of coefficients in a and b */
    ;