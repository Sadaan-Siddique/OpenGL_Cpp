#include <iostream>
#include <cmath>
using std::cout, std::endl;

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

long double another_sin(double x)
{
    // 1. Range Reduction:
    // Taylor series gets highly inaccurate for huge numbers.
    // This safely loops any massive angle back down into the standard -2PI to 2PI range.
    x = fmod(x, 2.0 * PI);

    // 2. The Running Term Trick
    long double term = x;  // The very first term (n=0) is always just x
    long double sinx = term;

    // Start at n=1, since we already added the n=0 term above
    for (int n = 1; n < 19; n++) 
    {
        // Multiply the previous term by (-x^2) / (2n * (2n+1))
        // This avoids calculating massive factorials!
        term *= -(x*x) / ((2.0*n) * (2.0*n + 1.0));
        
        // Add it to the total
        sinx += term;
    }
    
    return sinx;
}

// long double another_sin_2(double x)
// {
//     x = fmod(x, 2 * PI);
//     long double sinx = x;

//     for(int n = 1; n < 19; n++)
            // This logix is false
//         sinx += sinx * (-(x*x)) / ((2.0*n) * (2.0*n + 1.0));

//     return sinx;
// } 

int main()
{
    cout<<exponentialFunc(5, 3)<<endl;
    cout<<factorialFunc(6)<<endl;
    double x = 0;
    cout<<"Enter angle in radians: ";
    std::cin >> x;
    cout << fmod(x, 2.0 * PI) << endl;
    cout<<"sin(" << x << ") = " <<( x < 0 ? -my_sin(-x) : my_sin(x) )<<endl;
    cout<<"sin(" << x << ") = " <<( x < 0 ? -another_sin(-x) : another_sin(x) )<<endl;
    cout<<"sin(" << x << ") = " <<( x < 0 ? -sin(-x) : sin(x) )<<endl; // will call cmath sin function

    return 0;
}