
// maopWordsCountere compiller happy so you could use strptime function
#define __USE_XOPEN
#define _XOPEN_SOURCE
#define _GNU_SOURCE

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

#define MAX_WORD_LEN 40
#define BUFFER_SIZE 1024

int ready = 0;


struct parsedArgs {
    char**** operations;
    int opNumber;
    int* opSegments;
    int** opWords;

};

struct parsedArgs parseArgs (char* path) {

    FILE* fileHandle = fopen(path,"r");


        if(!fileHandle) {
            printf("Couldn't open %s, shutting down.\n", path);
            exit(EXIT_FAILURE);
        }


    struct parsedArgs args;
    args.opNumber = 0;
    args.operations = (char****) malloc(sizeof(char***));
    args.operations[0] = (char***) malloc(sizeof(char**));
    args.operations[0][0] = (char**) malloc(sizeof(char*));
    args.operations[0][0][0] = (char*) malloc(sizeof(char));

    args.opWords = (int**) malloc(sizeof(int*));
    args.opWords[0] = (int*) malloc(sizeof(int));
    args.opSegments = (int*) malloc(sizeof(int));


    int opWordsCounter = 0;
    int opSegmentCounter = 0;

    while(getc(fileHandle) != EOF) {

        fseek(fileHandle , -1 ,SEEK_CUR);

        int j = args.opNumber;
        args.opNumber += 1;
        args.operations = (char****) realloc(args.operations, args.opNumber*sizeof(char***));
        args.operations[j] = (char***) realloc(args.operations[j], sizeof(char**));
        args.opWords = (int**) realloc(args.opWords, args.opNumber*sizeof(int*));
        args.opSegments = (int*) realloc(args.opSegments, args.opNumber*sizeof(int));


        while(1) {

            char nextCharacter = getc(fileHandle);
            //printf("%i %c -> ", nextCharacter, nextCharacter);
            while(nextCharacter == ' ')
                nextCharacter = getc(fileHandle);

            if(nextCharacter == '\n' || nextCharacter == EOF)
                break;


            if(nextCharacter == '|') {
                opSegmentCounter++;
                opWordsCounter = 0;
                nextCharacter = getc(fileHandle);
                while(nextCharacter == ' ')
                    nextCharacter = getc(fileHandle);
                args.operations[j] = (char***) realloc(args.operations[j], (opSegmentCounter+1)*sizeof(char**));
            }

            fseek(fileHandle , -1 ,SEEK_CUR);

            //printf("%i %c\n", nextCharacter, nextCharacter);


            args.operations[j][opSegmentCounter] = (char**) realloc(args.operations[j][opSegmentCounter], (opWordsCounter+3)*sizeof(char*));
            args.operations[j][opSegmentCounter][opWordsCounter] = (char*) calloc(sizeof(char), MAX_WORD_LEN);

            fscanf(fileHandle, "%s", args.operations[j][opSegmentCounter][opWordsCounter]);
            //printf("%i, %i, %i\n", j, opSegmentCounter, opWordsCounter);
            //printf("%s\n", args.operations[j][opSegmentCounter][opWordsCounter]);
            opWordsCounter++;

            args.opWords[j] = (int*) realloc(args.opWords[j], (opSegmentCounter+1)*sizeof(int));
            args.opWords[j][opSegmentCounter] = opWordsCounter;


        }
        for(int i = 0; i < args.opSegments[j]; i++) {
            args.operations[j][i][opWordsCounter] = NULL;
        }
        args.opSegments[j] = opSegmentCounter+1;

        opWordsCounter = 0;
        opSegmentCounter = 0;
    }
    /*
    printf("%i\n", args.opNumber);
    for(int i = 0; i < args.opNumber; i++) {
         printf("%i ", args.opSegments[i]);
    } printf("\n");

    for(int i = 0; i < args.opNumber; i++) {
        for(int j = 0; j < args.opSegments[i]; j++) {
            printf("%i ", args.opWords[i][j]);
        } printf("\n");
    }
    */
    /*
        //printf("%i total op number", args.opNumber);
        for(int i = 0; i < args.opNumber; i++) {
            //printf("\n%i operations\n", args.opWords[i]);
            for(int j = 0; j < args.opSegments[i]; j++) {
                for(int k = 0; k < args.opWords[i][j]; k++) {
                     printf("%s ", args.operations[i][j][k]);
                }
                printf(" | ");
            }
            printf("\n");
        }
    */

