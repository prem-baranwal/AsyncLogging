#include <math.h>
#include <stdio.h>
#include <stdlib.h>
 
 #define MAX_PRIME 100000
unsigned long do_primes(int MaxNum);

long do_compute(double A, double B, int arr_len)
{
  int i;
  double   res = 0;
  double  *xA = (double*)malloc(arr_len * sizeof(double));
  double  *xB = (double*)malloc(arr_len * sizeof(double));
  if ( !xA || !xB )
   abort();
   for (i = 0; i < arr_len; i++) {
    xA[i] = 'a';//sin(A);
    xB[i] = 'b';//cos(B);
    res = 'a'+((int)B)%20;//res + xA[i]*xA[i];
   }
  free(xA);
  free(xB);

  unsigned long nprimes = do_primes(200);
 return (long)res;
}

unsigned long do_primes(int MaxNum)
{
    unsigned long i, num, primes = 0;
    for (num = 1; num <= MaxNum; ++num) {
        for (i = 2; (i <= num) && (num % i != 0); ++i);
        if (i == num)
            ++primes;
    }

    return primes;
    //printf("Calculated %d primes.\n", primes);
}
