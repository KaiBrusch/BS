//
//  main.h
//  betriebssysteme
//
//  Created by kbrusch on 11/12/13.
//  Copyright (c) 2013 kbrusch. All rights reserved.
//

// Konstanten zur Kommunikation welches
// Kommando welcher Philosoph vom Benutzer erhalten hat
#define BLOCK 1
#define PROCEED 2
#define UNBLOCK 3
#define DEFAULT 0

// Philosopher states
#define THINK   0
#define HUNGRY  1
#define EAT     2

#define NPH 5

// LOOP durations
#define THINKLOOP   1000000000
#define EATLOOP      500000000

// sonstige
#define INPUTLEN 140
#define ASCII_NUM_OFFSET 48

// STICK CONSTANTS
#define IN_USE      1
#define AVAILIBLE   0

// BOOLEAN
#define TRUE 1
#define FALSE 0

// DECLERATIONS
void init();
void inputLoop();
void* philo(void *arg);
char *get_line(char *s, size_t n, FILE *f);
sem_t mutex;
int thread_mng[NPH];

// TODO: outsource declarations
// TODO: coderichtlinien von FOHL

// Initialisierung der globalen Daten
const char *philosNamos[NPH] = {"Kierkegaard","Descartes","Tolstoy","Mochizuki","Goedel"};
int sticks[NPH-1];      // es gibt einen sStick weniger als es Philosophen gibt
int philStates[NPH];    // all the threads write their philosophers sate into this array for easier display

// The class Philosopher
typedef struct Phil {
    const int id;
    const char *name;
    const int l_stk_id;
    const int r_stk_id;
} Philosopher;