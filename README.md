# polynomial_mul

Fast polynomial multiplications using avx2. Based on work by Zhenfei Zhang, ported to int32_t by CapacitorSet, GPLv3 licensed.

Usage: `make`.

This piece of code handles polynomial multiplications modulo q with 
q = 2^k for k < 14. It is most efficient for polynomials whose degrees
are between 512 and 768.

It uses a hybrid of the following algorithms:
* index based school book multiplication for polynomials with degree 
less than 32

* Toom-3 multiplicaiton for polynomials with degree 
< 96. 

* Toom-4 multiplicaiton for polynomials with degree 
< 384.

* karatsuba multiplicaiton for polynomials with degree 
< 768. 


**Old data - todo: update**

Benchmark on NTRU polynomials:
karatsuba_old: 290 k cycles
this program (with avx2):  130 k cycles
this program (w/o avx2):  180 k cycles