#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Number of Philosophers
#define NPHILO 5

// Iterations for EAT and THINK
#define THINK_LOOP 100000000
#define EAT_LOOP 500000000

// macro for calculating the
#define STICK_LINKS(p_id) p_id
//Ermittelt die ID des Sticks rechts vom Philosophen
#define STICK_RECHTS(p_id) ((p_id+1)%NPHILO)
//Ermittelt die ID des rechten Nachbarn des Philosophen
#define PHILO_RECHTS(p_id) ((p_id == NPHILO-1)? 0 : p_id+1)
//Ermittelt die ID des linken Nachbarn des Philosophen
#define PHILO_LINKS(p_id) ((p_id == 0)? NPHILO-1 : p_id-1)
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

void *philo(void *arg);
void think(int p_id);
void eat(int p_id);
void get_sticks(int p_id);
void put_sticks(int p_id);
void block_philo(int p_id);
void disp_philo_states();
void init();
void inputLoop();
void handle_quit(char first_char);
void handle_command(char cmd_char, int p_id);

// TODO:
// * check against Styleguide
// * test against all fallacy cases
