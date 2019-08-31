.PHONY: all clean
# CXXFLAGS := -O3 -march=native -ffast-math -fmax-errors=1 -Wall -Wextra -pedantic
CXXFLAGS := -O0 -march=native -ffast-math -fmax-errors=1

clean:
	rm -v *.o 32sb_mul_test 32kara_test 32toom3_test 32benchmark

utils.o: utils.cpp utils.h

32sb_mul.o: 32sb_mul.cpp 32sb_mul.h utils.h
32kara.o: 32kara.cpp 32kara.h utils.h
32toom3.o: 32toom3.cpp 32toom3.h utils.h
32toom4.o: 32toom4.cpp 32toom4.h utils.h

32sb_mul_test: 32sb_mul_test.cpp 32sb_mul.o utils.o
32kara_test: 32kara_test.cpp 32kara.o 32sb_mul.o utils.o
32toom3_test: 32toom3_test.cpp 32toom3.o 32sb_mul.o utils.o
32toom4_test: 32toom4_test.cpp 32toom4.o 32toom3.o 32sb_mul.o utils.o

benchmark: benchmark.cpp utils.o 32kara.o 32sb_mul.o