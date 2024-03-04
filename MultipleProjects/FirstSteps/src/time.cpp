#include <cassert>
#include <cstring>
#include <ctime>
#include <iostream>

void manage_time(void)
{
    std::time_t result = std::time(nullptr);

    std::cout << std::ctime(&result);
 
    char buffer[32];
    std::strncpy(buffer, std::ctime(&result), 26);
    assert('\n' == buffer[std::strlen(buffer) - 1]);
    std::cout << buffer;

}