#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define CHAIRS 6
#define STYLISTS 1
#define CUSTOMERS 75
#define DELAY 1000000

sem_t customers;
sem_t stylists;
sem_t mutex;

int waiting = 0;
int haircuts = 0;

//pshared set to 0 to prevent other 
//threads from accessing semaphore at the same time
int pshared = 0;

void stylist(void);
void customer(void);

void main(void){

    sem_init(&mutex, pshared, 1);
    sem_init(&stylists, pshared, 0);
    sem_init(&customers, pshared, 0);

    int i = STYLISTS;
    pthread_t stylistsThreads[STYLISTS];
    pthread_create(&stylistsThreads[i], NULL,(void*)stylist, NULL);
    pthread_join(stylistsThreads[0], NULL);

    pthread_t customersThreads[CUSTOMERS];
    for(int j = 0; j < CUSTOMERS; j++) {
        pthread_create(&customersThreads[j], NULL,(void*)customer, NULL);
    }
    for(int j = 0; j < CUSTOMERS; j++) {
        pthread_join(customersThreads[j], NULL);
    }
    return;
}

void stylist(void){

    int j;
    while(1){
        
        if(haircuts == CUSTOMERS) {
            printf("All Customers have had a haricut!\n");
            break;
        }
        
        sem_wait(&mutex);

        if(waiting == 0){
            printf("[");
            for(int i = 0; i< CHAIRS; i++){
            printf("_");
            }
            printf("] Shop is empty, stylist goes to sleep\n");
        }

        sem_post(&mutex);
        sem_wait(&customers);
        sem_wait(&mutex); 

        waiting--;

        printf("[");
        for(int i = 0; i< waiting; i++){
            printf("C");
        }
        for(int i = 0; i< CHAIRS - waiting; i++){
            printf("_");
        }
        printf("]\n");

        sem_post(&stylists);
        sem_post(&mutex);

        haircuts++;
        printf("This is haircut number %d \n", haircuts);

        for( int i = 0; i < DELAY; i++);
    }
}

void customer(void){
    int j;

    while(1){
        sem_wait(&mutex);

        if(waiting < CHAIRS){
            
            waiting++;
            
            printf("[");
            for(int i = 0; i< waiting; i++){
                printf("C");
            }
            for(int i = 0; i< CHAIRS - waiting; i++){
                printf("_");
            }
            printf("]\n");

            sem_post(&customers);
            sem_post(&mutex);
            sem_wait(&stylists);

            printf("Customer is getting a haircut\n");
            if(haircuts == CUSTOMERS) {
            printf("All Customers have had a haircut!\n");
            }

            break;
        }else{
            sem_post(&mutex);
            for( int i = 0; i < DELAY*100; i++);
        }
    }
}