/*
 * toom4.cpp
 *
 *  Created on: Mar 28, 2017
 *      Author: zhenfei
 */
#include "32toom4.h"
#include "32sb_mul.h"
#include "32toom3.h"

int toom4_SB(int32_t *r,       /* out - a * b in Z[x], must be length 2n */
             int32_t const *a, /*  in - polynomial */
             int32_t const *b, /*  in - polynomial */
             int n)            /*  in - number of coefficients in a and b */
{
  if (n < 96) {
    grade_school_mul(r, a, b, n);
    return -1;
  }
  if (n > 384) {
    printf("degree exceeds the maximum (384) allowed\n");
    return -1;
  }
  const int s = 96, s2 = 192;
  uint16_t i;
  uint16_t x; // swap space
  int32_t const *a1 = a + s, *a2 = a + 2 * s, *a3 = a + 3 * s;
  int32_t const *b1 = b + s, *b2 = b + 2 * s, *b3 = b + 3 * s;
  int32_t *r1 = r + s, *r2 = r + 2 * s, *r4 = r + 4 * s, *r6 = r + 6 * s,
          *r7 = r + 7 * s;
  int32_t t[2*s], t3[2*s], t5[2*s];

  // +-1 ---- t: -, r2: +
  for (i = 0; i < s; i++) {
    r1[i] = a[i] + a2[i];
    x = a1[i] + a3[i];
    r[i] = r1[i] + x;
    r1[i] -= x;
    r7[i] = b[i] + b2[i];
    x = b1[i] + b3[i];
    r6[i] = r7[i] + x;
    r7[i] -= x;
  }
  grade_school_mul(t, r, r6, s);
  grade_school_mul(t5, r1, r7, s);
  for (i = 0; i < s2; i++) {
    r2[i] = (t[i] + t5[i]) >> 1;
    t[i] = (t[i] - t5[i]) >> 1;
  }
  // +-2 ---- t3: -, r4: +
  for (i = 0; i < s; i++) {
    r1[i] = a[i] + (a2[i] << 2);
    x = (a1[i] << 1) + (a3[i] << 3);
    r[i] = r1[i] + x;
    r1[i] -= x;
    r7[i] = b[i] + (b2[i] << 2);
    x = (b1[i] << 1) + (b3[i] << 3);
    r6[i] = r7[i] + x;
    r7[i] -= x;
  }
  grade_school_mul(t3, r, r6, s);
  grade_school_mul(t5, r1, r7, s);
  for (i = 0; i < s2; i++) {
    r4[i] = (t3[i] + t5[i]) >> 1;
    t3[i] = (t3[i] - t5[i]) >> 2;
  }
  // +3 ---- t5
  for (i = 0; i < s; i++) {
    r[i] = (((a3[i] * 3 + a2[i]) * 3) + a1[i]) * 3 + a[i];
    r6[i] = (((b3[i] * 3 + b2[i]) * 3) + b1[i]) * 3 + b[i];
  }
  grade_school_mul(t5, r, r6, s);  // t5 = H0+3H1+9H2+27H3+81H4+243H5+728H6
  grade_school_mul(r, a, b, s);    // r = H0
  grade_school_mul(r6, a3, b3, s); // r6 = H6
  // solve H1~H5
  for (i = 0; i < s2; i++) {
    r2[i] -= r[i] + r6[i];
    r4[i] = (((r4[i] - r[i] - (r6[i] << 6)) >> 2) - r2[i]) * 43691;
    r2[i] -= r4[i];
    t3[i] = (t3[i] - t[i]) * 43691;
    t5[i] = ((t5[i] - r[i] - 9 * (r2[i] + 9 * (r4[i] + 9 * r6[i]))) * 43691 -
             t[i]) >>
            3;
    t3[i] = (t3[i] << 1) - t5[i];
    t5[i] = ((t5[i] - t3[i]) >> 1) * 52429;
    t[i] -= t3[i] + t5[i];
  }
  for (i = 0; i < s2; i++) {
    (r + s)[i] += t[i];
    (r + 3 * s)[i] += t3[i];
    (r + 5 * s)[i] += t5[i];
  }
  return 0;
}

