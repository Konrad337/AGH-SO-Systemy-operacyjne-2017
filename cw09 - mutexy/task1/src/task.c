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
pthread_mutex_t prod_arr_m;
pthread_mutex_t cons_arr_m;
pthread_mutex_t file_m;
pthread_mutex_t* arr_m;
pthread_mutex_t* arr_m_p;

int * array_helper;
int done = -10;

#define MAX_LINE_SIZE 900

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
}

void *producer(void *args) {

    int* num = (int*) args;


    int r;
    while(1) {
        char* line_buffer = (char*) malloc(sizeof(char) * MAX_LINE_SIZE);
        pthread_mutex_lock(&prod_arr_m);
        int curr_n = curr_N_p;
        curr_N_p += 1;
        if(curr_N_p == N)
            curr_N_p = 0;

        pthread_mutex_lock(&arr_m_p[curr_n]);
        pthread_mutex_unlock(&prod_arr_m);
        //pthread_mutex_lock(&file_m);
        //pthread_mutex_unlock(&prod_arr_m);
        r = fscanf(file, "%[^\n]\n", line_buffer);
        //pthread_mutex_unlock(&file_m);

        if(r == EOF) {
            pthread_mutex_unlock(&arr_m_p[curr_n]);
            pthread_mutex_unlock(&prod_arr_m);
            break;
        }
        while(work_arr[curr_n] != NULL);

        if(w_mode)
            printf(KBLU "P%i %i %s\n" RESET, num[0], curr_n, line_buffer);
        work_arr[curr_n] = line_buffer;
        pthread_mutex_unlock(&arr_m_p[curr_n]);


    }

    return NULL;
}

void *consumer(void *args) {

    while(1) {
        pthread_mutex_lock(&cons_arr_m);
        int curr_n = curr_N_c;
        curr_N_c += 1;
        if(curr_N_c == N)
            curr_N_c = 0;
        //if(array_helper[curr_n] == 1) {
        //    pthread_mutex_unlock(&cons_arr_m);
        //    continue;
        //}


        pthread_mutex_lock(&arr_m[curr_n]);
        //pthread_mutex_unlock(&cons_arr_m);
        while(work_arr[curr_n] == NULL) {
            if(end_flag == 1) {
                pthread_mutex_unlock(&cons_arr_m);
                return NULL;
            }
        }
        pthread_mutex_unlock(&cons_arr_m);

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
        pthread_mutex_unlock(&arr_m[curr_n]);



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
    pthread_mutex_init (&prod_arr_m, NULL);
    pthread_mutex_init (&cons_arr_m, NULL);
    pthread_mutex_init (&file_m, NULL);
    arr_m = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t) * N);
    arr_m_p = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t) * N);
    array_helper = (int*) malloc(sizeof(int) * N);
    for(int i = 0; i < N; i++) {
        pthread_mutex_init (&arr_m[i], NULL);
        pthread_mutex_init (&arr_m_p[i], NULL);
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
    } else {
        sleep(nk);
        clean();
    }



    fclose(file);


    return 0;
}
