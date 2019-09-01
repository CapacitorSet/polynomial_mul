#include "32sb_mul.h"

void grade_school_mul(
    int32_t *res1,    /* out - a * b in Z[x], must be length 2N */
    int32_t const *a, /*  in - polynomial */
    int32_t const *b, /*  in - polynomial */
    int N)            /*  in - number of coefficients in a and b */
{
  for (int j = 0; j < N; j++) {
    res1[j] = a[0] * b[j];
  }
  for (int i = 1; i < N; i++) {
    res1[i + N - 1] = 0;
    for (int j = 0; j < N; j++) {
      res1[i + j] += a[i] * b[j];
    }
  }
  res1[2 * N - 1] = 0;

  return;
}

void __m256i_grade_school_mul_32(
    int32_t *res1,    /* out - a * b in Z[x], must be length 2N */
    int32_t const *a, /*  in - polynomial */
    int32_t const *b, /*  in - polynomial */
    int N)            /*  in - number of coefficients in a and b <= 32*/
{
  int sizen = N * sizeof(int32_t);
  int32_t buf[64] = {};
  // memset(buf, 0, sizen * 2);
  memcpy(buf, a, sizen);

  int32_t *buf1 = buf + 16;
  int32_t *buf2 = buf + 32;
  int32_t *buf3 = buf + 48;

  __m256i tmp, tmp2;

  __m256i buf2_acc_hi = _mm256_set1_epi32(0);
  __m256i buf2_acc_lo = _mm256_set1_epi32(0);
  __m256i buf3_acc_hi = _mm256_set1_epi32(0);
  __m256i buf3_acc_lo = _mm256_set1_epi32(0);

  tmp = _mm256_set1_epi32(b[0]);
  // acc = accumulator
  tmp2 = _mm256_loadu_si256((__m256i *)(buf));
  __m256i buf_acc_hi = _mm256_mullo_epi32(tmp, tmp2);
  tmp2 = _mm256_loadu_si256((__m256i *)(buf + 8));
  __m256i buf_acc_lo = _mm256_mullo_epi32(tmp, tmp2);

  tmp2 = _mm256_loadu_si256((__m256i *)(buf1));
  __m256i buf1_acc_hi = _mm256_mullo_epi32(tmp, tmp2);
  tmp2 = _mm256_loadu_si256((__m256i *)(buf1 + 8));
  __m256i buf1_acc_lo = _mm256_mullo_epi32(tmp, tmp2);

  for (int i = 1; i < 16; i++) {
    tmp = _mm256_set1_epi32(b[i]);

    memmove(buf + i, buf + i - 1, sizen);
    buf[i - 1] = 0;

    tmp2 = _mm256_loadu_si256((__m256i *)(buf));
    buf_acc_hi = _mm256_add_epi32(buf_acc_hi, _mm256_mullo_epi32(tmp, tmp2));
    tmp2 = _mm256_loadu_si256((__m256i *)(buf + 8));
    buf_acc_lo = _mm256_add_epi32(buf_acc_lo, _mm256_mullo_epi32(tmp, tmp2));

    tmp2 = _mm256_loadu_si256((__m256i *)(buf1));
    buf1_acc_hi = _mm256_add_epi32(buf1_acc_hi, _mm256_mullo_epi32(tmp, tmp2));
    tmp2 = _mm256_loadu_si256((__m256i *)(buf1 + 8));
    buf1_acc_lo = _mm256_add_epi32(buf1_acc_lo, _mm256_mullo_epi32(tmp, tmp2));

    tmp2 = _mm256_loadu_si256((__m256i *)(buf2));
    buf2_acc_hi = _mm256_add_epi32(buf2_acc_hi, _mm256_mullo_epi32(tmp, tmp2));
    tmp2 = _mm256_loadu_si256((__m256i *)(buf2 + 8));
    buf2_acc_lo = _mm256_add_epi32(buf2_acc_lo, _mm256_mullo_epi32(tmp, tmp2));
  }

  tmp = _mm256_set1_epi32(b[16]);
  memmove(buf + 16, buf + 15, sizen);
  buf[15] = 0;

  tmp2 = _mm256_loadu_si256((__m256i *)(buf1));
  buf1_acc_hi = _mm256_add_epi32(buf1_acc_hi, _mm256_mullo_epi32(tmp, tmp2));
  tmp2 = _mm256_loadu_si256((__m256i *)(buf1 + 8));
  buf1_acc_lo = _mm256_add_epi32(buf1_acc_lo, _mm256_mullo_epi32(tmp, tmp2));

  tmp2 = _mm256_loadu_si256((__m256i *)(buf2));
  buf2_acc_hi = _mm256_add_epi32(buf2_acc_hi, _mm256_mullo_epi32(tmp, tmp2));
  tmp2 = _mm256_loadu_si256((__m256i *)(buf2 + 8));
  buf2_acc_lo = _mm256_add_epi32(buf2_acc_lo, _mm256_mullo_epi32(tmp, tmp2));

  for (int i = 17; i < 32; i++) {
    tmp = _mm256_set1_epi32(b[i]);

    memmove(buf + i, buf + i - 1, sizen);
    buf[i - 1] = 0;

    tmp2 = _mm256_loadu_si256((__m256i *)(buf1));
    buf1_acc_hi = _mm256_add_epi32(buf1_acc_hi, _mm256_mullo_epi32(tmp, tmp2));
    tmp2 = _mm256_loadu_si256((__m256i *)(buf1 + 8));
    buf1_acc_lo = _mm256_add_epi32(buf1_acc_lo, _mm256_mullo_epi32(tmp, tmp2));

    tmp2 = _mm256_loadu_si256((__m256i *)(buf2));
    buf2_acc_hi = _mm256_add_epi32(buf2_acc_hi, _mm256_mullo_epi32(tmp, tmp2));
    tmp2 = _mm256_loadu_si256((__m256i *)(buf2 + 8));
    buf2_acc_lo = _mm256_add_epi32(buf2_acc_lo, _mm256_mullo_epi32(tmp, tmp2));

    tmp2 = _mm256_loadu_si256((__m256i *)(buf3));
    buf3_acc_hi = _mm256_add_epi32(buf3_acc_hi, _mm256_mullo_epi32(tmp, tmp2));
    tmp2 = _mm256_loadu_si256((__m256i *)(buf3 + 8));
    buf3_acc_lo = _mm256_add_epi32(buf3_acc_lo, _mm256_mullo_epi32(tmp, tmp2));
  }

  _mm256_storeu_si256((__m256i *)res1, buf_acc_hi);
  _mm256_storeu_si256((__m256i *)res1 + 1, buf_acc_lo);
  _mm256_storeu_si256((__m256i *)res1 + 2, buf1_acc_hi);
  _mm256_storeu_si256((__m256i *)res1 + 3, buf1_acc_lo);
  _mm256_storeu_si256((__m256i *)res1 + 4, buf2_acc_hi);
  _mm256_storeu_si256((__m256i *)res1 + 5, buf2_acc_lo);
  _mm256_storeu_si256((__m256i *)res1 + 6, buf3_acc_hi);
  _mm256_storeu_si256((__m256i *)res1 + 7, buf3_acc_lo);
  return;
}
