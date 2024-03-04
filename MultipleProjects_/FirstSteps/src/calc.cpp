#include "calc.h"
#include <math.h>
#include <iostream>

void manage_calc()
{
    std::cout << "Summe: " << Calc::add(3,5) << std::endl;
    std::cout << "Sqrt: " << Calc::calc_sqrt(100) << std::endl;
}
int Calc::add(int a, int b)
{
    return a+b;
}

double Calc::calc_sqrt(float value)
{
    return sqrt(value);
}