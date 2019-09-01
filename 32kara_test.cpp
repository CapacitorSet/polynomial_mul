#include "32kara.h"
#include "32sb_mul.h"
#include "utils.h"

int main() {
  const int N = 768;
  int32_t *a = new int32_t[2 * N];
  int32_t *b = new int32_t[2 * N];
  // int32_t *buf = new int32_t[4 * N];
  int32_t *r = new int32_t[4 * N];
  int32_t *r2 = new int32_t[4 * N];
  int32_t *r3 = new int32_t[4 * N];

  uint16_t i, j;
  uint16_t test_dim;
  uint64_t ss1, ss2, ss3;
  clock_t start, end;

  cout << "testing karatsuba 3" << endl;
  for (test_dim = 384; test_dim < 768; test_dim++) {
    ss1 = 0;
    ss2 = 0;
    ss3 = 0;
    cout << "dimension: " << test_dim << " ";
    // for (j=0;j<100;j++) {
    for (j = 0; j < 10; j++) {
      memset(a + test_dim, 0, 2 * N * sizeof(int32_t));
      memset(b + test_dim, 0, 2 * N * sizeof(int32_t));
      for (i = 0; i < test_dim; i++) {
        a[i] = (rand() % (4 * 1024)) - (2 * 1024); // rand32();
        b[i] = rand32();
      }

      /*
      printf("A=[%d", a[0]);
      for (int i = 1; i < test_dim; i++)
        printf(", %d", a[i]);
      printf("]\n");
      printf("B=[%d", b[0]);
      for (int i = 1; i < test_dim; i++)
        printf(", %d", b[i]);
      printf("]\n");
      */

      start = clock();
      grade_school_mul(r, a, b, test_dim);
      end = clock();
      ss1 += end - start;

      start = clock();
      karatsuba_SB(r2, a, b, test_dim);
      end = clock();
      ss2 += end - start;

      start = clock();
      __mm256i_karatsuba__mm256_toom4(r3, a, b, test_dim);
      end = clock();
      ss3 += end - start;

      for (i = 0; i < test_dim * 2 - 1; i++) {
        if ((r[i] - r2[i]) % 2048 != 0) {
          printf("error\n");
          for (j = 0; j < test_dim * 2 - 1; j++) {
            printf("%d %d %d %d\n", j, r[j], r2[j], (r[j] - r2[j]) % 2048);
            if (j == i)
              printf("!\n");
          }
          return 1;
        }
        if ((r[i] - r3[i]) % 2048 != 0) {
          printf("error\n");
          for (j = 0; j < test_dim * 2 - 1; j++) {
            printf("%d %d %d %d\n", j, r[j], r3[j], (r[j] - r3[j]) % 2048);
            if (j == i)
              printf("!\n");
          }
          return 1;
        }
      }
    }
    cout << "| " << ss1 << " " << ss2 << " " << ss3 << endl;
  }
  return 0;
}