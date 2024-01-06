#include <iostream>
#include <vector>
#include <string>

#include "calc.h"

using namespace std;

int main()
{
    vector<string> msg {"Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

    for (const string& word : msg)
    {
        cout << word << " ";
    }
    cout << endl;

    int zahl1, zahl2 = 0;

    cout << "Zahl1: ";
    cin >> zahl1;

    cout << "Zahl2: ";
    cin >> zahl2;

    cout << "Ergebnis: " << Calc::add(zahl1, zahl2) << endl;
}