int toom4_toom3(int32_t *r,       /* out - a * b in Z[x], must be length 2n */
                int32_t const *a, /*  in - polynomial */
                int32_t const *b, /*  in - polynomial */
                int n)            /*  in - number of coefficients in a and b */
{
  if (n < 96) {
    grade_school_mul(r, a, b, n);
    return -1;
  }
  if (n > 384) {
    printf("degree exceeds the maximum (384) allowed\n");
    return -1;
  }
  const int s = 96, s2 = 192;
  uint16_t i;
  uint16_t x; // swap space
  int32_t const *a1 = a + s, *a2 = a + 2 * s, *a3 = a + 3 * s;
  int32_t const *b1 = b + s, *b2 = b + 2 * s, *b3 = b + 3 * s;
  int32_t *r1 = r + s, *r2 = r + 2 * s, *r4 = r + 4 * s, *r6 = r + 6 * s,
          *r7 = r + 7 * s;
  int32_t t[2*s], t3[2*s], t5[2*s];

  // +-1 ---- t: -, r2: +
  for (i = 0; i < s; i++) {
    r1[i] = a[i] + a2[i];
    x = a1[i] + a3[i];
    r[i] = r1[i] + x;
    r1[i] -= x;
    r7[i] = b[i] + b2[i];
    x = b1[i] + b3[i];
    r6[i] = r7[i] + x;
    r7[i] -= x;
  }
  toom3(t, r, r6, s);
  toom3(t5, r1, r7, s);
  for (i = 0; i < s2; i++) {
    r2[i] = (t[i] + t5[i]) >> 1;
    t[i] = (t[i] - t5[i]) >> 1;
  }
  // +-2 ---- t3: -, r4: +
  for (i = 0; i < s; i++) {
    r1[i] = a[i] + (a2[i] << 2);
    x = (a1[i] << 1) + (a3[i] << 3);
    r[i] = r1[i] + x;
    r1[i] -= x;
    r7[i] = b[i] + (b2[i] << 2);
    x = (b1[i] << 1) + (b3[i] << 3);
    r6[i] = r7[i] + x;
    r7[i] -= x;
  }
  toom3(t3, r, r6, s);
  toom3(t5, r1, r7, s);
  for (i = 0; i < s2; i++) {
    r4[i] = (t3[i] + t5[i]) >> 1;
    t3[i] = (t3[i] - t5[i]) >> 2;
  }
  // +3 ---- t5
  for (i = 0; i < s; i++) {
    r[i] = (((a3[i] * 3 + a2[i]) * 3) + a1[i]) * 3 + a[i];
    r6[i] = (((b3[i] * 3 + b2[i]) * 3) + b1[i]) * 3 + b[i];
  }
  toom3(t5, r, r6, s);  // t5 = H0+3H1+9H2+27H3+81H4+243H5+728H6
  toom3(r, a, b, s);    // r = H0
  toom3(r6, a3, b3, s); // r6 = H6
  // solve H1~H5
  for (i = 0; i < s2; i++) {
    r2[i] -= r[i] + r6[i];
    r4[i] = (((r4[i] - r[i] - (r6[i] << 6)) >> 2) - r2[i]) * 43691;
    r2[i] -= r4[i];
    t3[i] = (t3[i] - t[i]) * 43691;
    t5[i] = ((t5[i] - r[i] - 9 * (r2[i] + 9 * (r4[i] + 9 * r6[i]))) * 43691 -
             t[i]) >>
            3;
    t3[i] = (t3[i] << 1) - t5[i];
    t5[i] = ((t5[i] - t3[i]) >> 1) * 52429;
    t[i] -= t3[i] + t5[i];
  }
  for (i = 0; i < s2; i++) {
    (r + s)[i] += t[i];
    (r + 3 * s)[i] += t3[i];
    (r + 5 * s)[i] += t5[i];
  }
  return 0;
}

