#include <iostream>
#include <complex>
#include <concepts>
using namespace std::complex_literals;

// while waiting for P2078 to add the is_complex type trait,
// we implement a makeshift version...
template<typename T>
concept Complex = requires(T a, T b) 
{
        { a.imag() };
        { a.real() };
        { a + b } -> std::convertible_to<T>;
};

template<typename T>
concept Continuous = Complex<T> || std::floating_point<T>;

std::integral auto factorial(std::integral auto a)
{
    if (a <= 0) return 1;
    else return a * factorial(a - 1);
}

std::floating_point auto factorial(std::floating_point auto a)
{
    decltype(a) one = 1.0;
    return std::tgamma(a + one);
}

Complex auto complexGamma(Complex auto a)
{
    auto z = 0.0 + 0i;
    // A whole mess of complex math here...
    // (for one example, see https://www.johndcook.com/blog/cpp_gamma/)
    return z;
}

Complex auto factorial(Complex auto a)
{
    decltype(a) one = 1.0;
    return complexGamma(a + one);
}

void manage_test(void)
{
    using namespace std::complex_literals;
    std::cout << factorial(10) << std::endl;
    std::cout << factorial(-10.5) << std::endl;
    std::cout << factorial(10.0 + 2i) << std::endl;
}