#include <boost/chrono.hpp>
#include <cmath>
#include <iostream>

using namespace boost::chrono;

void manage_boost_chrono()
{
    boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();

    for ( long i = 0; i < 10000000; ++i )
    {
        std::sqrt( 123.456L ); // burn some time
    }

    boost::chrono::duration<double> sec = boost::chrono::system_clock::now() - start;
    std::cout << "took " << sec.count() << " seconds\n";


    std::cout << system_clock::now() << '\n';
    #ifdef BOOST_CHRONO_HAS_CLOCK_STEADY
        std::cout << steady_clock::now() << '\n';
    #endif
    std::cout << high_resolution_clock::now() << '\n';

    #ifdef BOOST_CHRONO_HAS_PROCESS_CLOCKS
        std::cout << process_real_cpu_clock::now() << '\n';
        std::cout << process_user_cpu_clock::now() << '\n';
        std::cout << process_system_cpu_clock::now() << '\n';
        std::cout << process_cpu_clock::now() << '\n';
    #endif

    #ifdef BOOST_CHRONO_HAS_THREAD_CLOCK
        std::cout << thread_clock::now() << '\n';
    #endif
}