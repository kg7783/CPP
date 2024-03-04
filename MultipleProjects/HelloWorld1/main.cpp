#include <iostream>

extern int add(int a, int b);

int main(void)
{
    std::cout << "Hello World 1: " << add(3,5) << std::endl;
    return 0;
}