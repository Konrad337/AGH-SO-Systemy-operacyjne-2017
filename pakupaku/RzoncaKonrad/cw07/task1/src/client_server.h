#ifndef CLIENT_SERVER_H_
#define CLIENT_SERVER_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/sem.h>

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\x1B[0m"

#define DESIRED_KEY_NUMBER 42
#define DESIRED_KEY_NUMBER2 420
#define SHARED_MEMORY_SIZE 10240


#define SLEEPY_MAN_PID 0
#define SLEEPY_OLD_MAN 4
#define CHAIRS 5
#define CHAIRS_TAKEN 6
#define GHOST_CHAIR 7
#define WAKER 8
#define NEXT 9
#define FIFO_START 10
#define FIFO_END SHARED_MEMORY_SIZE/sizeof(unsigned int)


void* shared_memory_adress = NULL;
int shared_memory_id = 0;
int semaphore_id = 0;


union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};

int semaphore_op(int op, int which) {

    struct sembuf sem;
    sem.sem_num = which;
    sem.sem_flg = 0;
    sem.sem_op = op;

    return semop(semaphore_id, &sem, 1);

}

int semaphore_op2(int op, int op2, int which) {

    struct sembuf* sem;
    sem = (struct sembuf*) malloc(sizeof(struct sembuf) * 2);

    for(int i = 0; i < 2; i++) {
        sem[i].sem_num = which;
        sem[i].sem_flg = 0;
    }
    sem[0].sem_op = op;
    sem[1].sem_op = op2;

    return semop(semaphore_id, sem, 2);

}

#endif
