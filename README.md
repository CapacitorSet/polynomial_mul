# polynomial_mul: fast polynomial multiplication

Fast polynomial multiplications using AVX2. Based on work by Zhenfei Zhang, ported to int32_t by CapacitorSet, GPLv3 licensed.

Usage: `make`.

It was used to [experiment](https://github.com/tfhe/tfhe/issues/202) alternative implementations of negacyclic convolution in TFHE; because the experiment proved unfruitful (3x slower than FFT for int32 polynomials with degree 1000), this project will **no longer be developed**.