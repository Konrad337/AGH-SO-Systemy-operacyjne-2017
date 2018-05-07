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
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <semaphore.h>


#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\x1B[0m"

#define DESIRED_KEY_NUMBER "/shm"
#define DESIRED_KEY_NUMBER2 "/sem"
#define DESIRED_KEY_NUMBER3 "/semm"
#define DESIRED_KEY_NUMBER4 "/semmmm"
#define DESIRED_KEY_NUMBER5 "/semmmmm"
#define DESIRED_KEY_NUMBER6 "/semmmmmm"
#define DESIRED_KEY_NUMBER7 "/semmmmmmm"

#define SHARED_MEMORY_SIZE 1024


#define SLEEPY_MAN_PID 0
#define SLEEPY_OLD_MAN 4
#define CHAIRS 5
#define CHAIRS_TAKEN 6
#define GHOST_CHAIR 7
#define WAKER 8
#define NEXT 9
#define FIFO_START 10
#define FIFO_END SHARED_MEMORY_SIZE/sizeof(int)


void* shared_memory_adress = NULL;
int shared_memory_id = 0;
sem_t* semaphore_id_0 = 0;
sem_t* semaphore_id_1 = 0;
sem_t* semaphore_id_2 = 0;
sem_t* semaphore_id_3 = 0;
sem_t* semaphore_id_4 = 0;


int semaphore_op(int op, sem_t *sem) {

    if(op == 1)
        return sem_post(sem);
    else if(op == -1)
        return sem_wait(sem);
    else if(op == 0) {
        int *valp = (int*) malloc(sizeof(int));
        *valp = 1;
        while(*valp > 0)
            sem_getvalue(sem, valp);
    }
    return 0;
}

void semaphore_op2(int op, int op2, sem_t *sem) {
    semaphore_op(op, sem);
    semaphore_op(op2, sem);
}


#endif
