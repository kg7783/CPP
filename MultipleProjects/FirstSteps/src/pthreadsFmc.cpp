// library headers here
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>



// forward declaration
static void * threadFuncRx(void *parg);


void manage_pthreadsFMC(void) 
{
    // data items for threads
    pthread_t rxThread;
    pthread_attr_t attrtRxThread;

    // creates the thread needed by the Certificate Manager
    if(pthread_attr_init(&attrtRxThread) != 0)
    {
        return;
    };

    if(pthread_attr_setdetachstate(&attrtRxThread, PTHREAD_CREATE_DETACHED) != 0)
    {
        return;
    }

    if (pthread_create(&rxThread, &attrtRxThread, &threadFuncRx, NULL) != 0)
    {
        return;
    }

    while (1)
    {
        sleep(1);   // thread is startet, do nothing but sleep until machine is switched off (!!??)

        static int counter = 0;
        printf("main: %d | ", counter);
        counter++;

        if(counter > 5)
        {
            break;
        }
    }

    if (pthread_attr_destroy(&attrtRxThread))
    {
        return;
    }

    pthread_cancel(rxThread);

    while(1)
    {
        sleep(1);

        static int counter = 0;
        printf("main end: %d | ", counter);
        counter++;
    }
    
    return;     
}

static void * threadFuncRx(void *parg)
{
    // need to wait until the 5008 is done setting the serial port
    sleep(0);

    parg = static_cast <void *> (NULL);
    // stay in this loop and read the "MACHINE_STATUS__FILE_NAME" every second (!!??) until service detected
    while (1)
    {
        sleep(1);

        static int counter = 0;
        printf("thread: %d\n", counter);
        counter++;
   }

    return parg;
}