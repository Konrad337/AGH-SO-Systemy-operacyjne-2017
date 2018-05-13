
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

#define MAX_WORD_LEN 40

struct parsedArgs {
    char*** operations;
    int opNumber;
    int* opWords;
};

void setLimits(char* memLimit, char* timeLimit) {

    long memLim = strtol(memLimit, NULL, 10)* 1048576;
    long timeLim = strtol(timeLimit, NULL, 10);

    struct rlimit cpuTimeLimit;
    struct rlimit memoryLimit;

  cpuTimeLimit.rlim_cur = (rlim_t) timeLim;
  cpuTimeLimit.rlim_max = (rlim_t) timeLim;

  memoryLimit.rlim_cur = (rlim_t) memLim;
  memoryLimit.rlim_max = (rlim_t) memLim;

  setrlimit(RLIMIT_CPU, &cpuTimeLimit);
  setrlimit(RLIMIT_AS, &memoryLimit);

}
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



    if(argc != 4)
        exit(EXIT_FAILURE);
    struct parsedArgs args = parseArgs(argv[1]);
    char* timeLimit = argv[2];
    char* memLimit =  argv[3];

    struct timeval start, uStart, sStart;
    struct timeval end, uEnd, sEnd;
    struct rusage usage;



    for(int i = 0; i < args.opNumber; i++) {

        gettimeofday(&start, 0);
        getrusage(RUSAGE_CHILDREN, &usage);
        uStart = usage.ru_utime;
        sStart = usage.ru_stime;


        pid_t pid = fork();
        int status;

        if(pid == 0) {

            setLimits(memLimit,timeLimit);

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
            
            gettimeofday(&end, 0);
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


        getrusage(RUSAGE_CHILDREN, &usage);
        uEnd = usage.ru_utime;
        sEnd = usage.ru_stime;

        long real = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
        long user = (uEnd.tv_sec-uStart.tv_sec)*1000000 + uEnd.tv_usec-uStart.tv_usec;
        long systemT = (sEnd.tv_sec-sStart.tv_sec)*1000000 + sEnd.tv_usec-sStart.tv_usec;

        printf("\n Real time: %li milisecs\n"
               " User time: %li milisecs\n"
               " System time: %li milisecs\n"
               "%s\n \n",
               real,
               user,
               systemT,
               args.operations[i][0]);

    }

    errno = 0;
    while (errno != ECHILD)
       wait(NULL);


    return 0;
}
