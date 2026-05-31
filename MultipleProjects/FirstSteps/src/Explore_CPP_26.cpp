#include <iostream>
#include <print>
#include <expected>
#include <flat_map>
#include <inplace_vector>
#include <generator>
#include <mdspan>
#include <string>
#include <vector>
#include <span>
#include <ranges>
#include <array>
#include <typeinfo>

// C++26: Contracts – [[pre: p > 0]], [[post: __return >= 0]]
double safe_sqrt(double p)
{
    if (p <= 0) return 0;
    double r = __builtin_sqrt(p);
    if (r <= 0 && p > 0) return 0;
    return r;
}

// C++26: Reflection via ^T und [: ... :] (P2996, angenommen für C++26) – hier: typeid als Platzhalter
template <typename T>
void reflect_type()
{
    std::println("Typ: {} | Groesse: {} Bytes", typeid(T).name(), sizeof(T));
}

// C++26: std::inplace_vector<T,N> – fixe Kapazität, kein Allokator, contiguous Storage
void demo_inplace_vector() 
{
    std::inplace_vector<int, 5> vec;

    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    std::println("inplace_vector: ");
    for (int v : vec) std::print("{} ", v);
    std::println(" | Kapazitaet: {}", vec.capacity());
}

template <typename T, size_t N>
struct SimdArray
{
    std::array<T, N> data{};

    SimdArray() = default;
    SimdArray(std::initializer_list<T> il)
    {
        size_t i = 0;
        for (auto v : il) if (i < N) data[i++] = v;
    }

    SimdArray operator+(const SimdArray& o) const
    {
        SimdArray r;
        for (size_t i = 0; i < N; ++i) r.data[i] = data[i] + o.data[i];
        return r;
    }

    SimdArray operator*(const SimdArray& o) const
    {
        SimdArray r;
        for (size_t i = 0; i < N; ++i) r.data[i] = data[i] * o.data[i];
        return r;
    }

    friend void print_simd(const std::string& label, const SimdArray& v)
    {
        std::print("{} [", label);
        for (size_t i = 0; i < N; ++i)
        {
            std::print("{}{}", v.data[i], i < N - 1 ? ", " : "");
        }
        std::print("]");
    }
};

// Eigenbau-SIMD-Ersatz: std::simd<T, N> war für C++26 geplant, aber nicht aufgenommen – elementweise Vektorarithmetik als Platzhalter
void demo_simd()
{
    SimdArray<float, 4> a{1.0f, 2.0f, 3.0f, 4.0f};
    SimdArray<float, 4> b{5.0f, 6.0f, 7.0f, 8.0f};

    auto c = a + b;
    auto d = a * b;

    print_simd("a", a); std::print(" + "); print_simd("b", b);
    std::print(" = "); print_simd("c", c); std::println("");

    print_simd("a", a); std::print(" * "); print_simd("b", b);
    std::print(" = "); print_simd("d", d); std::println("");
}

// C++23: std::expected – Fehlerbehandlung ohne Exceptions
std::expected<int, std::string> divide(int a, int b) 
{
    if (b == 0) 
    {
        return std::unexpected("Division durch Null!");
    }
    return a / b;
}

void demo_expected() 
{
    auto result = divide(10, 2);
    if (result) 
    {
        std::println("Ergebnis: {}", *result);
    }

    auto err = divide(5, 0);
    if (!err) 
    {
        std::println("Fehler: {}", err.error());
    }
}

// C++23: std::flat_map – sortierter Key-Value-Contiguous-Container
void demo_flat_map() 
{
    std::flat_map<std::string, int> alters_map = 
    {
        {"Anna", 25}, {"Bob", 30}, {"Clara", 22}
    };

    alters_map.insert({"David", 28});

    std::println("flat_map Eintraege:");
    for (const auto& [name, alter] : alters_map) 
    {
        std::println("  {} ist {} Jahre alt", name, alter);
    }
}

