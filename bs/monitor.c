#include "main.h"

// variables from main
extern int philo_state[NPHILO];
extern int stick_state[NPHILO];
extern pthread_mutex_t mutex;
extern pthread_cond_t cond[NPHILO];

// getsticks is called from each of the threads
void get_sticks(int p_id) {
    
    // cirtical section begins
    pthread_mutex_lock(&mutex);
    
    // philsopher is hungry until the thread reaches the eat(...) method in main
    philo_state[p_id] = HUNGRY;
    disp_philo_states();
    
    // wait until both are free
    // the while ensures that both are indeed true once the thread exits the loop
    while(stick_state[L_STICK(p_id)] == USED
            || stick_state[R_STICK(p_id)] == USED) {
        pthread_cond_wait(&cond[p_id], &mutex);
    }
    
    // now it's this philosophers turn to eat
    stick_state[L_STICK(p_id)] = USED;
    stick_state[R_STICK(p_id)] = USED;
    
    philo_state[p_id] = EAT;
    disp_philo_states();

    // cirtical section ends
    pthread_mutex_unlock(&mutex);
}

void put_sticks(int p_id) {

    // cirtical section begins
    pthread_mutex_lock(&mutex);
    
    // time to think......
    philo_state[p_id] = THINK;
    disp_philo_states();
    
    // puts back the sticks
    stick_state[L_STICK(p_id)] = UNUSED;
    stick_state[R_STICK(p_id)] = UNUSED;

    // Finished eating. Signal the neighbors
    pthread_cond_signal(&cond[L_PHIL(p_id)]);
    pthread_cond_signal(&cond[R_PHIL(p_id)]);

    // cirtical section ends
    pthread_mutex_unlock(&mutex);
}

// Display all states and check for inconsistency
void disp_philo_states() {

    // Check for inconsistencies
    for(int i = 0; i < NPHILO; i++) {
        if(philo_state[i] == EAT) {
            if((philo_state[L_PHIL(i)] == EAT
                || philo_state[R_PHIL(i)] == EAT)){
                printf("Two adjacent philosophers must NOT eat simultaneously!\n");
                break;
            }
        }
    }
    
    // out each philosopher
    for(int i = 0; i < NPHILO; i++) {
         switch (philo_state[i]) {
            case EAT:
                printf("%d%c ", i, 'E');
                break;
                
            case THINK:
                printf("%d%c ", i, 'T');
                break;
                
            case HUNGRY:
                printf("%d%c ", i, 'H');
                break;
                
            default:
                printf("%d%c ", i, '?');    // catch error state
                break;
        }
    }
    // formatting
    printf("\n");
}
