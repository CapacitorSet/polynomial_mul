#include "utils.h"

void
karatsuba_SB(
    int32_t         *r, /* out - a * b in Z[x], must be length 2n */
    int32_t const   *a, /*  in - polynomial */
    int32_t const   *b, /*  in - polynomial */
    int n) /*  in - number of coefficients in a and b */
;

void
__mm256i_karatsuba__mm256_toom4(
    int32_t         *r, /* out - a * b in Z[x], must be length 2n */
    int32_t const   *a, /*  in - polynomial */
    int32_t const   *b, /*  in - polynomial */
    int n) /*  in - number of coefficients in a and b */
;