// C++23: std::generator – Coroutine-basierte Lazy Sequences
std::generator<int> fibonacci(int n) 
{
    int a = 0, b = 1;
    for (int i = 0; i < n; ++i) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

void demo_generator() 
{
    std::println("Fibonacci (10):");
    for (int f : fibonacci(10)) 
    {
        std::print("{} ", f);
    }
    std::println("");
}

// C++23: std::mdspan – Multi-dimensional Array View
void demo_mdspan() 
{
    std::vector data = {1, 2, 3, 4, 5, 6};
    std::mdspan matrix{data.data(), 2, 3};

    std::println("2x3 Matrix (mdspan):");
    for (int i = 0; i < matrix.extent(0); ++i) 
    {
        for (int j = 0; j < matrix.extent(1); ++j) 
        {
            std::print("{} ", matrix[i, j]);
        }
        std::println("");
    }
}

// C++20: std::span – C++26 verbessert Bounds-Checking (Library Hardening via P2759)
void demo_span() 
{
    std::vector<int> vec = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::span s{vec};

    auto first_three = s.first(3);
    auto last_two    = s.last(2);
    auto mid         = s.subspan(3, 4);

    std::println("Span first(3):");
    for (int v : first_three) std::print("{} ", v);
    std::println("");

    std::println("Span last(2):");
    for (int v : last_two) std::print("{} ", v);
    std::println("");

    std::println("Span subspan(3, 4):");
    for (int v : mid) std::print("{} ", v);
    std::println("");
}

// C++26: Pack-Indexing – Ts...[N] direkter Zugriff auf Parameter-Pack-Elemente
template <typename... Ts>
void demo_pack_indexing()
{
    using FirstType = Ts...[0];
    using LastType  = Ts...[sizeof...(Ts) - 1];

    static_assert(std::same_as<FirstType, int>);
    static_assert(std::same_as<LastType, std::string>);

    std::println("Pack-Indexing: Ts...[0] = int, Ts...[2] = string (compile-time checked)");
}

struct Person 
{
    std::string name;
    int alter;
    double groesse;
};

void manage_explore_cpp_26() 
{
    std::println("========== C++26 Features Demo ==========");
    std::println("Compiler: {}.{}", __GNUC__, __GNUC_MINOR__);
    std::println("C++ Standard: {}\n", __cplusplus);

    // C++26: Reflection via ^T und [: ... :] (P2996) – hier: typeid als Platzhalter
    std::println("--- Reflection (RTTI-Demo) ---");
    reflect_type<Person>();
    std::println("");

    // C++26: Contracts via [[pre:]], [[post:]] (hier: manuelle Prüfung als Platzhalter)
    std::println("--- Contracts ---");
    double q = safe_sqrt(9.0);
    std::println("sqrt(9.0) = {}\n", q);

    // C++26: std::inplace_vector<T,N> – fixe Kapazität, kein Allokator
    std::println("--- inplace_vector ---");
    demo_inplace_vector();
    std::println("");

    // Eigenbau-Ersatz: std::simd war geplant, aber nicht in C++26 aufgenommen
    std::println("--- SIMD ---");
    demo_simd();
    std::println("");

    // C++23: std::expected – Fehlerbehandlung ohne Exceptions
    std::println("--- std::expected ---");
    demo_expected();
    std::println("");

    // C++23: std::flat_map – sortierter Key-Value-Contiguous-Container
    std::println("--- flat_map ---");
    demo_flat_map();
    std::println("");

    // C++23: std::generator – Coroutine-basierte Lazy Sequences
    std::println("--- generator ---");
    demo_generator();
    std::println("");

    // C++23: std::mdspan – Multi-dimensional Array View
    std::println("--- mdspan ---");
    demo_mdspan();
    std::println("");

    // C++20: std::span (C++26: Library Hardening via P2759)
    std::println("--- span ---");
    demo_span();
    std::println("");

    // C++26: Pack Indexing – Ts...[N] direkter Zugriff auf Parameter-Pack-Elemente
    std::println("--- Pack Indexing ---");
    demo_pack_indexing<int, double, std::string>();
    std::println("");

    // C++26 (vorgeschlagen, nicht angenommen): Pattern Matching via inspect – noch kein Compiler-Support
    std::println("--- Pattern Matching (C++26, nicht angenommen) ---");
    std::println("  Syntax: inspect(obj) {{ <int x> => f(x); <string s> => g(s); }}\n");

    // C++26 (vorgeschlagen, nicht angenommen): std::execution / Senders+Receivers (P2300)
    std::println("--- Senders/Receivers (C++26, nicht angenommen) ---");
    std::println("  std::execution::sender auto s = just(42) | then(f) | upon_error(g);\n");

    // C++26: std::linalg – Linear-Algebra-Bibliothek (P1673, angenommen)
    std::println("--- Linear Algebra (C++26, std::linalg) ---");
    std::println("  std::linalg::matrix_product(A, B, C);\n");

    std::println("========== Ende ==========");
}
