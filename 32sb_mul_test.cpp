#include "32sb_mul.h"
#include "utils.h"
#include <iostream>

using namespace std;

int main() {
  const int N = 32;
  int32_t *a = new int32_t[2 * N];
  int32_t *b = new int32_t[2 * N];
  int32_t *buf = new int32_t[4 * N];
  int32_t *r = new int32_t[4 * N];
  int32_t *r2 = new int32_t[4 * N];

  for (int test_dim = 8; test_dim < 32; test_dim++) {
    uint64_t ss1 = 0, ss2 = 0, ss3 = 0;
    ss1 = 0;
    ss2 = 0;
    ss3 = 0;
    cout << "dimension: " << test_dim << " ";
    for (int j=0;j<100000;j++) {
    // for (int j = 0; j < 1; j++) {
      memset(a + test_dim, 0, 2 * N * sizeof(int32_t));
      memset(b + test_dim, 0, 2 * N * sizeof(int32_t));
      for (int i = 0; i < test_dim; i++) {
        a[i] = (rand() % (4 * 1024)) - (2 * 1024); // rand32();
        b[i] = rand32();
      }

      clock_t start, end;

      start = rdtsc();
      grade_school_mul(r, a, b, test_dim);
      end = rdtsc();
      ss1 += (end - start);

      start = rdtsc();
      // karatsuba_old16(r2, buf,a, b, test_dim);
      end = rdtsc();
      ss3 += (end - start);

      //            start = clock();
      //            __m256i_grade_school_mul_32_old(r2, buf,a, b, test_dim);
      //            end = clock();
      //            ss1 += (float)(end-start);

      start = rdtsc();
      __m256i_grade_school_mul_32(r2, buf, a, b, test_dim);
      end = rdtsc();
      ss2 += (end - start);

      for (int i = 0; i < test_dim * 2 - 1; i++) {
        if (r[i] != r2[i]) {
          printf("error\n");
          for (int j = 0; j < test_dim * 2 - 1; j++) {
            printf("%d %d %d %d\n", j, r[j], r2[j], r[j] - r2[j]);
          }
          return 1;
        }
      }
    }
    cout << ss1 / 100000 << " " << ss3 / 100000 << " " << ss2 / 100000 << " "
         << endl;
  }
  return 0;
}