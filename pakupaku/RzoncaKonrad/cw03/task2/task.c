
// make compiller happy so you could use strptime function
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

struct parsedArgs {
    char*** operations;
    int opNumber;
    int* opWords;
};

struct parsedArgs parseArgs (char* path) {

    FILE* fileHandle = fopen(path,"r");


        if(!fileHandle) {
            printf("Couldn't open %s, shutting down.\n", path);
            exit(EXIT_FAILURE);
        }


    struct parsedArgs args;
    args.opNumber = 0;
    args.operations = (char***) malloc(sizeof(char**));
    args.opWords = (int*) malloc(sizeof(int));


    int k = 0;

    while(getc(fileHandle) != EOF) {
        fseek(fileHandle , -1 ,SEEK_CUR);

        int j = args.opNumber;
        args.opNumber += 1;
        args.operations = (char***) realloc(args.operations, args.opNumber*sizeof(char**));
        args.opWords = (int*) realloc(args.opWords, args.opNumber*sizeof(int));

        while(1) {

            char nextCharacter = getc(fileHandle);
            while(nextCharacter == ' ')
                nextCharacter = getc(fileHandle);

            if(nextCharacter == '\n' || nextCharacter == EOF)
                break;

            fseek(fileHandle , -1 ,SEEK_CUR);

            args.operations[j] = (char**) realloc(args.operations[j], (k+2)*sizeof(char*));
            args.operations[j][k] = (char*) calloc(sizeof(char), MAX_WORD_LEN);
            fscanf(fileHandle, "%s", args.operations[j][k]);
            k++;
            args.opWords[j] = k;

        }
        args.operations[j][k] = NULL;

        k = 0;

    }

/*
        //printf("%i total op number", args.opNumber);
        for(int i = 0; i < args.opNumber; i++) {
            //printf("\n%i operations\n", args.opWords[i]);
            for(int j = 0; j < args.opWords[i]; j++) {
                printf("%s ", args.operations[i][j]);
            }
            printf("\n");
        }
*/

    return args;
}

int main( int argc, char* argv[] ) {



    if(argc != 2)
        exit(EXIT_FAILURE);
    struct parsedArgs args = parseArgs(argv[1]);

    for(int i = 0; i < args.opNumber; i++) {

        pid_t pid = fork();
        int status;

        if(pid == 0) {

            if(execvp(args.operations[i][0],args.operations[i])) {
                char* fullName = args.operations[i][0];
                while (strstr(args.operations[i][0], "/") != NULL) {
                    int x = 0;
                    while(args.operations[i][0][x] != '/')
                        x++;
                    x++;
                    strncpy(args.operations[i][0], args.operations[i][0]+x, MAX_WORD_LEN - x);
            }
            /*
            printf("%i total op number", args.opNumber);
            for(int i = 0; i < args.opNumber; i++) {
                //printf("\n%i operations\n", args.opWords[i]);
                for(int j = 0; j < args.opWords[i]; j++) {
                    printf("%s ", args.operations[i][j]);
                }
                printf("\n");
            }
            */
                if(execv(fullName,args.operations[i]))
                     exit(errno);
            }

            exit(EXIT_SUCCESS);

        } else {

            waitpid(pid, &status, 0);
              if(errno == ECHILD)
                  printf("Kind of lost him\n");
            printf("Executed");
            for (int j = 0; j < args.opWords[i]; j++) {
                 printf(" %s", args.operations[i][j]);
            }
            if (status != 0)
                printf(" which FAILED with error number %i", status);
            printf("\n");
        }

    }

    errno = 0;
    while (errno != ECHILD)
       wait(NULL);


    return 0;
}
