#include <box2d/box2d.h>

#include <fmt/core.h> 



void manage_vcpkg(void)
{
    fmt::print("The answer is {}.\n", 42);

    b2Vec2 gravity{0.f, 9.8f};
    b2World newWorld{gravity};
}