int toom4__mm256i_toom3(int32_t *r, /* out - a * b in Z[x], must be length 2n */
                        int32_t const *a, /*  in - polynomial */
                        int32_t const *b, /*  in - polynomial */
                        int n) /*  in - number of coefficients in a and b */
{
  if (n < 96) {
    __mm256i_toom3__mm256i_SB(r, a, b, n);
    return -1;
  }
  if (n > 384) {
    printf("degree exceeds the maximum (384) allowed\n");
    return -1;
  }
  const int s = 96, s2 = 192;
  uint16_t i;
  int32_t x; // swap space
  int32_t const *a1 = a + s, *a2 = a + 2 * s, *a3 = a + 3 * s;
  int32_t const *b1 = b + s, *b2 = b + 2 * s, *b3 = b + 3 * s;
  int32_t *r1 = r + s, *r2 = r + 2 * s, *r4 = r + 4 * s, *r6 = r + 6 * s,
          *r7 = r + 7 * s;
  int32_t t[2*s];
  int32_t t3[2*s];
  int32_t t5[2*s];
  // +-1 ---- t: -, r2: +
  for (i = 0; i < s; i++) {
    r1[i] = a[i] + a2[i];
    x = a1[i] + a3[i];
    r[i] = r1[i] + x;
    r1[i] -= x;
    r7[i] = b[i] + b2[i];
    x = b1[i] + b3[i];
    r6[i] = r7[i] + x;
    r7[i] -= x;
  }
  __mm256i_toom3__mm256i_SB(t, r, r6, s);
  __mm256i_toom3__mm256i_SB(t5, r1, r7, s);
  for (i = 0; i < s2; i++) {
    r2[i] = (t[i] + t5[i]) >> 1;
    t[i] = (t[i] - t5[i]) >> 1;
  }
  // +-2 ---- t3: -, r4: +
  for (i = 0; i < s; i++) {
    r1[i] = a[i] + (a2[i] << 2);
    x = (a1[i] << 1) + (a3[i] << 3);
    r[i] = r1[i] + x;
    r1[i] -= x;
    r7[i] = b[i] + (b2[i] << 2);
    x = (b1[i] << 1) + (b3[i] << 3);
    r6[i] = r7[i] + x;
    r7[i] -= x;
  }
  __mm256i_toom3__mm256i_SB(t3, r, r6, s);
  __mm256i_toom3__mm256i_SB(t5, r1, r7, s);
  for (i = 0; i < s2; i++) {
    r4[i] = (t3[i] + t5[i]) >> 1;
    t3[i] = (t3[i] - t5[i]) >> 2;
  }
  // +3 ---- t5
  for (i = 0; i < s; i++) {
    r[i] = (((a3[i] * 3 + a2[i]) * 3) + a1[i]) * 3 + a[i];
    r6[i] = (((b3[i] * 3 + b2[i]) * 3) + b1[i]) * 3 + b[i];
  }
  __mm256i_toom3__mm256i_SB(t5, r, r6, s); // t5 = H0+3H1+9H2+27H3+81H4+243H5+728H6
  __mm256i_toom3__mm256i_SB(r, a, b, s);    // r = H0
  __mm256i_toom3__mm256i_SB(r6, a3, b3, s); // r6 = H6
  // solve H1~H5
  for (i = 0; i < s2; i++) {
    r2[i] -= r[i] + r6[i];
    r4[i] = (((r4[i] - r[i] - (r6[i] << 6)) >> 2) - r2[i]) * 43691;
    r2[i] -= r4[i];
    t3[i] = (t3[i] - t[i]) * 43691;
    t5[i] = ((t5[i] - r[i] - 9 * (r2[i] + 9 * (r4[i] + 9 * r6[i]))) * 43691 -
             t[i]) >>
            3;
    t3[i] = (t3[i] << 1) - t5[i];
    t5[i] = ((t5[i] - t3[i]) >> 1) * 52429;
    t[i] -= t3[i] + t5[i];
  }
  for (i = 0; i < s2; i++) {
    (r + s)[i] += t[i];
    (r + 3 * s)[i] += t3[i];
    (r + 5 * s)[i] += t5[i];
  }
  return 0;
}

