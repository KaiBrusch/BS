#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

//Anzahl der Philosophen
#define NPHILO 5
//Simulieren von THINK durch Zaehlschleife
#define THINK_LOOP 100000000
//Simulieren von EAT durch Zaehlschleife
#define EAT_LOOP 500000000
//Ermittelt die ID des Sticks links vom Philosophen
#define STICK_LINKS(philoID) philoID
//Ermittelt die ID des Sticks rechts vom Philosophen
#define STICK_RECHTS(philoID) ((philoID+1)%NPHILO)
//Ermittelt die ID des rechten Nachbarn des Philosophen
#define PHILO_RECHTS(philoID) ((philoID == NPHILO-1)? 0 : philoID+1)
//Ermittelt die ID des linken Nachbarn des Philosophen
#define PHILO_LINKS(philoID) ((philoID == 0)? NPHILO-1 : philoID-1)
//Fuer Zahlenvergleich(ASCII 48 entspricht 0)
#define ASCII_NUM_OFFSET 48
//Erlaubte Laenge der Eingabe
#define INPUT_LEN 5

//Datentyp fuer die moeglichen Zustaende eines Philosophen
#define THINK 0
#define HUNGRY 1
#define EAT 2

//Datentyp fuer die moeglichen Zustaende eines Sticks
#define UNUSED 0
#define USED 1

// Thread commands
#define DEFAULT 'd'
#define BLOCK 'b'
#define PROCEED 'p'
#define QUIT 'q'
#define UNBLOCK 'u'

//Datentyp Boolean
typedef enum {
    FALSE = 0,
    TRUE = 1
} Boolean;

//Speichert den Zustand der Philosophen
int philo_state[NPHILO];
//Speichert den Zustand der Sticks
int stick_state[NPHILO];

//Mutex
pthread_mutex_t mutex;
//Eine Condition Variable je Philosoph
pthread_cond_t cond[NPHILO];
//Ein Semaphor je Philosoph
sem_t semaphoren[NPHILO];
//Befehle fuer Philosophen werden hier abgespeichert.
//"q"=quit, "b"=block, "u"=unblock, "p"=proceed, "-"=kein Befehl
char input_commands[NPHILO];

void *philo(void *arg);
void think(int philoID);
void eat(int philoID);
void get_sticks(int philoID);
void put_sticks(int philoID);
void checkBlock(int philoID);
void disp_philo_states();
void init();
void inputLoop();