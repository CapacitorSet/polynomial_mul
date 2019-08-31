#include "utils.h"

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

int32_t rand32() {
	return rand() | (int32_t(rand()) << 16);
}