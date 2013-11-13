#include "main.h"


//Speichert den Zustand der Philosophen
int philo_state[NPHILO];
//Speichert den Zustand der Sticks
int stick_state[NPHILO];
//Mutex
pthread_mutex_t mutex;
//Eine Condition Variable je Philosoph
pthread_cond_t cond[NPHILO];
//Ein Semaphor je Philosoph
sem_t semaphores[NPHILO];
//Befehle fuer Philosophen werden hier abgespeichert.
//"q"=quit, "b"=block, "u"=unblock, "p"=proceed, "-"=kein Befehl
char input_commands[NPHILO];
pthread_t philo_threads[NPHILO];
char *ph_name[NPHILO] = {"Goedel", "Aristoteles", "Tolstoy", "Mochizuki", "Plato"};

int main(void) {
    init();
    inputLoop();
    exit(EXIT_SUCCESS);
}

void inputLoop(){

    char input[INPUT_LEN];
    
    while(TRUE){
        
		// read input and show it before processing
        fgets(input, INPUT_LEN, stdin);
        printf("Read from Keyboard: %s\n", input);
        
        // calculate the p_id
        int p_id = ((int) input[0]) - ASCII_NUM_OFFSET;
        
        handle_quit(input[0]);	// this will be executed if a 'q' or 'Q' was the first letter
        
        handle_command(input[1], p_id);	// this will be executed if it was a command
        
    }
    
}

void handle_quit(char first_char){
    if(first_char == 'q' || first_char == 'Q') {
		
        printf("Quitting...\n");
		
		// send quit commands and release all from any semaphore blocks
        for(int i = 0; i < NPHILO; i++) {
            input_commands[i] = QUIT;
            sem_post(&semaphores[i]);
        }
		// join all threads
        for(int i = 0; i < NPHILO; i++) {
            pthread_cond_signal(&cond[i]);
            pthread_join(philo_threads[i], NULL);
            printf("%s (%d) is waiting to join...\n", ph_name[i], i);
        }
        
        // destroy sycn objects(semaphores and condition variables) and mutex
        for(int i = 0; i < NPHILO; i++) {
            pthread_cond_destroy(&cond[i]);
            sem_destroy(&semaphores[i]);
            printf("%s (%d) was killed.\n", ph_name[i], i);
        }
        pthread_mutex_destroy(&mutex);
		
        printf("Ohh nooes! ALL dead.\n");
        printf("Exit!\n");
        
        exit(EXIT_SUCCESS);
    }
}

void handle_command(char cmd_char, int p_id){
    
    // only if the id is valid
    if(p_id >= NPHILO){
        printf("Invalid philosopher-ID: %d\n", p_id);
    } else {
        switch (cmd_char) {
            case BLOCK:
                printf("Block %s (%d).\n", ph_name[p_id], p_id);
                input_commands[p_id] = BLOCK;
                break;
                
            case UNBLOCK:
                printf("Unblock %s (%d).\n", ph_name[p_id], p_id);
                sem_post(&semaphores[p_id]);
                break;
                
            case PROCEED:
                printf("%s (%d) wants to eat or think.\n", ph_name[p_id], p_id);
                input_commands[p_id] = PROCEED;
                break;
                
            default:
                printf("Incorrect command: %s\n", cmd_char);
                break;
        }
    }
}

// philosopher-thread
void *philo(void *p_id) {
    int pid = (int) p_id;

    printf("%s (%d) is ready to Think!\n", ph_name[pid], pid);

    while(TRUE) {
        think(pid);
		
        get_sticks(pid);
		
        eat(pid);
		
        put_sticks(pid);
        
        if(input_commands[pid] == QUIT) {
            pthread_exit(NULL);
        }
    }
    
    return NULL;
}


// Check if the philo is to be blocked and block it. (is called by the corresponding thread)
void block_philo(int p_id){
    if(input_commands[p_id] == BLOCK) {
        sem_wait(&semaphores[p_id]);
    }
}

// THINK is a idleloop. If a BLOCK or PROCEED commad has come, it'll block itself or quit the loop.
void think(int p_id){
    for(int i = 0; i < THINK_LOOP; i++) {
        block_philo(p_id);
        if(input_commands[p_id] == PROCEED) {
            input_commands[p_id] = DEFAULT;
            break;
        }
    }
}

// EAT idleloop. Mostly equal to as think(...).
void eat(int p_id) {
    for(int i = 0; i < EAT_LOOP; i++) {
        block_philo(p_id);
        if(input_commands[p_id] == PROCEED) {
            input_commands[p_id] = DEFAULT;
            break;
        }
    }
}



// Initialize
void init(){
    
    int result;
    
	// initialize for pthread_cond... set default values and initialize semaphores
    for(int i = 0; i < NPHILO; i++) {
        philo_state[i] = THINK;
        stick_state[i] = UNUSED;
		input_commands[i] = DEFAULT;
		
        result = pthread_cond_init(&cond[i], NULL);
        
        if(result != 0) {
            perror("Condition Creation failed!\n");
            exit(EXIT_FAILURE);
        }
        
        sem_init(&semaphores[i], 0, 0);
    }
	
	// init mutual exclusion
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
