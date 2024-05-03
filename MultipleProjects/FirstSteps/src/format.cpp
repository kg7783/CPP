#include <format>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <chrono>
#include <vector>


template<typename... Args>
std::string dyna_print(std::string_view rt_fmt_str, Args&&... args)
{
    return std::vformat(rt_fmt_str, std::make_format_args(args...));
}
 
void manage_format(void)
{
    // Save pi as a string with three decimals of precision:
    const std::string pi = std::format("{:.3f}", 3.141592654);
    std::cout << pi << std::endl << std::endl;


    auto ym = std::chrono::year { 2022 } / std::chrono::July;
    std::string msg = std::format("{:*^10}\n{:*>10}\nin {}!", "hello", "world", ym);
    std::cout << msg << std::endl << std::endl;

    std::string fmt;
    for (int i{}; i != 3; ++i)
    {
        fmt += "{} "; // constructs the formatting string
        std::cout << fmt << " : ";
        std::cout << dyna_print(fmt, "alpha", 'Z', 3.14, "unused");
        std::cout << '\n';
    }


}

