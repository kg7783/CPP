#include <iostream>
#include <pthread.h>

#include <chrono>
#include <thread>

using namespace std;

#define SLEEP_VALUE 200


struct argtype 
{
    int rank; 
    int a,b; 
    double c;
    pthread_mutex_t mutex;
};

void thread_print_args(string ident, argtype *args)
{
    pthread_mutex_lock(&args->mutex);
        cout << ident << ": " << args->rank << " " << args->a << " " << args->b << " " << args->c << endl;
    pthread_mutex_unlock(&args->mutex);
}


void thread_print_value(string ident, int value, int mod_value, argtype *args)
{
    if(value % mod_value == 0)
    {
        pthread_mutex_lock(&args->mutex);
            cout << ident << ": " << value << endl;
        pthread_mutex_unlock(&args->mutex);

        this_thread::sleep_for(chrono::milliseconds(SLEEP_VALUE));
    }
}

void* prod (argtype *args) 
{ /* Producer thread */

    thread_print_args("prod", args);

    int count=0;
    while (count<100000) 
    {
        count++;

        thread_print_value("prod", count, 10000, args);
    }

    return (void*)nullptr;
}

void* con (argtype *args) 
{ /* Consumer thread */

    thread_print_args("con", args);

    int count=0;
    while (count<1000000) 
    {
        count++;

        thread_print_value("con", count, 50000, args);
    }

    return (void*)nullptr;
}

void manage_threads(void)
{
    argtype args;
    args.rank = 1;
    args.a = 2;
    args.b = 3;
    args.c = 3.14;

    pthread_t thread_p, thread_c;
    pthread_attr_t attr_p, attr_c;

    pthread_mutex_init(&args.mutex,nullptr);

    pthread_attr_init(&attr_p);
    pthread_attr_init(&attr_c);

    pthread_attr_setdetachstate(&attr_p, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setdetachstate(&attr_c, PTHREAD_CREATE_JOINABLE);

    pthread_create(&thread_p, &attr_p, (void*(*)(void*)) prod, (void *) &args);
    pthread_create(&thread_c, &attr_c, (void*(*)(void*)) con, (void *) &args);

    pthread_join(thread_p, nullptr); 
    cout << "prod terminated" << endl;

//    pthread_cancel(thread_c);

    pthread_join(thread_c, nullptr);
    cout << "con terminated" << endl;

    pthread_mutex_destroy(&args.mutex);

    pthread_attr_destroy(&attr_p);
    pthread_attr_destroy(&attr_c);

}