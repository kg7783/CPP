
// Interface for class Calc

class Calc_Interface
{
public:
    virtual ~Calc_Interface() {}
    virtual int add(int a, int b) = 0;
    virtual int multiply(int a, int b) = 0;
    virtual double calc_sqrt(float value) = 0;
};

class Calc
{
public:
    static int add(int a, int b);
    static int multiply(int a, int b);
    static double calc_sqrt(float value);
};

class CalcDerived : public Calc_Interface
{
public:
    virtual int add(int a, int b);
    virtual int multiply(int a, int b);
    virtual double calc_sqrt(float value);
};


class Manage_calc
{
private:
    Calc_Interface* m_pCalc;

public:
    Manage_calc(Calc_Interface* pCalc);
    void Execute_calc();
};