int __mm256i_toom4__mm256i_toom3(
    int32_t *r,       /* out - a * b in Z[x], must be length 2n */
    int32_t const *a, /*  in - polynomial */
    int32_t const *b, /*  in - polynomial */
    int n)            /*  in - number of coefficients in a and b */
{
  if (n < 96) {
    __mm256i_toom3__mm256i_SB(r, a, b, n);
    return -1;
  }
  if (n > 384) {
    printf("degree exceeds the maximum (384) allowed\n");
    return -1;
  }
  const int s = 96, s2 = 192;

  int32_t const *a1 = a + s, *a2 = a + 2 * s, *a3 = a + 3 * s;
  int32_t const *b1 = b + s, *b2 = b + 2 * s, *b3 = b + 3 * s;
  int32_t *r1 = r + s, *r2 = r + 2 * s, *r4 = r + 4 * s, *r6 = r + 6 * s,
          *r7 = r + 7 * s;
  int32_t t[2*s], t3[2*s], t5[2*s];
  // +-1 ---- t: -, r2: +
  for (int i = 0; i < s; i += 8) {
    __m256i _a = _mm256_loadu_si256((__m256i *)(a + i));
    __m256i _a1 = _mm256_loadu_si256((__m256i *)(a1 + i));
    __m256i _a2 = _mm256_loadu_si256((__m256i *)(a2 + i));
    __m256i _a3 = _mm256_loadu_si256((__m256i *)(a3 + i));

    __m256i _r1 = _mm256_add_epi32(_a, _a2);
    __m256i tmp = _mm256_add_epi32(_a1, _a3);
    __m256i _r = _mm256_add_epi32(_r1, tmp);
    _r1 = _mm256_sub_epi32(_r1, tmp);

    __m256i _b = _mm256_loadu_si256((__m256i *)(b + i));
    __m256i _b1 = _mm256_loadu_si256((__m256i *)(b1 + i));
    __m256i _b2 = _mm256_loadu_si256((__m256i *)(b2 + i));
    __m256i _b3 = _mm256_loadu_si256((__m256i *)(b3 + i));

    __m256i _r7 = _mm256_add_epi32(_b, _b2);
    tmp = _mm256_add_epi32(_b1, _b3);
    __m256i _r6 = _mm256_add_epi32(_r7, tmp);
    _r7 = _mm256_sub_epi32(_r7, tmp);

    _mm256_storeu_si256((__m256i *)(r + i), _r);
    _mm256_storeu_si256((__m256i *)(r1 + i), _r1);
    _mm256_storeu_si256((__m256i *)(r6 + i), _r6);
    _mm256_storeu_si256((__m256i *)(r7 + i), _r7);
  }

  __mm256i_toom3__mm256i_SB(t, r, r6, s);
  __mm256i_toom3__mm256i_SB(t5, r1, r7, s);
  for (int i = 0; i < s2; i += 8) {
    __m256i _t = _mm256_loadu_si256((__m256i *)(t + i));
    __m256i _t5 = _mm256_loadu_si256((__m256i *)(t5 + i));

    __m256i _r2 = _mm256_add_epi32(_t, _t5);
    _t = _mm256_sub_epi32(_t, _t5);

    _r2 = _mm256_srai_epi32(_r2, 1);
    _t = _mm256_srai_epi32(_t, 1);

    _mm256_storeu_si256((__m256i *)(r2 + i), _r2);
    _mm256_storeu_si256((__m256i *)(t + i), _t);
  }

  // +-2 ---- t3: -, r4: +
  for (int i = 0; i < s; i += 8) {
    __m256i _a = _mm256_loadu_si256((__m256i *)(a + i));
    __m256i _a1 = _mm256_loadu_si256((__m256i *)(a1 + i));
    __m256i _a2 = _mm256_loadu_si256((__m256i *)(a2 + i));
    __m256i _a3 = _mm256_loadu_si256((__m256i *)(a3 + i));

    _a2 = _mm256_slli_epi32(_a2, 2);
    _a1 = _mm256_slli_epi32(_a1, 1);
    _a3 = _mm256_slli_epi32(_a3, 3);
    __m256i _r1 = _mm256_add_epi32(_a, _a2);
    __m256i tmp = _mm256_add_epi32(_a1, _a3);
    __m256i _r = _mm256_add_epi32(_r1, tmp);
    _r1 = _mm256_sub_epi32(_r1, tmp);

    __m256i _b = _mm256_loadu_si256((__m256i *)(b + i));
    __m256i _b1 = _mm256_loadu_si256((__m256i *)(b1 + i));
    __m256i _b2 = _mm256_loadu_si256((__m256i *)(b2 + i));
    __m256i _b3 = _mm256_loadu_si256((__m256i *)(b3 + i));

    _b2 = _mm256_slli_epi32(_b2, 2);
    _b1 = _mm256_slli_epi32(_b1, 1);
    _b3 = _mm256_slli_epi32(_b3, 3);
    __m256i _r7 = _mm256_add_epi32(_b, _b2);
    tmp = _mm256_add_epi32(_b1, _b3);
    __m256i _r6 = _mm256_add_epi32(_r7, tmp);
    _r7 = _mm256_sub_epi32(_r7, tmp);

    _mm256_storeu_si256((__m256i *)(r + i), _r);
    _mm256_storeu_si256((__m256i *)(r1 + i), _r1);
    _mm256_storeu_si256((__m256i *)(r6 + i), _r6);
    _mm256_storeu_si256((__m256i *)(r7 + i), _r7);
  }

  __mm256i_toom3__mm256i_SB(t3, r, r6, s);
  __mm256i_toom3__mm256i_SB(t5, r1, r7, s);

  for (int i = 0; i < s2; i += 8) {
    __m256i _t3 = _mm256_loadu_si256((__m256i *)(t3 + i));
    __m256i _t5 = _mm256_loadu_si256((__m256i *)(t5 + i));

    __m256i _r4 = _mm256_add_epi32(_t3, _t5);
    _t3 = _mm256_sub_epi32(_t3, _t5);

    _r4 = _mm256_srai_epi32(_r4, 1);
    _t3 = _mm256_srai_epi32(_t3, 2);

    _mm256_storeu_si256((__m256i *)(r4 + i), _r4);
    _mm256_storeu_si256((__m256i *)(t3 + i), _t3);
  }
  const __m256i three = _mm256_set1_epi32(3);
  for (int i = 0; i < s; i += 8) {
    __m256i _a3 = _mm256_loadu_si256((__m256i *)(a3 + i));
    __m256i _a2 = _mm256_loadu_si256((__m256i *)(a2 + i));
    __m256i _a1 = _mm256_loadu_si256((__m256i *)(a1 + i));
    __m256i _a = _mm256_loadu_si256((__m256i *)(a + i));

    __m256i _r = _mm256_mullo_epi32(_a3, three);
    _r = _mm256_add_epi32(_r, _a2);
    _r = _mm256_mullo_epi32(_r, three);
    _r = _mm256_add_epi32(_r, _a1);
    _r = _mm256_mullo_epi32(_r, three);
    _r = _mm256_add_epi32(_r, _a);

    __m256i _b3 = _mm256_loadu_si256((__m256i *)(b3 + i));
    __m256i _b2 = _mm256_loadu_si256((__m256i *)(b2 + i));
    __m256i _b1 = _mm256_loadu_si256((__m256i *)(b1 + i));
    __m256i _b = _mm256_loadu_si256((__m256i *)(b + i));

    __m256i _r6 = _mm256_mullo_epi32(_b3, three);
    _r6 = _mm256_add_epi32(_r6, _b2);
    _r6 = _mm256_mullo_epi32(_r6, three);
    _r6 = _mm256_add_epi32(_r6, _b1);
    _r6 = _mm256_mullo_epi32(_r6, three);
    _r6 = _mm256_add_epi32(_r6, _b);

    _mm256_storeu_si256((__m256i *)(r + i), _r);
    _mm256_storeu_si256((__m256i *)(r6 + i), _r6);
  }

  __mm256i_toom3__mm256i_SB(t5, r, r6, s); // t5 = H0+3H1+9H2+27H3+81H4+243H5+728H6
  __mm256i_toom3__mm256i_SB(r, a, b, s);    // r = H0
  __mm256i_toom3__mm256i_SB(r6, a3, b3, s); // r6 = H6

  // solve H1~H5
  const __m256i magic_num = _mm256_set1_epi32(43691);
  const __m256i magic_num_2 = _mm256_set1_epi32(52429);
  const __m256i nine = _mm256_set1_epi32(9);
  for (int i = 0; i < s2; i += 8) {
    __m256i _r = _mm256_loadu_si256((__m256i *)(r + i));
    __m256i _r6 = _mm256_loadu_si256((__m256i *)(r6 + i));
    __m256i _r4 = _mm256_loadu_si256((__m256i *)(r4 + i));
    __m256i _r2 = _mm256_loadu_si256((__m256i *)(r2 + i));

    __m256i tmp = _mm256_add_epi32(_r, _r6);
    _r2 = _mm256_sub_epi32(_r2, tmp);
    tmp = _mm256_slli_epi32(_r6, 6);
    _r4 = _mm256_sub_epi32(_r4, _r);
    _r4 = _mm256_sub_epi32(_r4, tmp);
    _r4 = _mm256_srai_epi32(_r4, 2);
    _r4 = _mm256_sub_epi32(_r4, _r2);
    _r4 = _mm256_mullo_epi32(_r4, magic_num);
    _r2 = _mm256_sub_epi32(_r2, _r4);

    __m256i _t = _mm256_loadu_si256((__m256i *)(t + i));
    __m256i _t3 = _mm256_loadu_si256((__m256i *)(t3 + i));
    __m256i _t5 = _mm256_loadu_si256((__m256i *)(t5 + i));

    _t3 = _mm256_sub_epi32(_t3, _t);
    _t3 = _mm256_mullo_epi32(_t3, magic_num);
    _t5 = _mm256_sub_epi32(_t5, _r);
    tmp = _mm256_mullo_epi32(nine, _r6);
    tmp = _mm256_add_epi32(_r4, tmp);
    tmp = _mm256_mullo_epi32(nine, tmp);
    tmp = _mm256_add_epi32(_r2, tmp);
    tmp = _mm256_mullo_epi32(nine, tmp);
    _t5 = _mm256_sub_epi32(_t5, tmp);
    _t5 = _mm256_mullo_epi32(_t5, magic_num);
    _t5 = _mm256_sub_epi32(_t5, _t);
    _t5 = _mm256_srai_epi32(_t5, 3);
    _t3 = _mm256_slli_epi32(_t3, 1);
    _t3 = _mm256_sub_epi32(_t3, _t5);
    _t5 = _mm256_sub_epi32(_t5, _t3);
    _t5 = _mm256_srai_epi32(_t5, 1);
    _t5 = _mm256_mullo_epi32(_t5, magic_num_2);
    tmp = _mm256_add_epi32(_t3, _t5);
    _t = _mm256_sub_epi32(_t, tmp);

    _mm256_storeu_si256((__m256i *)(t3 + i), _t3);
    _mm256_storeu_si256((__m256i *)(t5 + i), _t5);
    _mm256_storeu_si256((__m256i *)(t + i), _t);
    _mm256_storeu_si256((__m256i *)(r2 + i), _r2);
    _mm256_storeu_si256((__m256i *)(r4 + i), _r4);
  }
  for (int i = 0; i < s2; i++) {
    (r + s)[i] += t[i];
    (r + 3 * s)[i] += t3[i];
    (r + 5 * s)[i] += t5[i];
  }
  return 0;
}
