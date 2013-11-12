//
//  Header.h
//  betriebssysteme
//
//  Created by Ivan M on 12.11.13.
//  Copyright (c) 2013 kbrusch. All rights reserved.
//

#ifndef betriebssysteme_Header_h
#define betriebssysteme_Header_h

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Anzahl der Philosophen
#define NPH         5

// Konstanten zur Kommunikation welches
// Kommando welcher Philosoph vom Benutzer erhalten hat
#define BLOCK       1
#define PROCEED     2
#define UNBLOCK     3
#define DEFAULT     0

// Statusdefinition des Philosophen
#define THINK		0
#define HUNGRY		1
#define EAT         2
// STICK CONSTANTS
#define IN_USE      1
#define AVAILIBLE   0
// Beschreibt die Stick ID
#define LEFT_S      pID
#define RIGHT_S		((pID+1)%NPH)
// LOOP durations
#define THINKLOOP   1000000000
#define EATLOOP     500000000
// Input size MAX
#define MAX_INPUT 	5
// BOOLEAN
#define TRUE        1
#define FALSE       0
// definiert den linken und den rechten philo
#define LEFT_P   ((pID - 1 + NPH) % NPH)
#define RIGHT_P  ((pID + 1) % NPH)


///Method Declaration/////
void init();
void inputLoop();
void* philo(void *arg);
char *get_line(char *s, size_t n, FILE *f);
sem_t *mutex[NPH];
int thread_mng[NPH];

// TODO: outsource declarations
// TODO: coderichtlinien von FOHL

//SOLLTE AUCH IN DEI PHILO.C AUSGELAGERT WERDEN
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
    int state; // das nacher refaktorisieren
} Philosopher;

//SOLLTE UNBEDINGT AUSGELAGERT WERDEN IN DIE PHILO.C
pthread_cond_t *cond[NPH];

Philosopher *unsere_philos[NPH];
#endif
