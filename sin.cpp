#include <iostream>
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

long double sin(long double x)
{
    long double sinx = 0;
    // Summation Function
    for (int n = 0; n < 5; n++)
      sinx += ( exponentialFunc(-1, n) * exponentialFunc(x, ((2*n) + 1)) ) / factorialFunc((2*n) + 1);
    
    return sinx;
}

int main()
{
    cout<<exponentialFunc(5, 3)<<endl;
    cout<<factorialFunc(6)<<endl;
    double x = 0;
    cout<<"Enter angle in radians: ";
    std::cin >> x;

    cout<<( x < 0 ? -sin(-x) : sin(x) )<<endl;

    return 0;
}