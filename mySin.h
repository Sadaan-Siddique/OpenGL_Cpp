#include <cmath> // Needed for fmod (range reduction)

#define PI 3.141592653589793238462649

long double exponentialFunc(double base, unsigned int exponent)
{
    if (base == 0 && exponent == 0)
        return -1;
    long double power = 1;
    for(int i = 1; i <= exponent; i++)
        power *= base;
    return power;
}

unsigned long long factorialFunc(unsigned int n)
{
    unsigned long long factorial = 1;
    for(int i = n; i > 0; i--)
        factorial *=  i;
    return factorial;
}

long double my_sin(double x)
{
    x = fmod(x, 2.0 * PI);
    long double sinx = 0;
    // Summation Function of taylor series of sinx
    for (int n = 0; n < 9; n++)
      sinx += ( exponentialFunc(-1, n) * exponentialFunc(x, ((2*n) + 1)) ) / factorialFunc((2*n) + 1);
    return sinx;
}