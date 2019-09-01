#include "utils.h"

void
grade_school_mul(
    int32_t        *res1,  /* out - a * b in Z[x], must be length 2N */
    int32_t const  *a,     /*  in - polynomial */
    int32_t const  *b,     /*  in - polynomial */
    int N)     /*  in - number of coefficients in a and b */
;

void
__m256i_grade_school_mul_32(
    int32_t        *res1,  /* out - a * b in Z[x], must be length 2N */
    int32_t const  *a,     /*  in - polynomial */
    int32_t const  *b,     /*  in - polynomial */
    int N)     /*  in - number of coefficients in a and b <= 32*/
;