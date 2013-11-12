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
#define ASCII_ZERO 48
//Erlaubte Laenge der Eingabe
#define EINGABELAENGE 3

//Datentyp fuer die moeglichen Zustaende eines Philosophen
typedef enum {
    THINK = 0,
    HUNGRY = 1,
    EAT = 2
} PhiloZustand;

//Datentyp fuer die moeglichen Zustaende eines Sticks
typedef enum {
    FREE = 0,
    IN_USE = 1
} StickZustand;

//Datentyp Boolean
typedef enum {
    FALSE = 0,
    TRUE = 1
} Boolean;

//Speichert den Zustand der Philosophen
PhiloZustand philoZustaende[NPHILO];
//Speichert den Zustand der Sticks
StickZustand stickZustaende[NPHILO];

//Mutex
pthread_mutex_t mutex;
//Eine Condition Variable je Philosoph
pthread_cond_t cond[NPHILO];
//Ein Semaphor je Philosoph
sem_t semaphoren[NPHILO];
//IDs der Philosophen
int philoIDs[NPHILO];
//Speicher von Tastatureingaben
char eingabe[EINGABELAENGE];
//Befehle fuer Philosophen werden hier abgespeichert.
//"q"=quit, "b"=block, "u"=unblock, "p"=proceed, "-"=kein Befehl
char steuerarray[NPHILO];

void *philo(void *arg);
void think(int philoID);
void eat(int philoID);
void get_sticks(int philoID);
void put_sticks(int philoID);
void checkBlock(int philoID);
void disp_philo_states();