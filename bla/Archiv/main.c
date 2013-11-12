#include "main.h"

int main(void)
{
    pthread_t philoThreads[NPHILO];
    int i;
    int err;
    int res[NPHILO];

    //Initialisieren
    for(i = 0; i < NPHILO; i++) {
        philoZustaende[i] = THINK;
        stickZustaende[i] = FREE;
        err = pthread_cond_init(&cond[i], NULL);
        assert(!err);
        sem_init(&semaphoren[i], 0, 0);
    }
    pthread_mutex_init(&mutex, NULL);

    //Threads starten
    for(i = 0; i < NPHILO; i++) {
        philoIDs[i] = i;
        res[i] = pthread_create(&philoThreads[i], NULL, philo, &philoIDs[i]);

        if(res[i] != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    //Tastatureingabe
    while(TRUE) {
        fgets(eingabe, EINGABELAENGE, stdin);

        if(eingabe[0] == 'q' || eingabe[0] == 'Q') {

         printf("Programm wird geschlossen...\n");
	    
	    for(i = 0; i < NPHILO; i++) {
	      steuerarray[i] = 'q';
	      sem_post(&semaphoren[eingabe[0]-ASCII_ZERO]);
	    }
        for(i = 0; i < NPHILO; i++) {
			pthread_cond_signal(&cond[i]);
            pthread_join(philoThreads[i], NULL);
            printf("Thread %d beendet\n", i);
            }
            
        //Entfernen der Synchronisationsobjekte
        for(i = 0; i < NPHILO; i++) {
			pthread_cond_destroy(&cond[i]);
            sem_destroy(&semaphoren[i]);
	    }
        pthread_mutex_destroy(&mutex);
        
        pthread_exit(NULL);
        printf("Programm beendet.\n");
        exit(EXIT_SUCCESS);
        }
        else if(eingabe[0] >= '0' && eingabe[0] < NPHILO + ASCII_ZERO) {
            if(eingabe[1] == 'b') {
                printf("Philosoph Nr. %d wird blockiert.\n", eingabe[0] - ASCII_ZERO);
                steuerarray[eingabe[0] - ASCII_ZERO] = eingabe[1];
            }
            else if(eingabe[1] == 'u') {
                printf("Philosoph Nr. %d block entfernt.\n", eingabe[0] - ASCII_ZERO);
                steuerarray[eingabe[0] - ASCII_ZERO] = 'u';
                sem_post(&semaphoren[eingabe[0] - ASCII_ZERO]);
            }
            else if(eingabe[1] == 'p') {
                printf("Philosoph Nr. %d ueberspringt Schleife.\n", eingabe[0] - ASCII_ZERO);
                steuerarray[eingabe[0] - ASCII_ZERO] = 'p';
            }
        }
    }
    return 0;
}

//Beinhaltet das Verhalten von Philosophen.
void *philo(void *pID)
{
    int *philoID = pID;
    int aktiv = 1;

    printf("Philosoph %d ist nun aktiv\n", *philoID);

    while(aktiv) {
        checkBlock(*philoID);
        think(*philoID);
        checkBlock(*philoID);
        get_sticks(*philoID);
        eat(*philoID);
        checkBlock(*philoID);
        put_sticks(*philoID);

        if(steuerarray[*philoID] == 'q' || steuerarray[*philoID] == 'Q') {
            aktiv = 0;
        }
    }
    return NULL;
}

//Ueberprueft ob der Thread blockiert ist.
void checkBlock(int philoID)
{
    if(steuerarray[philoID] == 'b') {
        sem_wait(&semaphoren[philoID]);
    }
}


//Simulieren von THINK durch Zaehlschleife
void think(int philoID)
{
    int i;
    for(i = 0; i < THINK_LOOP; i++) {
        if(steuerarray[philoID] == 'p') {
            steuerarray[philoID] = '-';
            break;
        }
    }
}

//Simulieren von EAT durch Zaehlschleife
void eat(int philoID)
{
    int i;
    for(i = 0; i < EAT_LOOP; i++) {
        if(steuerarray[philoID] == 'p') {
            steuerarray[philoID] = '-';
            break;
        }
    }
}