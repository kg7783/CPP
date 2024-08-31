#include "calc.h"
#include <math.h>
#include <iostream>

void manage_calc()
{
    CalcDerived calc_;
    Manage_calc manage_Calc(&calc_);

    manage_Calc.Execute_calc();
}


int Calc::add(int a, int b)
{
    return a+b;
}

int Calc::multiply(int a, int b)
{
    return a*b;
}

double Calc::calc_sqrt(float value)
{
    return sqrt(value);
}




int CalcDerived::add(int a, int b)
{
    return a+b;
}

int CalcDerived::multiply(int a, int b)
{
    return a*b;
}

double CalcDerived::calc_sqrt(float value)
{
    return sqrt(value);
}


Manage_calc::Manage_calc(Calc_Interface* pCalc)
{
    m_pCalc = pCalc;
}

void Manage_calc::Execute_calc()
{
    m_pCalc->add(3,5);
    m_pCalc->multiply(3,5);
    m_pCalc->calc_sqrt(100);

    std::cout << "Summe: " << m_pCalc->add(3,5) << std::endl;
    std::cout << "Multiplizieren: " << m_pCalc->multiply(3,5) << std::endl;
    std::cout << "Sqrt: " << m_pCalc->calc_sqrt(100) << std::endl;
}


