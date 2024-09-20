#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "../src/calc.h"

#include <math.h>



using ::testing::Return;


class MockCalc : public Calc_Interface
{
public:
    MOCK_METHOD(int, add, (int, int), (override));
    MOCK_METHOD(int, multiply, (int, int), (override));
    MOCK_METHOD(double, calc_sqrt, (float), (override));
};

struct ExampleTests : public::testing::Test
{
    int* x = nullptr;

    int a = 0, b = 0;

     // Sets up the test fixture.
    virtual void SetUp() override
    {
        x = new int(42);
    }

    // Tears down the test fixture.
    virtual void TearDown() override
    {
        delete x;
        x = nullptr;
    }

};

TEST_F(ExampleTests, DemonstrateGTestMacros)
{
    EXPECT_EQ(true, true);
    EXPECT_EQ(false, false);
}

TEST_F(ExampleTests, TestPointer)
{
    EXPECT_NE(x, nullptr);
    EXPECT_EQ(*x, 42);
}

TEST_F(ExampleTests, TestSum)
{
    Calc calc;

    while(a<100 && b<100)
    {
        EXPECT_EQ(calc.add(a,b), a+b);

        a++;
        b++;
    }
}

TEST_F(ExampleTests, TestProduct)
{
    int a=0,b=0;
    Calc calc_;

    while(a<100 && b<100)
    {
        EXPECT_EQ(calc_.multiply(a,b), a*b);

        a++;
        b++;
    }
}

TEST_F(ExampleTests, TestWithMock)
{
    MockCalc mock_calc;
    Manage_calc manage_Calc(&mock_calc);

//    EXPECT_CALL(mock_calc, add(3,5))
//        .Times(0);

    EXPECT_CALL(mock_calc, multiply)
        .Times(1)
        .WillRepeatedly(Return(15));

    EXPECT_CALL(mock_calc, calc_sqrt)
        .Times(1)
        .WillRepeatedly(Return(10));

    manage_Calc.Execute_calc();
 
}