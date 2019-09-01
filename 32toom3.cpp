#include "32toom3.h"
#include "32sb_mul.h"

int toom3(int32_t *r,       /* out - a * b in Z[x], must be length 2n */
          int32_t *t,       /*  in - n coefficients of scratch space */
          int32_t const *a, /*  in - polynomial */
          int32_t const *b, /*  in - polynomial */
          int n)            /*  in - number of coefficients in a and b */
{
  if (n > 96) {
    printf("degree exceeds the maximum (96) allowed\n");
    return -1;
  }
  if (n <= 2) {
    grade_school_mul(r, a, b, n);
    return 0;
  }

  uint16_t i;
  // uint16_t s = 1;
  // uint16_t s2 = 2;
  uint16_t s = 32;
  uint16_t s2 = 64;
  int32_t const *a1 = a + s, *a2 = a + s2;
  int32_t const *b1 = b + s, *b2 = b + s2;
  int32_t *r1 = r + s, *r2 = r + s2, *r3 = r + s * 3, *r4 = r + s * 4,
          *r5 = r + s * 5;
  int32_t *t2 = t + s2, *t4 = t + s2 * 2, *t6 = t + s2 * 3, *t8 = t + s2 * 4;

  /*
   * t  = w0   = a(0) * b(0)
   * t8 = w4   = a(inf) * b(inf)
   *           = a2     * b2
   */

  grade_school_mul(t, a, b, s);
  grade_school_mul(t8, a2, b2, s);

#ifdef DEBUG
  printf("t:");
  print32poly(t);
  print32poly(t + 32);
  printf("t8:");
  print32poly(t8);
  print32poly(t8 + 32);
#endif
  /*
   * t2 = a(1) *b(1)
   *    = (a0+a1+a2)*(b0+b1+b2)
   *    = r         * r2
   * t4 = a(-1) * b(-1)
   *    = (a0-a1+a2)*(b0-b1+b2)
   *    = r1        * r3
   */
  for (i = 0; i < s; i++) {
    r[i] = a[i] + a2[i];
    r1[i] = r[i] - a1[i];
    r[i] = r[i] + a1[i];
    r2[i] = b[i] + b2[i];
    r3[i] = r2[i] - b1[i];
    r2[i] = r2[i] + b1[i];
  }
  grade_school_mul(t2, r, r2, s);
  grade_school_mul(t4, r1, r3, s);

#ifdef DEBUG
  printf("t2:");
  print32poly(t2);
  print32poly(t2 + 32);
  printf("t4:");
  print32poly(t4);
  print32poly(t4 + 32);
#endif

  /*
   * r  = (t2+t4)/2 - w0 - w4
   *    = w2
   * r2 = (t2-t4)/2
   *    = w1 + w3
   */
  for (i = 0; i < s2; i++) {
    r[i] = (t2[i] + t4[i]) / 2 - t[i] - t8[i];
    r2[i] = (t2[i] - t4[i]) / 2;
  }

#ifdef DEBUG
  printf("r:");
  print32poly(r);
  print32poly(r + 32);
  printf("r2:");
  print32poly(r2);
  print32poly(r2 + 32);
#endif

  /*
   * t6 = a(2) *b(2)
   *    = (a0+2a1+4a2)*(b0+2b1+4b2)
   *    = r4          * r5
   */
  for (i = 0; i < s; i++) {
    r4[i] = a[i] + 2 * a1[i] + 4 * a2[i];
    r5[i] = b[i] + 2 * b1[i] + 4 * b2[i];
  }
  grade_school_mul(t6, r4, r5, s);

  /*
   * t6 = w1 + 4*w3
   *    = (t6 - w0 - 4*w2 - 16*w4)/2
   *    = (t6 - t0 - 4*r  - 16*t8)/2
   */

  for (i = 0; i < s2; i++) {
    t6[i] = (t6[i] - t[i] - 4 * r[i] - 16 * t8[i]) / 2;
  }

#ifdef DEBUG
  printf("t6:");
  print32poly(t6);
  print32poly(t6 + 32);
#endif

  /*
   * t2 = w3 = (t6 - r2)/3
   *    = (t6 - t4) * 43691
   * t4 = w1 = (4*r2-t6)/3
   *    = (4*r4-t6) * 43691
   */
  for (i = 0; i < s2; i++) {
    t2[i] = (t6[i] - r2[i]) * 43691;
    t4[i] = ((4 * r2[i] - t6[i]) * 43691);
  }
#ifdef DEBUG
  printf("t2:");
  print32poly(t2);
  print32poly(t2 + 32);
  printf("t4:");
  print32poly(t4);
  print32poly(t4 + 32);
#endif

  /*
   * now we have
   *  t0 = w0
   *  t4 = w1
   *  r  = w2
   *  t2 = w3
   *  t8 = w4
   * putting them back
   */

  memcpy(r2, r, sizeof(int32_t) * s2);
  memcpy(r, t, sizeof(int32_t) * s2);
  memcpy(r4, t8, sizeof(int32_t) * s2);

  for (i = 0; i < s2; i++) {
    r1[i] += t4[i];
    r3[i] += t2[i];
  }

  return 0;
}

