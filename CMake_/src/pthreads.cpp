/* thread1.c */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/* 100000 Elemente */
#define MAX 10000

/* ein Array von großen zu kleinen Werten */
int test_array1[MAX];
int test_array2[MAX];

/* in umgekehrter Reihenfolge erstellen */
void init_test_array(int *array) 
{
   int i, j;
   for(i = MAX,j=0; i >= 0; i--,j++)
      array[j] = i;
}

// Thread 1
static void *bubble1(void* val) 
{
   static int i, temp, elemente=MAX;
   printf("Thread bubble1() wurde gestartet\n");
   while(elemente--)
   {
      printf("bubble1: %d\n", elemente);
      for(i = 1; i <= elemente; i++)
      {
         if(test_array1[i-1] > test_array1[i]) 
         {
            temp=test_array1[i];
            test_array1[i]=test_array1[i-1];
            test_array1[i-1]=temp;
         }
      }
    }
   printf("Thread bubble1() wurde beendet\n");
   // Der Rückgabewert interessiert uns nicht.
   return NULL;
}

// Thread 2
static void *bubble2(void* val) 
{
   static int i, temp, elemente=MAX;
   printf("Thread bubble2() wurde gestartet\n");
   while(elemente--)
   {
      printf("bubble2: %d\n", elemente);
      for(i = 1; i <= elemente; i++)
      {
         if(test_array2[i-1] > test_array2[i]) 
         {
            temp=test_array2[i];
            test_array2[i]=test_array2[i-1];
            test_array2[i-1]=temp;
         }
      }
    }
   printf("Thread bubble2() wurde beendet\n");
   // Der Rückgabewert interessiert uns nicht.
   return NULL;
}


int manage_pthreads(void) 
{
    pthread_t thread1, thread2;
    int i, rc;

    // Ausgabe in eine Textdatei
    freopen("myoutput.txt", "w+", stdout);

    printf("Haupt-Thread main() wurde gestartet\n");
    // beide Arrays mit Werten initialisieren
    init_test_array(test_array1);
    init_test_array(test_array2);
    // Thread 1 erzeugen
    rc = pthread_create( &thread1, NULL, &bubble1, NULL );
    if( rc != 0 ) 
    {
        printf("Konnte Thread 1 nicht erzeugen\n");
        return EXIT_FAILURE;
    }
    // Thread 2 erzeugen
    rc = pthread_create( &thread2, NULL, &bubble2, NULL );
    if( rc != 0 ) 
    {
        printf("Konnte Thread 2 nicht erzeugen\n");
        return EXIT_FAILURE;
    }
    // Main-Thread wartet auf beide Threads.
    pthread_join( thread1, NULL );
    pthread_join( thread2, NULL );

    // das Ergebnis der Sortierung in die Datei
    // myoutput.txt schreiben
    for(i = 0; i < MAX; i++) 
    {
       printf("[%d-%d]\n", test_array1[i], test_array2[i]);
    }
    printf("\nHaupt-Thread main() wurde beendet\n");
    return EXIT_SUCCESS;
}