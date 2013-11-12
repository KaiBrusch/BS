//
//  monitor.c
//  betriebssysteme
//
//  Created by kbrusch on 11/12/13.
//  Copyright (c) 2013 kbrusch. All rights reserved.
//

#include <stdio.h>
#include "Header.h"

void get_sticks(int pID){

    pthread_mutex_lock(mutex[pID]); // der Kritische abschnitt wird gesperrt

    philStates[pID]= HUNGRY; // Der Zustand des Philosophen wird auf Hungry gesetzt


    while (sticks[LEFT_S] == IN_USE || sticks[RIGHT_S] == IN_USE) {
        //todo StickIDs als Konstanten Deklarieren
        pthread_cond_wait(cond[pID],mutex[pID]);

    }

    philStates[pID] = EAT;
    sticks[LEFT_S] = IN_USE;
    sticks[RIGHT_S] = IN_USE;
    pthread_mutex_unlock(&mutex); // mutex wieder unlocken

}

void put_sticks(int pID) {

    pthread_mutex_lock(&mutex); //sperrt wieder den abschitt
    philStates[pID] = THINK;

    sticks[LEFT_S] = AVAILIBLE; // gibt die sticks verfügbar
    sticks[RIGHT_S] = AVAILIBLE;

    pthread_cond_signal(cond[LEFT_P]); // muss wach auf signale an die Philosophen senden
	pthread_cond_signal(cond[RIGHT_P]);

    pthread_mutex_unlock(mutex[pID]); // wieder freigeben des kritischen abschnittes

}

void disp_philo_states()
{
	int pID;
	for (pID = 0; pID< NPH; pID++)
	{
		if (((philStates[pID] == EAT)  && (philStates[LEFT_P] == EAT)) || ((philStates[pID] == EAT)  && (philStates[RIGHT_P] == EAT)))
		{
			printf("Fehler bei der Synchronisation! \n");
			break;
		}
		switch(philStates[pID])
		{
			case HUNGRY: printf("%dH ",pID ); break;
			case THINK : printf("%dT ",pID ); break;
			case EAT   : printf("%dE ",pID ); break;
	    }
	}
	printf("\n");
