#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <monitor.h>
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
        
        // create threads, give the corresponding ID
        pthread_t tid;
        int res = pthread_create(&tid, NULL, &philo, (void *)i);
        if(res!=0){
            perror("Thred not created.");
            exit(EXIT_FAILURE);
        }
    }
}

void get_sticks(int pID){
    // TODO: write it
}

void put_sticks(int pID){
    // TODO: write it
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
            // alle threads mit pthread_join
            // synchronisationsobjekte loeschen
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




// philosophers thread function
void *philo(void *arg){
//Philosopher myphilo = ((Philosopher)arg);
    int pID = (int)arg;
    
    // TODO: threads commandos ausfuehren
    
//printf("%s", myphilo.name);
    
    while (1) {
        // first state is THINK
        for(int c=0;c<THINKLOOP;c++){}
        
        // second state in HUNGRY
        
        // third state is EAT
        get_sticks(pID);
        for(int c=0;c<EATLOOP;c++){}
        put_sticks(pID);
        
        
        // next state again becomes THINK
    }
}


// Helping function to abstract away the readLine
char *get_line(char *s, size_t n, FILE *f) {
    char *p = fgets (s, n, f);
    if (p != NULL) strtok (s, "\n");
    return p;
}