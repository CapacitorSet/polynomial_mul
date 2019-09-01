#include "32sb_mul.h"
#include "32toom3.h"

int main() {
  const int N = 96;
  int32_t *a = new int32_t[2 * N];
  int32_t *b = new int32_t[2 * N];
  int32_t *r = new int32_t[4 * N];
  int32_t *r2 = new int32_t[4 * N];
  int32_t *r3 = new int32_t[4 * N];
  int32_t *r4 = new int32_t[4 * N];

  uint16_t i, j;
  uint16_t test_dim;
  uint64_t ss0, ss1, ss2, ss3;
  clock_t start, end;

  cout << "testing toom 3" << endl;
  for (test_dim = 65; test_dim < 96; test_dim++) {
    ss0 = 0;
    ss1 = 0;
    ss2 = 0;
    ss3 = 0;
    cout << "dimension: " << test_dim << " ";
    for (j = 0; j < 1000; j++) {
      memset(a + test_dim, 0, 2 * N * sizeof(int32_t));
      memset(b + test_dim, 0, 2 * N * sizeof(int32_t));
      for (i = 0; i < test_dim; i++) {
        a[i] = (rand() % (4 * 1024)) - (2 * 1024); // rand32();
        b[i] = rand();
      }
      start = clock();
      grade_school_mul(r, a, b, test_dim);
      end = clock();
      ss0 += end - start;

      start = clock();
      toom3(r2, a, b, test_dim);
      end = clock();
      ss1 += end - start;

      start = clock();
      toom3__mm256i_SB(r3, a, b, test_dim);
      end = clock();
      ss2 += end - start;

      start = clock();
      __mm256i_toom3__mm256i_SB(r4, a, b, test_dim);
      end = clock();
      ss3 += end - start;

      for (i = 0; i < test_dim * 2 - 1; i++) {
        if (((r[i] - r2[i]) % 2048) != 0) {
          printf("error r2\n");
          for (j = 0; j < test_dim * 2 - 1; j++) {
            printf("%d %d %d %d\n", j, r[j], r2[j], r[j] - r2[j]);
          }
          return 1;
        }
        if (((r[i] - r3[i]) % 2048) != 0) {
          printf("error r3\n");
          for (j = 0; j < test_dim * 2 - 1; j++) {
            printf("%d %d %d %d\n", j, r[j], r3[j], r[j] - r2[j]);
          }
          return 1;
        }
        if (((r[i] - r4[i]) % 2048) != 0) {
          printf("error r4\n");
          for (j = 0; j < test_dim * 2 - 1; j++) {
            printf("%d %d %d %d\n", j, r[j], r4[j], r[j] - r2[j]);
          }
          return 1;
        }
      }
    }
    cout << ss0 << " " << ss1 << " " << ss2 << " " << ss3 << endl;
  }
  return 0;
}