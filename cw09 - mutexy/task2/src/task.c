#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>
#include <math.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\x1B[0m"


char** work_arr;
int N, P, C;
size_t L;
int curr_N_p = 0;
int curr_N_c = 0;
int w_mode;
char s_mode;
int end_flag = 0;
FILE *file;
pthread_t* producers = NULL;
pthread_t* consumers = NULL;
sem_t* prod_arr_m;
sem_t* cons_arr_m;
sem_t* file_m;
sem_t** arr_m;
sem_t** arr_m_p;

int * array_helper;
int done = -10;

#define MAX_LINE_SIZE 900

void clean_sem() {
    sem_close(prod_arr_m);
    sem_close(cons_arr_m);
    sem_close(file_m);

    sem_unlink("/prod_arr_m");
    sem_unlink("/cons_arr_m");
    sem_unlink("/file_m");

    for(int i = 0; i < N; i++) {
        char arr_m_name[20];
        sprintf(arr_m_name, "/arr_m%i", i);
        char arr_m_p_name[20];
        sprintf(arr_m_p_name, "/arr_m_p%i", i);
        sem_close(arr_m[i]);
        sem_unlink(arr_m_name);
        sem_close(arr_m_p[i]);
        sem_unlink(arr_m_p_name);
    }
}

void clean() {
    printf("Cleaning\n");
    if (producers != NULL && consumers != NULL) {
        for(int i = 0; i < P; i++) {
            pthread_cancel(producers[i]);
        }
        for(int i = 0; i < C; i++) {
            pthread_cancel(consumers[i]);
        }
    }
    clean_sem();
}

int semaphore_op(int op, sem_t *sem) {

    if(op == -1) {
        if(sem_post(sem) == -1)
             perror("sem_post failed");
         }
    else if(op == 1) {
        if(sem_wait(sem) == -1)
             perror("sem_wait failed");
        }
    else if(op == 0) {
        int *valp = (int*) malloc(sizeof(int));
        *valp = 0;
        while(*valp <= 0)
            sem_getvalue(sem, valp);
    }
    return 0;
}

void *producer(void *args) {

    int* num = (int*) args;


    int r;
    while(1) {
        char* line_buffer = (char*) malloc(sizeof(char) * MAX_LINE_SIZE);
        semaphore_op(1, prod_arr_m);
        int curr_n = curr_N_p;
        curr_N_p += 1;
        if(curr_N_p == N)
            curr_N_p = 0;

        semaphore_op(1, arr_m_p[curr_n]);
        semaphore_op(-1, prod_arr_m);
        //pthread_mutex_lock(&file_m);
        //pthread_mutex_unlock(&prod_arr_m);
        r = fscanf(file, "%[^\n]\n", line_buffer);
        //pthread_mutex_unlock(&file_m);

        if(r == EOF) {
            semaphore_op(-1, arr_m_p[curr_n]);
            semaphore_op(-1, prod_arr_m);
            break;
        }
        while(work_arr[curr_n] != NULL);

        if(w_mode)
            printf(KBLU "P%i %i %s\n" RESET, num[0], curr_n, line_buffer);
        work_arr[curr_n] = line_buffer;
        semaphore_op(-1, arr_m_p[curr_n]);


    }

    return NULL;
}

