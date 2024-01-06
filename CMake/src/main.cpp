#include <iostream>
#include "calc.h"

using namespace std;

int main(int, char**)
{
    cout << "Hello, from CMakeStarterCpp!\n";

    Calc calc;

    cout << Calc::add(3, 5) << endl;


    return 0;
}
