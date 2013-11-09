#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

void init_threads()
{
	print("initThreads");
}

void mainLoop(){

	int status = 1;
	while(status){
		char[] x = fgets();
		print(x);
	}

}

int main()
{
	init_threads();

	mainLoop();

}