#include <any>
#include <string>
#include <iostream>
#include <map>
#include <cassert>
#include <vector>

class MyType
{
    int a, b;
    
public:
    MyType(int x, int y) : a(x), b(y) { }
};
 
void manage_any(void)
{
    std::any a(12);

    // set any value:
    a = std::string("Hello!");
    a = 16;
    // reading a value:

    // we can read it as int
    std::cout << std::any_cast<int>(a) << '\n'; 

    // but not as string:
    try 
    {
        std::cout << std::any_cast<std::string>(a) << '\n';
    }
    catch(const std::bad_any_cast& e) 
    {
        std::cout << e.what() << '\n';
    }

    // reset and check if it contains any value:
    a.reset();
    if (!a.has_value())
    {
        std::cout << "a is empty!" << "\n";
    }

    // you can use it in a container:
    std::map<std::string, std::any> m;

    m["integer"] = 10;
    m["string"] = std::string("Hello World");
    m["float"] = 1.0f;

    m.insert(std::pair<std::string, std::any>("int", 20));
    m.insert(std::pair<std::string, std::any>("str", std::string("Hello String")));
    m.insert(std::pair<std::string, std::any>("fl", 3.14f));

    for (auto &[key, val] : m)
    {
        if (val.type() == typeid(int))
            std::cout << key << ": " << std::any_cast<int>(val) << "\n";
        else if (val.type() == typeid(std::string))
            std::cout << key << ": " << std::any_cast<std::string>(val) << "\n";
        else if (val.type() == typeid(float))
            std::cout << key << ": "  << std::any_cast<float>(val) << "\n";
    }    

       // default initialization:
    std::any a_;
    std::cout << "a_.has_value(): " << a_.has_value() << '\n';
    assert(!a_.has_value());
    
    a_ = MyType(10, 11);
    a_ = std::string("Hello");
    
    a_.emplace<float>(100.5f);
    a_.emplace<std::vector<int>>({10, 11, 12, 13});
    a_.emplace<MyType>(10, 11);

    // initialization with an object:
    std::any a2(10); // int
    std::any a3(MyType(10, 11));
    
    // in_place:
    std::any a4(std::in_place_type<MyType>, 10, 11);
    std::any a5{std::in_place_type<std::string>, "Hello World"};
    
    // make_any
    std::any a6 = std::make_any<std::string>("Hello World");
}



