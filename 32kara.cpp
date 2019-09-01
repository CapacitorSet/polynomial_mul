#include "32sb_mul.h"
#include "32kara.h"
#include "32toom4.h"
#include "utils.h"

// There is no version based on the simd sb multiplication because it seems to be as fast as the normal one.
void karatsuba_SB(
    int32_t *r,       /* out - a * b in Z[x], must be length 2n */
    int32_t const *a, /*  in - polynomial */
    int32_t const *b, /*  in - polynomial */
    int n)            /*  in - number of coefficients in a and b */
{
  if (n < 384) {
    grade_school_mul(r, a, b, n);
    return;
  }
  if (n > 768) {
    printf("degree %d exceeds the maximum (384) allowed\n", n);
    return;
  }
  uint16_t i;
  const int s = 384;
  int32_t const *a1 = a + s;
  int32_t const *b1 = b + s;
  int32_t t[s];
  int32_t t1[s];
  int32_t *r1 = r + s, *r2 = r + 2 * s, *r3 = r + 3 * s;

  for (i = 0; i < s; i += 8) {
    __m256i _a = _mm256_loadu_si256((__m256i *)(a + i));
    __m256i _a1 = _mm256_loadu_si256((__m256i *)(a1 + i));
    __m256i _b = _mm256_loadu_si256((__m256i *)(b + i));
    __m256i _b1 = _mm256_loadu_si256((__m256i *)(b1 + i));
    __m256i _r = _mm256_sub_epi32(_a, _a1);
    __m256i _r1 = _mm256_sub_epi32(_b1, _b);
    _mm256_storeu_si256((__m256i *)(r + i), _r);
    _mm256_storeu_si256((__m256i *)(r1 + i), _r1);
  }

  grade_school_mul(t, r, r1, s);
  grade_school_mul(r2, a1, b1, s);

  for (int i = 0; i < s; i += 8) {
    __m256i _r2 = _mm256_loadu_si256((__m256i *)(r2 + i));
    __m256i _t = _mm256_loadu_si256((__m256i *)(t + i));
    __m256i _r3 = _mm256_loadu_si256((__m256i *)(r3 + i));
    __m256i _t1 = _mm256_loadu_si256((__m256i *)(t1 + i));
    __m256i _r1 = _mm256_add_epi16(_t, _r2);
    _r2 = _mm256_add_epi32(_r3, _r2);
    _r2 = _mm256_add_epi32(_r2, _t1);
    _mm256_storeu_si256((__m256i *)(r1 + i), _r1);
    _mm256_storeu_si256((__m256i *)(r2 + i), _r2);
  }

  grade_school_mul(t, a, b, s);

  for (i = 0; i < s; i++) {
    r[i] = t[i];
  }
  for (i = 0; i < s; i += 8) {
    __m256i _r1 = _mm256_loadu_si256((__m256i *)(r1 + i));
    __m256i _t = _mm256_loadu_si256((__m256i *)(t + i));
    __m256i _t1 = _mm256_loadu_si256((__m256i *)(t1 + i));
    __m256i _r2 = _mm256_loadu_si256((__m256i *)(r2 + i));
    _r1 = _mm256_add_epi32(_r1, _t);
    _r1 = _mm256_add_epi32(_r1, _t1);
    _r2 = _mm256_add_epi32(_r2, _t1);
    _mm256_storeu_si256((__m256i *)(r1 + i), _r1);
    _mm256_storeu_si256((__m256i *)(r2 + i), _r2);
  }

  return;
}

