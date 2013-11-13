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

    while(stick_state[STICK_LINKS(philoID)] == USED
          || stick_state[STICK_RECHTS(philoID)] == USED) {
        pthread_cond_wait(&cond[philoID], &mutex);
    }

    stick_state[STICK_LINKS(philoID)] = USED;
    stick_state[STICK_RECHTS(philoID)] = USED;

    philo_state[philoID] = EAT;
    disp_philo_states();

    pthread_mutex_unlock(&mutex);
}

void put_sticks(int philoID)
{
    pthread_mutex_lock(&mutex);
    philo_state[philoID] = THINK;
    disp_philo_states();

    stick_state[STICK_LINKS(philoID)] = UNUSED;
    stick_state[STICK_RECHTS(philoID)] = UNUSED;

    //Wecken der Nachbarn
    pthread_cond_signal(&cond[PHILO_LINKS(philoID)]);
    pthread_cond_signal(&cond[PHILO_RECHTS(philoID)]);

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
            if((philo_state[PHILO_LINKS(i)] == EAT || philo_state[PHILO_RECHTS(i)] == EAT)){
                printf("Benachbarte Philosophen duerfen nicht zeitgleich essen!\n");
                break;
            }
        }
    }
}
