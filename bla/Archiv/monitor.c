#include "main.h"

void get_sticks(int philoID)
{
    pthread_mutex_lock(&mutex);

    philoZustaende[philoID] = HUNGRY;
    disp_philo_states();

    while(stickZustaende[STICK_LINKS(philoID)] == IN_USE
          || stickZustaende[STICK_RECHTS(philoID)] == IN_USE) {
        pthread_cond_wait(&cond[philoID], &mutex);
    }

    stickZustaende[STICK_LINKS(philoID)] = IN_USE;
    stickZustaende[STICK_RECHTS(philoID)] = IN_USE;

    philoZustaende[philoID] = EAT;
    disp_philo_states();

    pthread_mutex_unlock(&mutex);
}

void put_sticks(int philoID)
{
    pthread_mutex_lock(&mutex);
    philoZustaende[philoID] = THINK;
    disp_philo_states();

    stickZustaende[STICK_LINKS(philoID)] = FREE;
    stickZustaende[STICK_RECHTS(philoID)] = FREE;

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
        PhiloZustand philoZustand = philoZustaende[i];
        if(philoZustand == EAT){
            printf("%d%c ", i, 'E');
        }
        else if(philoZustand == THINK){
            printf("%d%c ", i, 'T');
        }
        else{
            printf("%d%c ", i, 'H');
        }
    }
    printf("\n");

    //Kontrolliert ob benachbarte Philosophen gleichzeitig essen
    for(i = 0; i < NPHILO; i++) {
        if(philoZustaende[i] == EAT) {
            if((philoZustaende[PHILO_LINKS(i)] == EAT || philoZustaende[PHILO_RECHTS(i)] == EAT)){
                printf("Benachbarte Philosophen duerfen nicht zeitgleich essen!\n");
                break;
            }
        }
    }
}
