#include "32sb_mul.h"
#include "32toom4.h"
#include "utils.h"

int main() {
  const int N = 384;
  int32_t *a = new int32_t[2 * N];
  int32_t *b = new int32_t[2 * N];
  int32_t *buf = new int32_t[4 * N];
  int32_t *r = new int32_t[4 * N];
  int32_t *r2 = new int32_t[4 * N];
  int32_t *r3 = new int32_t[4 * N];
  uint16_t i, j;
  uint16_t test_dim;
  float ss0, ss1, ss2, ss3;
  clock_t start, end;

  cout << "testing toom 4" << endl;
  for (test_dim = 289; test_dim < 384; test_dim++) {
    ss0 = 0;
    ss1 = 0;
    ss2 = 0;
    ss3 = 0;
    cout << "dimension: " << test_dim << " ";
    for (j = 0; j < 1000; j++) {
      memset(a + test_dim, 0, 2 * N * sizeof(uint16_t));
      memset(b + test_dim, 0, 2 * N * sizeof(uint16_t));
      for (i = 0; i < test_dim; i++) {
        a[i] = abs((rand() % (4 * 1024)) - (2 * 1024)); // rand32();
        b[i] = abs(rand() & 1);
      }
      start = clock();
      grade_school_mul(r, a, b, test_dim);
      end = clock();
      ss0 += end - start;

      start = clock();
      toom4_SB(r2, buf, a, b, test_dim);
      end = clock();
      ss1 += end - start;

      start = clock();
      toom4_toom3(r3, buf, a, b, test_dim);
      // toom4__mm256i_toom3(r2, buf, a, b, test_dim);
      end = clock();
      ss2 += end - start;

      start = clock();
      // __mm256i_toom4__mm256i_toom3(r2, buf, a, b, test_dim);
      end = clock();
      ss3 += end - start;

      for (i = 0; i < test_dim * 2 - 1; i++) {
        if (((r[i] - r2[i]) % 2048) != 0) {
          printf("error\n");
          for (j = 0; j < test_dim * 2 - 1; j++) {
            printf("%d %d %d %d\n", j, r[j], r2[j], r[j] - r2[j]);
          }
          return 1;
        }
        if (((r[i] - r3[i]) % 2048) != 0) {
          printf("error\n");
          for (j = 0; j < test_dim * 2 - 1; j++) {
            printf("%d %d %d %d\n", j, r[j], r3[j], r[j] - r2[j]);
          }
          return 1;
        }
      }
    }
    cout << ss0 << " " << ss1 << " " << ss2 << " " << ss3 << endl;
  }
  return 0;
}