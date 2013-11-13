#include "main.h"

extern int philo_state[NPHILO];
extern int stick_state[NPHILO];
extern pthread_mutex_t mutex;
extern pthread_cond_t cond[NPHILO];

void get_sticks(int philoID)
{
    pthread_mutex_lock(&mutex);

    philo_state[philoID] = HUNGRY;
    disp_philo_states();

    while(stick_state[L_STICK(philoID)] == USED
          || stick_state[R_STICK(philoID)] == USED) {
        pthread_cond_wait(&cond[philoID], &mutex);
    }

    stick_state[L_STICK(philoID)] = USED;
    stick_state[R_STICK(philoID)] = USED;

    philo_state[philoID] = EAT;
    disp_philo_states();

    pthread_mutex_unlock(&mutex);
}

void put_sticks(int philoID)
{
    pthread_mutex_lock(&mutex);
    philo_state[philoID] = THINK;
    disp_philo_states();

    stick_state[L_STICK(philoID)] = UNUSED;
    stick_state[R_STICK(philoID)] = UNUSED;

    //Wecken der Nachbarn
    pthread_cond_signal(&cond[L_PHIL(philoID)]);
    pthread_cond_signal(&cond[R_PHIL(philoID)]);

    pthread_mutex_unlock(&mutex);
}

void disp_philo_states()
{
    int i;

    //Ausgabe der Philosophen und ihren aktuellen Zustaenden
    for(i = 0; i < NPHILO; i++) {
        if(philo_state[i] == EAT){
            printf("%d%c ", i, 'E');
        }
        else if(philo_state[i] == THINK){
            printf("%d%c ", i, 'T');
        }
        else{
            printf("%d%c ", i, 'H');
        }
    }
    printf("\n");

    //Kontrolliert ob benachbarte Philosophen gleichzeitig essen
    for(i = 0; i < NPHILO; i++) {
        if(philo_state[i] == EAT) {
            if((philo_state[L_PHIL(i)] == EAT || philo_state[R_PHIL(i)] == EAT)){
                printf("Benachbarte Philosophen duerfen nicht zeitgleich essen!\n");
                break;
            }
        }
    }
}
