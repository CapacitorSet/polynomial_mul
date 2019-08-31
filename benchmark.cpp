#include "32kara.h"
#include "32sb_mul.h"
#include "utils.h"

int main() {
  const int N = 768;
  int32_t *a = new int32_t[2 * N];
  int32_t *b = new int32_t[2 * N];
  int32_t *buf = new int32_t[4 * N];
  int32_t *r = new int32_t[4 * N];
  int32_t *r2 = new int32_t[4 * N];
  // int32_t *r3 = new int32_t[4 * N];

  uint16_t i, j;
  uint64_t ss0, ss1, ss2, ss3;
  clock_t start, end;
  uint64_t startc, endc;

  ss2 = 0;
  ss3 = 0;
  for (j = 0; j < 1000; j++) {
    memset(a, 0, 2 * N * sizeof(int32_t));
    memset(b, 0, 2 * N * sizeof(int32_t));
    memset(buf, 0, 4 * N * sizeof(int32_t));
    memset(r, 0, 4 * N * sizeof(int32_t));
    for (i = 0; i < N; i++) {
      a[i] = (rand() % (4 * 1024)) - (2 * 1024); // rand32();
      b[i] = rand32();
    }

    start = clock();
    startc = rdtsc();
    // __mm256i_karatsuba__mm256_toom4(r, buf, a, b, N);
    karatsuba_SB(r, buf, a, b, N);
    endc = rdtsc();
    end = clock();
    ss3 += end - start;
    ss0 += (endc - startc);
    cout << (end - start) << " " << (endc - startc) << " ";

    start = clock();
    startc = rdtsc();
    grade_school_mul(r2, a, b, N);
    endc = rdtsc();
    end = clock();
    ss2 += end - start;
    cout << (end - start) << " " << (endc - startc) << endl;
    ss1 += (endc - startc);

    for (i = 0; i < N * 2 - 1; i++) {
      if ((r[i] - r2[i]) % 2048 != 0) {
        printf("error\n");
        for (j = 0; j < N * 2 - 1; j++) {
          printf("%d %d %d %d\n", j, r[j], r2[j], (r[j] - r2[j]) % 2048);
          if (j == i)
            printf("!\n");
        }
        return 1;
      }
    }
  }
  cout << endl;
  cout << ss3 << " " << ss0 << " " << ss2 << " " << ss1 << endl;
}