int toom3__mm256i_SB(int32_t *r, /* out - a * b in Z[x], must be length 2n */
                     int32_t *t, /*  in - n coefficients of scratch space */
                     int32_t const *a, /*  in - polynomial */
                     int32_t const *b, /*  in - polynomial */
                     int n) /*  in - number of coefficients in a and b */
{
  if (n > 96) {
    printf("degree exceeds the maximum (96) allowed\n");
    return -1;
  }
  if (n <= 32) {
    grade_school_mul(r, a, b, n);
    return 0;
  }

  uint16_t i;
  uint16_t s = 32, s2 = 64;
  int32_t const *a1 = a + s, *a2 = a + s2;
  int32_t const *b1 = b + s, *b2 = b + s2;
  int32_t *r1 = r + s, *r2 = r + s2, *r3 = r + s * 3, *r4 = r + s * 4,
          *r5 = r + s * 5;
  int32_t *t2 = t + s2, *t4 = t + s2 * 2, *t6 = t + s2 * 3, *t8 = t + s2 * 4;
  int32_t *buf = t + s2 * 10;

  /*
   * t  = w0   = a(0) * b(0)
   * t8 = w4   = a(inf) * b(inf)
   *           = a2     * b2
   */

  // grade_school_mul(t, a, b, s);
  // grade_school_mul(t8, a2, b2, s);

  __m256i_grade_school_mul_32(t, buf, a, b, s);
  __m256i_grade_school_mul_32(t8, buf, a2, b2, s);
#ifdef DEBUG
  printf("t:");
  print32poly(t);
  print32poly(t + 32);
  printf("t8:");
  print32poly(t8);
  print32poly(t8 + 32);
#endif
  /*
   * t2 = a(1) *b(1)
   *    = (a0+a1+a2)*(b0+b1+b2)
   *    = r         * r2
   * t4 = a(-1) * b(-1)
   *    = (a0-a1+a2)*(b0-b1+b2)
   *    = r1        * r3
   */
  for (i = 0; i < s; i++) {
    r[i] = a[i] + a2[i];
    r1[i] = r[i] - a1[i];
    r[i] = r[i] + a1[i];
    r2[i] = b[i] + b2[i];
    r3[i] = r2[i] - b1[i];
    r2[i] = r2[i] + b1[i];
  }
  // grade_school_mul(t2, r, r2, s);
  // grade_school_mul(t4, r1, r3, s);

  __m256i_grade_school_mul_32(t2, buf, r, r2, s);
  __m256i_grade_school_mul_32(t4, buf, r1, r3, s);

#ifdef DEBUG
  printf("t2:");
  print32poly(t2);
  print32poly(t2 + 32);
  printf("t4:");
  print32poly(t4);
  print32poly(t4 + 32);
#endif

  /*
   * r  = (t2+t4)/2 - w0 - w4
   *    = w2
   * r2 = (t2-t4)/2
   *    = w1 + w3
   */
  for (i = 0; i < s2; i++) {
    r[i] = (t2[i] + t4[i]) / 2 - t[i] - t8[i];
    r2[i] = (t2[i] - t4[i]) / 2;
  }

#ifdef DEBUG
  printf("r:");
  print32poly(r);
  print32poly(r + 32);
  printf("r2:");
  print32poly(r2);
  print32poly(r2 + 32);
#endif

  /*
   * t6 = a(2) *b(2)
   *    = (a0+2a1+4a2)*(b0+2b1+4b2)
   *    = r4          * r5
   */
  for (i = 0; i < s; i++) {
    r4[i] = a[i] + 2 * a1[i] + 4 * a2[i];
    r5[i] = b[i] + 2 * b1[i] + 4 * b2[i];
  }
  // grade_school_mul(t6, r4, r5, s);
  __m256i_grade_school_mul_32(t6, buf, r4, r5, s);
  ;

  /*
   * t6 = w1 + 4*w3
   *    = (t6 - w0 - 4*w2 - 16*w4)/2
   *    = (t6 - t0 - 4*r  - 16*t8)/2
   */

  for (i = 0; i < s2; i++) {
    t6[i] = (t6[i] - t[i] - 4 * r[i] - 16 * t8[i]) / 2;
  }

#ifdef DEBUG
  printf("t6:");
  print32poly(t6);
  print32poly(t6 + 32);
#endif

  /*
   * t2 = w3 = (t6 - r2)/3
   *    = (t6 - t4) * 43691
   * t4 = w1 = (4*r2-t6)/3
   *    = (4*r4-t6) * 43691
   */
  for (i = 0; i < s2; i++) {
    t2[i] = (t6[i] - r2[i]) * 43691;
    t4[i] = ((4 * r2[i] - t6[i]) * 43691);
  }
#ifdef DEBUG
  printf("t2:");
  print32poly(t2);
  print32poly(t2 + 32);
  printf("t4:");
  print32poly(t4);
  print32poly(t4 + 32);
#endif

  /*
   * now we have
   *  t0 = w0
   *  t4 = w1
   *  r  = w2
   *  t2 = w3
   *  t8 = w4
   * putting them back
   */

  memcpy(r2, r, sizeof(int32_t) * s2);
  memcpy(r, t, sizeof(int32_t) * s2);
  memcpy(r4, t8, sizeof(int32_t) * s2);

  for (i = 0; i < s2; i++) {
    r1[i] += t4[i];
    r3[i] += t2[i];
  }

  return 0;
}

