#include <iostream>
#include <boost/locale.hpp>
#include <boost/locale/date_time.hpp>


using namespace boost::locale;
using namespace std;


struct EnsureLocaleBase {
    EnsureLocaleBase() { Init(); }

  private:
    static bool Init() 
    {
        static auto const s_init = [] 
        {
            boost::locale::generator gen;
            std::locale l = gen("");
            std::locale::global(l);
            return true;
        }();
        return s_init;
    }
};

class DateTimeWrapper : EnsureLocaleBase {
    boost::locale::date_time m_date_time;
};


void manage_boost_locale(void)
{
    DateTimeWrapper local_date_time_object;

    
    using namespace boost::locale;
    static date_time now; // Create date_time class with default calendar initialized to current time
    date_time tomorrow = now + period::day();
    cout << "Let's meet tomorrow at " << as::date << tomorrow << endl;
    date_time some_point = period::year(1995) + period::january() + period::day(1);
    // Set some_point's date to 1995-Jan-1.
    cout << "The "<< as::date << some_point << " is the " 
        << as::ordinal << some_point / period::day_of_week_local() << " day of the week"  << endl;
}