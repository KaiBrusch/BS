/** @file main.h
 *  @brief This file contains all constants and method signatures
 *
 * @author Kai Brusch
 * @author Matthias Nitsche
 * @author Swaneet Kumar
 * @author Ivan Morozov
 */

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

// only 5 names are given at the beginning.
// this #define is needed to set the default names for the remaining philosophers
#define GIVEN_NAMES 5

// methods of main

/** @brief Initialize all philosophers states and create the pthreads.
 * @return void
 */
void init();

/** @brief a continuous loop that listens on user input.
 *  @return void
 */
void inputLoop();

/** @brief a for loop that runs EAT_LOOP times to simulate a waiting time.
    The Function is also used to block the philosophers for this type of action.
 *  @param p_id philosopher id
 *  @return void
 */

void eat(int p_id);

/** @brief a for loop that runs THINK_LOOP times to simulate a waiting time.
    The Function is also used to block the philosophers for this type of action.
 *  @param p_id philosopher id
 *  @return void
 */
void think(int p_id);

/** @brief is the philosopher thread where a philosopher calls "think",
 *  "get_sticks", "eat" and "put_sticks". A philosopher thread runs until
 *  it is destroyed by the user through an input in the inputloop starting
 *  with "q|Q".
 *  @param *arg a pointer to a arbitrary object (in this case philo id).
 *  @return void
 */
void *philo(void *arg);

/** @brief prints out the current state of each philosopher in a row based
 *  view within the shell/IDE. The view works like this:
 *  p_id_0 + p_state_0 + " " .. p_id_NPHILO + p_state_NPHILO + " "
 *  @return void
 */
void disp_philo_states();

/** @brief gets the left and right sticks next to this particular philosopher (p_id).
 *  To do so the pthread_mutex is locked at the beginning and then unlocked at the end.
 *  This is to make sure that the sticks won't be used by other philosophers. The state
 *  of the Philosopher is then changed. The Philosopher then waits for his left and right stick
 *  to be freed so the philosopher can be set to the State EAT.
 *  @param p_id philosopher id
 *  @return void
 */
void get_sticks(int p_id);

/** @brief put_sticks is called after get_sticks to set the left and right stick of
 *  the current Philosoph to UNUSED. This is to ensure that the sticks can be used
 *  by other philosophers. The Philosopher is set to the state THINK again.
 *  @param p_id philosopher id
 *  @return void
 */
void put_sticks(int p_id);

/** @brief block_philo takes the p_id of the Philosopher and asks if its state is BLOCKED. If so
 *  set the state to default and call sem_wait to block him else do nothing.
 *  @param p_id philosopher id
 *  @return void
 */
void block_philo(int p_id);

/** @brief handle_quit take the first character of the input and checks if it
 *  is a q or Q. If so release all Philosophers from their semaphore blocks which
 *  were set through sem_wait. After that join all philosopher so that each philo
 *  waits for the others. If they were joined the semaphores are destroyed and the
 *  pthread_mutex are destroyed. The philosophers within their function get a callback
 *  thorugh the input_command by the user and calls pthread_exit to kill this thread.
 *  @param first_char is the first input character of the input by the user.
 *  @return void
 */
void handle_quit(char first_char);

/** @brief the handle_command handles the userinput. the cmd_char is at input[1]
 *  and is a char that must be equal to 'b' for BLOCK, 'u' for UNBLOCK, 'p' for
 *  PROCEED, else a default is called. The second argument is the philosopher id
 *  that is taken from input[0] and has to be between 0 and NPHILO. At maximum 9
 *  Philosophers are allowed. In any case the input_commands array is set to a
 *  specific state for the Philosopher (p_id).
 *  @param cmd_char second character of input by user.
 *  @param p_id philosopher id
 *  @return void
 */
void handle_command(char cmd_char, int p_id);