void *consumer(void *args) {

    while(1) {
        semaphore_op(1, cons_arr_m);
        int curr_n = curr_N_c;
        curr_N_c += 1;
        if(curr_N_c == N)
            curr_N_c = 0;
        //if(array_helper[curr_n] == 1) {
        //    pthread_mutex_unlock(&cons_arr_m);
        //    continue;
        //}


        semaphore_op(1, arr_m[curr_n]);
        //pthread_mutex_unlock(&cons_arr_m);
        while(work_arr[curr_n] == NULL) {
            if(end_flag == 1) {
                semaphore_op(-1, cons_arr_m);
                return NULL;
            }
        }
        semaphore_op(-1, cons_arr_m);

        if(s_mode == '=') {
            if(strlen(work_arr[curr_n]) == L) {
                 printf("%i %s\n", curr_n, work_arr[curr_n]);
            }
        } else
        if(s_mode == '>') {
            if(strlen(work_arr[curr_n]) > L) {
                 printf("%i %s\n", curr_n, work_arr[curr_n]);
            }
        } else
        if(s_mode == '<') {
            if(strlen(work_arr[curr_n]) < L) {
                 printf("%i %s\n", curr_n, work_arr[curr_n]);
            }
        }

        free(work_arr[curr_n]);
        work_arr[curr_n] = NULL;
        semaphore_op(-1, arr_m[curr_n]);



    }

    return NULL;
}

void finish(int signal) {

    clean();
    printf("%c%cExiting \n", 8, 8);
    exit(EXIT_SUCCESS);

}

int main( int argc, char* argv[] ) {

    struct sigaction grand_finale;
    grand_finale.sa_handler = finish;
    grand_finale.sa_flags = 0;
    sigaction(SIGINT, &grand_finale, NULL);

    if(argc < 9)
        exit(EXIT_FAILURE);
    P =  strtol(argv[1], NULL, 10);
    C =  strtol(argv[2], NULL, 10);
    N =  strtol(argv[3], NULL, 10);
    char* f_path = argv[4];
    L =  strtol(argv[5], NULL, 10);
    s_mode = argv[6][0];
    if(s_mode != '=' && s_mode != '>' && s_mode != '<')
        exit(EXIT_FAILURE);
    w_mode = strtol(argv[7], NULL, 10);
    int nk =  strtol(argv[8], NULL, 10);
    producers = (pthread_t*) malloc(sizeof(pthread_t) * P);
    consumers = (pthread_t*) malloc(sizeof(pthread_t) * C);
    work_arr = (char**) malloc(sizeof(char*) * N);


    file = fopen(f_path, "r");
    if (file == NULL) {
        perror("Error while opening file");
    }
    prod_arr_m = sem_open("/prod_arr_m", O_RDWR | O_CREAT | O_EXCL , 0660, 1);
    cons_arr_m = sem_open("/cons_arr_m", O_RDWR | O_CREAT | O_EXCL , 0660, 1);
    file_m = sem_open("/file_m", O_RDWR | O_CREAT | O_EXCL , 0660, 1);

    arr_m = (sem_t**) malloc(sizeof(sem_t*) * N);
    arr_m_p = (sem_t**) malloc(sizeof(sem_t*) * N);
    array_helper = (int*) malloc(sizeof(int) * N);
    for(int i = 0; i < N; i++) {
        char arr_m_name[20];
        sprintf(arr_m_name, "/arr_m%i", i);
        char arr_m_p_name[20];
        sprintf(arr_m_p_name, "/arr_m_p%i", i);
        arr_m[i] = sem_open(arr_m_name, O_RDWR | O_CREAT | O_EXCL , 0660, 1);
        arr_m_p[i] = sem_open(arr_m_p_name, O_RDWR | O_CREAT | O_EXCL , 0660, 1);
        array_helper[i] = 0;
    }

    for(int i = 0; i < P; i++) {
        int *num = (int*) malloc(sizeof(int));
        num[0] = i;
        pthread_create(&producers[i], NULL, producer, num);
    }
    for(int i = 0; i < C; i++) {
        int *num = (int*) malloc(sizeof(int));
        num[0] = i;
        pthread_create(&consumers[i], NULL, consumer, num);
    }


    if(nk == 0) {
        for(int i = 0; i < P; i++) {
            pthread_join(producers[i], NULL);
        }
        if(w_mode)
            printf(KBLU "Producers ended job\n" RESET);
        end_flag = 1;
        for(int i = 0; i < C; i++) {
            pthread_join(consumers[i], NULL);
        }
        clean_sem();
    } else {
        sleep(nk);
        clean();
    }



    fclose(file);


    return 0;
}