    return args;
}

void doNothing(int signal, siginfo_t* info, void* ucontext) { return; }
void pingBack(int signal, siginfo_t* info, void* ucontext) {

    kill(info -> si_pid, SIGUSR1);

}

void getReady(int signal, siginfo_t* info, void* ucontext) {

    ready = 1;

}

int main( int argc, char* argv[] ) {

    struct sigaction getPinged;
    getPinged.sa_sigaction = getReady;
    getPinged.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &getPinged, NULL);



    if(argc != 2)
        exit(EXIT_FAILURE);
    struct parsedArgs args = parseArgs(argv[1]);

    for(int i = 0; i < args.opNumber; i++) {

        int** fd = (int**) malloc(sizeof(int*)*args.opSegments[i]);
        pid_t * pid = (pid_t*) malloc(sizeof(pid_t)*args.opSegments[i]);

        for(int k = 0; k < args.opSegments[i]; k++) {

        fd[k] = (int*) malloc(sizeof(int)*2);

        if(pipe(fd[k])) {
            printf("could not pipe :(\n");
            exit(EXIT_FAILURE);
        }

        pid[k] = fork();
        //printf("%i\n", pid[k]);
        int status;

        if(pid[k] == 0) {

            if(k < args.opSegments[i] - 1)
                dup2(fd[k][1],STDOUT_FILENO);


            if(k > 0) {


                struct sigaction gettingReady;
                gettingReady.sa_sigaction = getReady;
                gettingReady.sa_flags = SA_SIGINFO;
                sigaction(SIGUSR1, &gettingReady, NULL);

                dup2(fd[k-1][0],STDIN_FILENO);
                close(fd[k-1][1]);
                close(fd[k-1][0]);

                while (!ready) {
                    kill(pid[k-1], SIGUSR1);
                }



            }

            if(k < args.opSegments[i] - 1) {

                struct sigaction pingingBack;
                pingingBack.sa_sigaction = pingBack;
                pingingBack.sa_flags = SA_SIGINFO;
                sigaction(SIGUSR1, &pingingBack, NULL);

                sigset_t mask;
                sigfillset(&mask);
                sigdelset(&mask,SIGUSR1);
                sigsuspend(&mask);

                struct sigaction doingNothing;
                doingNothing.sa_sigaction = doNothing;
                doingNothing.sa_flags = SA_SIGINFO;
                sigaction(SIGUSR1, &doingNothing, NULL);

            }

/*
            char buffer[BUFFER_SIZE];

            ssize_t nbytes = 1;
            while(nbytes > 0) {
                nbytes = read(fd[k][0], buffer, sizeof(buffer));
                if (write(fd[k][1], buffer, nbytes) != nbytes) {
                    printf("Error while writing to pipe");
                    exit(EXIT_FAILURE);
                }
            }
*/

            if(execvp(args.operations[i][k][0],args.operations[i][k])) {
                char* fullName = args.operations[i][k][0];
                while (strstr(args.operations[i][k][0], "/") != NULL) {
                    int x = 0;
                    while(args.operations[i][k][0][x] != '/')
                        x++;
                    x++;
                    strncpy(args.operations[i][k][0], args.operations[i][k][0]+x, MAX_WORD_LEN - x);
                }

                printf("%i total op number", args.opNumber);
                for(int i = 0; i < args.opNumber; i++) {
                    //printf("\n%i operations\n", args.opWords[i]);
                    for(int j = 0; j < args.opWords[i][k]; j++) {
                    printf("%s ", args.operations[i][k][j]);
                    }
                    printf("\n");
                }

            if(execv(fullName,args.operations[i][k]))
                 exit(errno);
            }
            exit(errno);



        }

        else {


            if(k == args.opSegments[i] - 1) {
                for(int o = 0; o < args.opSegments[i]; o++) {
                    close(fd[k-1][1]);
                    close(fd[k-1][0]);
                }
                waitpid(pid[k], &status, 0);


            }


            printf("Executed");
            for (int j = 0; j < args.opWords[i][k]; j++) {
                 printf(" %s", args.operations[i][k][j]);
            }
            if (k == args.opSegments[i] - 1) {
                if (status != 0)
                    printf(" which FAILED with error number %i", status);
            }

            printf("\n");
        }

    }
}

    errno = 0;
    while (errno != ECHILD)
       wait(NULL);
    errno = 0;

    return 0;
}
