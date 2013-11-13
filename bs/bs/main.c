#include "main.h"

pthread_t philo_threads[NPHILO];

// Fehlerfaelle testen
int main(void)
{
    init();
    inputLoop();
    /*
    //Tastatureingabe
    while(TRUE) {
        fgets(eingabe, INPUT_LEN, stdin);

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
    }*/
    return 0;
}

void inputLoop(){
    
    char input[INPUT_LEN];
    int running = 1;
    
    while(running){
        
        fgets(input, INPUT_LEN, stdin); // Eingabe lesen und nochmal anzeigen
        printf("Gelesen: %s\n", input);
        
        // philosophenID, falls eine angegeben wurde in p_id speichern
        // wir muessen um den ACSII ziffernoffset verschieben da wir vom string convertieren
        int p_id = ((int) input[0]) - ASCII_NUM_OFFSET;
        
        // FAll: quit
        if(input[0] == 'q' || input[0] == 'Q') {
            
            printf("Programm wird geschlossen...\n");
            
            for(int i = 0; i < NPHILO; i++) {
                input_commands[i] = 'q';
                sem_post(&semaphoren[i]);
            }
            for(int i = 0; i < NPHILO; i++) {
                pthread_cond_signal(&cond[i]);
                pthread_join(philo_threads[i], NULL);
                printf("Thread %d is waiting for join...\n", i);
            }
            
            //Entfernen der Synchronisationsobjekte
            for(int i = 0; i < NPHILO; i++) {
                pthread_cond_destroy(&cond[i]);
                sem_destroy(&semaphoren[i]);
                printf("Thread %d was killed.\n", i);
            }
            pthread_mutex_destroy(&mutex);
            
            printf("Programm beendet.\n");
            fflush(stdout);

            exit(EXIT_SUCCESS);
        }
        
        // only if the id is valid
        if(p_id < NPHILO){
            if(input[1] == BLOCK) {// FAll: BLOCK
                printf("Blockiere den philosophen");
                input_commands[p_id] = BLOCK;
            }
            else if(input[1] == UNBLOCK) {// FAll: UNBLOCK
                //free den philosophen
                printf("Befreie den philosophen");
                sem_post(&semaphoren[p_id]);
                
            }
            else if(input[1] == PROCEED) {// FAll: PROCEED
                //jump and eat,think
                printf("Cmd: %d: jump to eat,think",p_id);
                input_commands[p_id] = PROCEED;
            }
            else {
                printf("Incorrect command.");
            }
        }
        else{
            printf("invalid philosopher-ID");
        }
    }
    
}

//Beinhaltet das Verhalten von Philosophen.
void *philo(void *p_id)
{
    int philo = (int) p_id;

    printf("Philosoph %d ist nun aktiv\n", philo);

    while(TRUE) {
        checkBlock(philo);
        think(philo);
        checkBlock(philo);
        get_sticks(philo);
        eat(philo);
        checkBlock(philo);
        put_sticks(philo);

        if(input_commands[philo] == QUIT) {
            pthread_exit(NULL);
        }
    }
    
    return NULL;
}


//Ueberprueft ob der Thread blockiert ist.
void checkBlock(int philoID)
{
    if(input_commands[philoID] == BLOCK) {
        sem_wait(&semaphoren[philoID]);
    }
}


//Simulieren von THINK durch Zaehlschleife
void think(int philoID)
{
    int i;
    for(i = 0; i < THINK_LOOP; i++) {
        if(input_commands[philoID] == PROCEED) {
            input_commands[philoID] = DEFAULT;
            break;
        }
    }
}

//Simulieren von EAT durch Zaehlschleife
void eat(int philoID)
{
    int i;
    for(i = 0; i < EAT_LOOP; i++) {
        if(input_commands[philoID] == PROCEED) {
            input_commands[philoID] = DEFAULT;
            break;
        }
    }
}



void init(){
    
    int result;
    
    //Initialize
    for(int i = 0; i < NPHILO; i++) {
        philo_state[i] = THINK;
        stick_state[i] = UNUSED;
        result = pthread_cond_init(&cond[i], NULL);
        
        if(result != 0) {
            perror("Condition Creation failed!");
            exit(EXIT_FAILURE);
        }
        
        sem_init(&semaphoren[i], 0, 0);
    }
    pthread_mutex_init(&mutex, NULL);
    
    //Create Threads
    for(int philo_id = 0; philo_id < NPHILO; philo_id++) {
        result = pthread_create(&philo_threads[philo_id], NULL, philo, (void *) philo_id);
        
        if(result != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }
    
}
