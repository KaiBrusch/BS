#include "main.h"

// module variables

// philosophers problem
int philo_state[NPHILO];      // global
int stick_state[NPHILO];      // global

// sync objects
pthread_mutex_t mutex;        // global
pthread_cond_t cond[NPHILO];  // global
sem_t semaphores[NPHILO];

// commands for communication
char input_commands[NPHILO];
pthread_t philo_threads[NPHILO];

// how about names?
char *ph_name[NPHILO] = {
    "Goedel",
    "Aristoteles",
    "Tolstoy",
    "Mochizuki",
    "Plato"
};

int main(void) {
    init();
    inputLoop();
    exit(EXIT_SUCCESS);
}

void inputLoop(){

    char input[INPUT_LEN];
    char first_char_idx = 0;
    char second_char_idx = 1;
    
    while(TRUE){
        
        // read input and show it before processing
        fgets(input, INPUT_LEN, stdin);
        printf("Read from Keyboard: %s\n", input);
        
        // calculate the p_id form the first char
        int p_id = ((int) input[first_char_idx]) - ASCII_NUM_OFFSET;
        
        // this will be executed if a 'q' or 'Q' was the first letter
        handle_quit(input[first_char_idx]);
        
        // this will be executed if it was a command
        handle_command(input[second_char_idx], p_id);
        
    }
    
}

void handle_quit(char first_char){
    if(first_char == 'q' || first_char == 'Q') {
        
        printf("Quitting...\n");
        
        int result;
        
        // send quit commands and release all from any semaphore blocks
        for(int i = 0; i < NPHILO; i++) {
            input_commands[i] = QUIT;
            sem_post(&semaphores[i]);
        }
        // join all threads
        for(int i = 0; i < NPHILO; i++) {
            pthread_cond_signal(&cond[i]);
            result = pthread_join(philo_threads[i], NULL);
            if(result != 0){
               perror("Thread(%d) join failed\n", i);
               exit(EXIT_FAILURE);
            }
            printf("%s (%d) is waiting to join...\n", ph_name[i], i);
        }
        
        // destroy sycn objects(semaphores and condition variables) and mutex
        for(int i = 0; i < NPHILO; i++) {
            result = pthread_cond_destroy(&cond[i]);
            if(result != 0){
               perror("cond_var(%d) destruction failed!\n", i);
               exit(EXIT_FAILURE);
            }
            sem_destroy(&semaphores[i]);
            printf("%s (%d) was killed.\n", ph_name[i], i);
        }
        result = pthread_mutex_destroy(&mutex);
        if(result != 0){
           perror("Mutex destruction failed!\n");
           exit(EXIT_FAILURE);
        }
        
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
			input_commands[p_id] = DEFAULT
            pthread_exit(NULL);
        }
    }
    
    return NULL;
}


// Check if the philo is to be blocked and block it
void block_philo(int p_id){
    if(input_commands[p_id] == BLOCK) {
		input_commands[p_id] = DEFAULT
        sem_wait(&semaphores[p_id]);
    }
}

// THINK is a idleloop. If a BLOCK or PROCEED commad has come,
// it'll block itself or quit the loop.
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
        
        result = sem_init(&semaphores[i], 0, 0);
        if(result != 0){
           perror("Semaphore initialization failed!\n");
           exit(EXIT_FAILURE);
        }
    }
    
    // init mutual exclusion
    result = pthread_mutex_init(&mutex, NULL);
    if(result != 0){
           perror("Mutex initialization failed!\n");
           exit(EXIT_FAILURE);
    }
    
    //Create Threads
    for(int p_id = 0; p_id < NPHILO; p_id++) {
        result = pthread_create(&philo_threads[p_id], NULL, philo, (void *) p_id);
        if(result != 0) {
            perror("Thread creation failed!\n");
            exit(EXIT_FAILURE);
        }
    }
    
}