int __mm256i_toom3__mm256i_SB(
    int32_t *r,       /* out - a * b in Z[x], must be length 2n */
    int32_t *t,       /*  in - n coefficients of scratch space */
    int32_t const *a, /*  in - polynomial */
    int32_t const *b, /*  in - polynomial */
    int n)            /*  in - number of coefficients in a and b */
{
  if (n > 96) {
    printf("degree exceeds the maximum (96) allowed\n");
    return -1;
  }
  if (n <= 32) {
    grade_school_mul(r, a, b, n);
    return 0;
  }

  uint16_t i;
  const int s = 32, s2 = 64;
  int32_t const *a1 = a + s, *a2 = a + s2;
  int32_t const *b1 = b + s, *b2 = b + s2;
  int32_t *r1 = r + s, *r2 = r + s2, *r3 = r + s * 3, *r4 = r + s * 4,
          *r5 = r + s * 5;
  int32_t t2[s2];
  int32_t t4[s2];
  int32_t t6[s2];
  int32_t t8[s2];
  int32_t buf[s2];

  /*
   * t  = w0   = a(0) * b(0)
   * t8 = w4   = a(inf) * b(inf)
   *           = a2     * b2
   */

  // grade_school_mul(t, a, b, s);
  // grade_school_mul(t8, a2, b2, s);

  __m256i_grade_school_mul_32(t, buf, a, b, s);
  __m256i_grade_school_mul_32(t8, buf, a2, b2, s);
#ifdef DEBUG
  printf("t:");
  print32poly(t);
  print32poly(t + 32);
  printf("t8:");
  print32poly(t8);
  print32poly(t8 + 32);
#endif
  /*
   * t2 = a(1) *b(1)
   *    = (a0+a1+a2)*(b0+b1+b2)
   *    = r         * r2
   * t4 = a(-1) * b(-1)
   *    = (a0-a1+a2)*(b0-b1+b2)
   *    = r1        * r3
   * t6 = a(2) *b(2)
   *    = (a0+2a1+4a2)*(b0+2b1+4b2)
   *    = r4          * r5
   */
  for (int i = 0; i < s; i += 8) {
    __m256i _a = _mm256_loadu_si256((__m256i *)(a + i));
    __m256i _a1 = _mm256_loadu_si256((__m256i *)(a1 + i));
    __m256i _a2 = _mm256_loadu_si256((__m256i *)(a2 + i));
    __m256i _r = _mm256_add_epi32(_a, _a2);
    __m256i _r1 = _mm256_sub_epi32(_r, _a1);
    _r = _mm256_add_epi32(_r, _a1);
    _a1 = _mm256_slli_epi32(_a1, 1);
    _a2 = _mm256_slli_epi32(_a2, 2);
    __m256i _r4 = _mm256_add_epi32(_a, _a1);
    _r4 = _mm256_add_epi32(_r4, _a2);

    __m256i _b = _mm256_loadu_si256((__m256i *)(b + i));
    __m256i _b1 = _mm256_loadu_si256((__m256i *)(b1 + i));
    __m256i _b2 = _mm256_loadu_si256((__m256i *)(b2 + i));
    __m256i _r2 = _mm256_add_epi32(_b, _b2);
    __m256i _r3 = _mm256_sub_epi32(_r2, _b1);
    _r2 = _mm256_add_epi32(_r2, _b1);
    _b1 = _mm256_slli_epi32(_b1, 1);
    _b2 = _mm256_slli_epi32(_b2, 2);
    __m256i _r5 = _mm256_add_epi32(_b, _b1);
    _r5 = _mm256_add_epi32(_r5, _b2);

    _mm256_storeu_si256((__m256i *)(r + i), _r);
    _mm256_storeu_si256((__m256i *)(r1 + i), _r1);
    _mm256_storeu_si256((__m256i *)(r2 + i), _r2);
    _mm256_storeu_si256((__m256i *)(r3 + i), _r3);
    _mm256_storeu_si256((__m256i *)(r4 + i), _r4);
    _mm256_storeu_si256((__m256i *)(r5 + i), _r5);
  }
  // grade_school_mul(t2, r, r2, s);
  // grade_school_mul(t4, r1, r3, s);

  __m256i_grade_school_mul_32(t2, buf, r, r2, s);
  __m256i_grade_school_mul_32(t4, buf, r1, r3, s);

#ifdef DEBUG
  printf("t2:");
  print32poly(t2);
  print32poly(t2 + 32);
  printf("t4:");
  print32poly(t4);
  print32poly(t4 + 32);
#endif

  /*
   * r  = (t2+t4)/2 - w0 - w4
   *    = w2
   * r2 = (t2-t4)/2
   *    = w1 + w3
   */
  for (int i = 0; i < s2; i += 8) {
    __m256i _t2 = _mm256_loadu_si256((__m256i *)(t2 + i));
    __m256i _t4 = _mm256_loadu_si256((__m256i *)(t4 + i));
    __m256i _t8 = _mm256_loadu_si256((__m256i *)(t8 + i));
    __m256i _t = _mm256_loadu_si256((__m256i *)(t + i));

    __m256i _r = _mm256_add_epi32(_t2, _t4);
    _r = _mm256_srai_epi32(_r, 1);
    _r = _mm256_sub_epi32(_r, _t);
    _r = _mm256_sub_epi32(_r, _t8);
    __m256i _r2 = _mm256_sub_epi32(_t2, _t4);
    _r2 = _mm256_srai_epi32(_r2, 1);

    _mm256_storeu_si256((__m256i *)(r + i), _r);
    _mm256_storeu_si256((__m256i *)(r2 + i), _r2);
  }

#ifdef DEBUG
  printf("r:");
  print32poly(r);
  print32poly(r + 32);
  printf("r2:");
  print32poly(r2);
  print32poly(r2 + 32);
#endif

  // grade_school_mul(t6, r4, r5, s);
  __m256i_grade_school_mul_32(t6, buf, r4, r5, s);
  ;

  /*
   * t6 = w1 + 4*w3
   *    = (t6 - w0 - 4*w2 - 16*w4)/2
   *    = (t6 - t0 - 4*r  - 16*t8)/2
   */
  const __m256i magic_num = _mm256_set1_epi32(43691);
  for (i = 0; i < s2; i += 8) {
    __m256i _t = _mm256_loadu_si256((__m256i *)(t + i));
    __m256i _t6 = _mm256_loadu_si256((__m256i *)(t6 + i));
    __m256i _t8 = _mm256_loadu_si256((__m256i *)(t8 + i));
    __m256i _r = _mm256_loadu_si256((__m256i *)(r + i));

    _r = _mm256_slli_epi32(_r, 1);
    _t8 = _mm256_slli_epi32(_t8, 3);
    __m256i tmp = _mm256_sub_epi32(_t6, _t);
    __m256i tmp2 = _mm256_add_epi32(_r, _t8);
    tmp = _mm256_srai_epi32(tmp, 1);
    _t6 = _mm256_sub_epi32(tmp, tmp2);

    /*
        _mm256_storeu_si256((__m256i *)(t6 + i), _t6);
      }

    #ifdef DEBUG
      printf("t6:");
      print32poly(t6);
      print32poly(t6 + 32);
    #endif

      // t2 = w3 = (t6 - r2)/3
      //    = (t6 - t4) * 43691
      // t4 = w1 = (4*r2-t6)/3
      //    = (4*r4-t6) * 43691
      for (i = 0; i < s2; i += 8) {
        __m256i _t6 = _mm256_loadu_si256((__m256i *)(t6 + i));
        */
    __m256i _r2 = _mm256_loadu_si256((__m256i *)(r2 + i));

    __m256i _t2 = _mm256_sub_epi32(_t6, _r2);
    _r2 = _mm256_slli_epi32(_r2, 2);
    _t2 = _mm256_mullo_epi32(_t2, magic_num);
    __m256i _t4 = _mm256_sub_epi32(_r2, _t6);
    _t4 = _mm256_mullo_epi32(_t4, magic_num);

    _mm256_storeu_si256((__m256i *)(t2 + i), _t2);
    _mm256_storeu_si256((__m256i *)(t4 + i), _t4);
  }
#ifdef DEBUG
  printf("t2:");
  print32poly(t2);
  print32poly(t2 + 32);
  printf("t4:");
  print32poly(t4);
  print32poly(t4 + 32);
#endif

  /*
   * now we have
   *  t0 = w0
   *  t4 = w1
   *  r  = w2
   *  t2 = w3
   *  t8 = w4
   * putting them back
   */

  memcpy(r2, r, sizeof(int32_t) * s2);
  memcpy(r, t, sizeof(int32_t) * s2);
  memcpy(r4, t8, sizeof(int32_t) * s2);

  for (i = 0; i < s2; i += 8) {
    __m256i _r1 = _mm256_loadu_si256((__m256i *)(r1 + i));
    __m256i _r3 = _mm256_loadu_si256((__m256i *)(r3 + i));
    __m256i _t4 = _mm256_loadu_si256((__m256i *)(t4 + i));
    __m256i _t2 = _mm256_loadu_si256((__m256i *)(t2 + i));

    _r1 = _mm256_add_epi32(_r1, _t4);
    _r3 = _mm256_add_epi32(_r3, _t2);

    _mm256_storeu_si256((__m256i *)(r1 + i), _r1);
    _mm256_storeu_si256((__m256i *)(r3 + i), _r3);
  }

  return 0;
}