void
__mm256i_karatsuba__mm256_toom4(
    int32_t         *r, /* out - a * b in Z[x], must be length 2n */
    int32_t const   *a, /*  in - polynomial */
    int32_t const   *b, /*  in - polynomial */
    int n) /*  in - number of coefficients in a and b */
{
    if (n < 384)
    {
        __mm256i_toom4__mm256i_toom3(r, a, b, n);
        return;
    }
    if (n > 768)
    {
        printf("degree %d exceeds the maximum (384) allowed\n", n);
        return;
    }
    uint16_t i;
    const int32_t s = 384;
    int32_t const *a1 = a+s;
    int32_t const *b1 = b+s;
    int32_t t[s], t1[s];
    int32_t *r1 = r+s, *r2 = r+2*s, *r3 = r+3*s;

    for (i=0; i<s; i+=8) {
      __m256i _a = _mm256_loadu_si256((__m256i *)(a+i));
      __m256i _a1 = _mm256_loadu_si256((__m256i *)(a1+i));
      __m256i _b = _mm256_loadu_si256((__m256i *)(b+i));
      __m256i _b1 = _mm256_loadu_si256((__m256i *)(b1+i));
      __m256i _r = _mm256_sub_epi32(_a, _a1);
      __m256i _r1 = _mm256_sub_epi32(_b1, _b);
      _mm256_storeu_si256((__m256i *)(r+i), _r);
      _mm256_storeu_si256((__m256i *)(r1+i), _r1);
    }
    // r = a - a1
    // r1 = b1 - b

    __mm256i_toom4__mm256i_toom3(t, r, r1, s);
    __mm256i_toom4__mm256i_toom3(r2, a1, b1, s);

    for (i=0; i<s; i+=8) {
      __m256i _r2 = _mm256_loadu_si256((__m256i *)(r2+i));
      __m256i _t = _mm256_loadu_si256((__m256i *)(t+i));
      __m256i _r3 = _mm256_loadu_si256((__m256i *)(r3+i));
      __m256i _t1 = _mm256_loadu_si256((__m256i *)(t1+i));
      __m256i _r1 = _mm256_add_epi16(_t, _r2);
      _r2 = _mm256_add_epi32(_r3, _r2);
      _r2 = _mm256_add_epi32(_r2, _t1);
      _mm256_storeu_si256((__m256i *)(r1+i), _r1);
      _mm256_storeu_si256((__m256i *)(r2+i), _r2);
    }
    // r1 = t + r2 = (r*r1) + (a1*b1) = (a-a1)(b1-b) + (a1*b1)

    __mm256i_toom4__mm256i_toom3(t, a, b, s);

    for (i=0; i<s; i++) {
      r[i] = t[i];
    }

    for (i=0; i<s; i+=8) {
      __m256i _r1 = _mm256_loadu_si256((__m256i *)(r1+i));
      __m256i _t = _mm256_loadu_si256((__m256i *)(t+i));
      __m256i _t1 = _mm256_loadu_si256((__m256i *)(t1+i));
      __m256i _r2 = _mm256_loadu_si256((__m256i *)(r2+i));
      _r1 = _mm256_add_epi32(_r1, _t);
      _r1 = _mm256_add_epi32(_r1, _t1);
      _r2 = _mm256_add_epi32(_r2, _t1);
      _mm256_storeu_si256((__m256i *)(r1+i), _r1);
      _mm256_storeu_si256((__m256i *)(r2+i), _r2);
    }
    return;
}

///////////////

/*
void
karatsuba_toom4(
    int32_t         *r, /* out - a * b in Z[x], must be length 2n
    int32_t         *t, /*  in - n coefficients of scratch space
    int32_t const   *a, /*  in - polynomial
    int32_t const   *b, /*  in - polynomial
    int n) /*  in - number of coefficients in a and b

{
    if (n < 32)
    {
        grade_school_mul(r, a, b, n);
        return;
    }
    uint16_t i;
    uint16_t s = n/2;
    int32_t const *a1 = a+s;
    int32_t const *b1 = b+s;
    int32_t *t1 = t+s;
    int32_t *r1 = r+s, *r2 = r+2*s, *r3 = r+3*s;
    for(i=0; i<s; i++)
    {
        r[i] = a[i]-a1[i];
        r1[i] = b1[i]-b[i];
    }
    toom4_toom3(t, r2, r, r1, s);
    toom4_toom3(r2, r, a1, b1, s);
    for(i=0; i<s; i++)
    {
        r1[i] = r2[i] + t[i];
        r2[i] += r3[i] + t1[i];
    }
    toom4_toom3(t, r, a, b, s);
    for(i=0; i<s; i++)
    {
        r[i] = t[i];
        r1[i] += t[i] + t1[i];
        r2[i] += t1[i];
    }
    return;
}
*/