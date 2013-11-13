#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Number of Philosophers
#define NPHILO 5

// Number of Iterations for EAT and THINK
#define THINK_LOOP 100000000
#define EAT_LOOP 500000000

// macros for calculating the left/right stick/philospher
#define L_STICK(p_id) (p_id)
#define R_STICK(p_id) ((p_id+1)%NPHILO)
#define L_PHIL(p_id) ((p_id+NPHILO-1)%NPHILO)
#define R_PHIL(p_id) ((p_id+1)%NPHILO)

// Offset because of the numbers appearing later in the ASCII
#define ASCII_NUM_OFFSET 48

// true for while-loops
#define TRUE 1

// INPUTLIMIT
#define INPUT_LEN 5

// Phjilosoper States
#define THINK 0
#define HUNGRY 1
#define EAT 2

// Stick states
#define UNUSED 0
#define USED 1

// Thread commands
#define DEFAULT 'd'
#define BLOCK 'b'
#define PROCEED 'p'
#define QUIT 'q'
#define UNBLOCK 'u'

// methods of main
void init();
void inputLoop();
void eat(int p_id);
void think(int p_id);
void *philo(void *arg);
void disp_philo_states();
void get_sticks(int p_id);
void put_sticks(int p_id);
void block_philo(int p_id);
void handle_quit(char first_char);
void handle_command(char cmd_char, int p_id);

// TODO:
// DONE check against Styleguide
// * test against all fallacy cases
// * add the special compiler flag for the (int)i into the Makefile
// * remove p_thread philo_threads[] because of less/irregular useage
// * why is "sem_inits(....)" last argument zero in our code. but in fohls example it's 1
// * PROTOKOLL as PDF for Fohl!
