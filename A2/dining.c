#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
 
#define N 5
const char *names[N] = { "Aristotle", "Kant", "Spinoza", "Marx", "Russell" };
pthread_mutex_t forks[N];
 
#define lock pthread_mutex_lock
#define unlock pthread_mutex_unlock

void print(int y, int x, const char *fmt, ...)
{
	
}
 
void eat(int id)
{

}
 
void think(int id)
{

}
 
void philosophize()
{
	print("yolo!");
}
 
int main()
{
	int i, id[N];
	pthread_t tid[N];
 
	for (i = 0; i < N; i++)
		pthread_mutex_init(forks + (id[i] = i), 0);
 
	for (i = 0; i < N; i++)
		pthread_create(tid + i, 0, philosophize, id + i);
 
	/* wait forever: the threads don't actually stop */
	return pthread_join(tid[0], 0);
}