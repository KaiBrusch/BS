
#include "main.h"


// die Monitorfunktionen sind die get_sticks und put_sticks // sie arbeiten mit den kritischen Abschnitten


// ==========================MAIN FUNCTION
int main() {
	init();
	inputLoop();
}


void init() {
    // for each philosopher
	for(int i=0; i<NPH; i++ ) {
        thread_mng[i] = DEFAULT;        // no default commandos
        
        // creation of the philosopher
        Philosopher philosMios = {i,philosNamos[i]};
        
        int res = sem_init(mutex[i], 0, 1);
        if(res != 0){
            perror("Semaphore initialization failed\n");
            exit(EXIT_FAILURE);
        }
        
        // create threads, give the corresponding ID
        pthread_t tid;
        res = pthread_create(&tid, NULL, &philo, (void *)&philosMios);
        if(res!=0){
            perror("Thred not created.\n");
            exit(EXIT_FAILURE);
        }
    }
}


// philosophers thread function
void *philo(void *arg){
    Philosopher *p = (Philosopher*)arg;
    int pID = p->id;
    
    printf("%s\n", p->name);
    
    while (1) {
        // first state is THINK
        for(int c=0;c<THINKLOOP;c++){
            if(thread_mng[pID] == PROCEED){
                break;
            }
        }
        
        // third state is EAT
        get_sticks(pID);
        for(int c=0;c<EATLOOP;c++){
            if(thread_mng[pID] == PROCEED){
                thread_mng[pID] = DEFAULT;
                break;
            }
            else if (thread_mng[pID] == JOIN){
                thread_mng[pID] = FINISHED;
                pthread_join((pthread_t)pID, NULL);
                sem_destroy(mutex[pID]);
                pthread_exit(0);
            }
            else if(thread_mng[pID] == BLOCK){
                sem_wait(mutex[pID]);
                thread_mng[pID] = DEFAULT;
            }
        }
        put_sticks(pID);
        
        // next state again becomes THINK
        
    }
}


void inputLoop(){
	while(TRUE){
        char x[INPUTLEN];
		get_line(&x, INPUTLEN, stdin); // Eingabe lesen und nochmal anzeigen
        printf("Gelesen: %s\n", x);
        
        // philosophenID, falls eine angegeben wurde in pID speichern
        // wir muessen um den ACSII ziffernoffset verschieben da wir vom string convertieren
        int pID = ((int)x[0]) - ASCII_NUM_OFFSET;
        
        // FAll: quit
        if(x[0] == 'q' || x[0] == 'Q') {
            
            for(int j = 0; j < NPH; j++){
                //
                thread_mng[j] = JOIN;
                
                // synchronisationsobjekte loeschen
            }
            
            
            printf("Quit!");
            exit(EXIT_SUCCESS);
        }
        
        // only if the id is valid
        if(pID < NPH){
            if(x[1] == 'b') {// FAll: BLOCK
                //blockieren den philosophen
                printf("Blockiere den philosophen");
                thread_mng[pID] = BLOCK;
            }
            else if(x[1] == 'u') {// FAll: UNBLOCK
                //free den philosophen
                printf("Befreie den philosophen");
                thread_mng[pID] = UNBLOCK;
            }
            else if(x[1] == 'p') {// FAll: PROCEED
                //jump and eat,think
                printf("jump to eat,think");
                thread_mng[pID] = PROCEED;
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

// Helping function to abstract away the readLine
char *get_line(char *s, size_t n, FILE *f) {
    char *p = fgets (s, n, f);
    if (p != NULL) strtok (s, "\n");
    return p;
}