#include "main.h"


pthread_t philo_threads[NPHILO];
char *ph_name[NPHILO] = {"Goedel", "Aristoteles", "Tolstoy", "Mochizuki", "Plato"};

// Fehlerfaelle testen
int main(void)
{
    init();
    inputLoop();
    exit(EXIT_SUCCESS);
}

void inputLoop(){
    
    char input[INPUT_LEN];
    int running = 1;
    
    while(running){
        
        fgets(input, INPUT_LEN, stdin); // Eingabe lesen und nochmal anzeigen
        printf("Read from Keyboard: %s\n", input);
        
        // philosophenID, falls eine angegeben wurde in p_id speichern
        // wir muessen um den ACSII ziffernoffset verschieben da wir vom string convertieren
        int p_id = ((int) input[0]) - ASCII_NUM_OFFSET;
        
        handle_quit(input[0]);
        
        handle_command(input[1], p_id);
        
    }
    
}

void handle_quit(char first_char){
    // FAll: quit
    if(first_char == 'q' || first_char == 'Q') {
        
        printf("Quitting...\n");
        
        for(int i = 0; i < NPHILO; i++) {
            input_commands[i] = 'q';
            sem_post(&semaphoren[i]);
        }
        for(int i = 0; i < NPHILO; i++) {
            pthread_cond_signal(&cond[i]);
            pthread_join(philo_threads[i], NULL);
            printf("%s (%d) is waiting to join...\n", ph_name[i], i);
        }
        
        //Entfernen der Synchronisationsobjekte
        for(int i = 0; i < NPHILO; i++) {
            pthread_cond_destroy(&cond[i]);
            sem_destroy(&semaphoren[i]);
            printf("%s (%d) was killed.\n", ph_name[i], i);
        }
        pthread_mutex_destroy(&mutex);
        printf("Ohh nooes! ALL dead.\n");
        printf("Exit!\n");
        fflush(stdout);
        
        exit(EXIT_SUCCESS);
    }
}

void handle_command(char cmd_char, int p_id){
    
    // only if the id is valid
    if(p_id >= NPHILO){
        printf("invalid philosopher-ID\n");
    } else {
        switch (cmd_char) {
            case BLOCK:
                printf("Block %s (%d).\n", ph_name[p_id], p_id);
                input_commands[p_id] = BLOCK;
                break;
                
            case UNBLOCK:
                printf("Unblock %s (%d).\n", ph_name[p_id], p_id);
                sem_post(&semaphoren[p_id]);
                break;
                
            case PROCEED:
                printf("%s (%d) wants to eat or think.\n", ph_name[p_id], p_id);
                input_commands[p_id] = PROCEED;
                break;
                
            default:
                printf("Incorrect command.\n");
                break;
        }
    }
}

//Beinhaltet das Verhalten von Philosophen.
void *philo(void *p_id)
{
    int philo = (int) p_id;

    printf("%s (%d) is ready to Think!\n", ph_name[philo], philo);

    while(TRUE) {
        
        think(philo);
        get_sticks(philo);
        eat(philo);
        put_sticks(philo);
        
        if(input_commands[philo] == QUIT) {
            pthread_exit(NULL);
        }
    }
    
    return NULL;
}


//Ueberprueft ob der Thread blockiert ist.
void block_philo(int philoID){
    if(input_commands[philoID] == BLOCK) {
        sem_wait(&semaphoren[philoID]);
    }
}

//Simulieren von THINK durch Zaehlschleife
void think(int philoID){
    int i;
    for(i = 0; i < THINK_LOOP; i++) {
        block_philo(philoID);
        if(input_commands[philoID] == PROCEED) {
            input_commands[philoID] = DEFAULT;
            break;
        }
    }
}

//Simulieren von EAT durch Zaehlschleife
void eat(int philoID) {
    int i;
    for(i = 0; i < EAT_LOOP; i++) {
        block_philo(philoID);
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
            perror("Condition Creation failed!\n");
            exit(EXIT_FAILURE);
        }
        
        sem_init(&semaphoren[i], 0, 0);
    }
    pthread_mutex_init(&mutex, NULL);
    
    //Create Threads
    for(int philo_id = 0; philo_id < NPHILO; philo_id++) {
        result = pthread_create(&philo_threads[philo_id], NULL, philo, (void *) philo_id);
        
        if(result != 0) {
            perror("Thread creation failed!\n");
            exit(EXIT_FAILURE);
        }
    }
    
}
