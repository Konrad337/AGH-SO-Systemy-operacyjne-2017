
#include <stdlib.h>
#include <stdio.h>
#include "fileLib.h"
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>


struct parsedArgs {
    //char* fileName;
    int recordSize;
    int recordNumber;
    char*** operations;
    int opNumber;
};

struct parsedArgs parseArgs (int argc, char* argv[]) {
    if(argc < 3)
        exit(EXIT_FAILURE);
    struct parsedArgs args;
    args.opNumber = 0;
    args.operations = (char***) malloc(sizeof(char**));
    args.operations[0] = (char**) malloc(sizeof(char*)*3);

    //args.fileName = argv[1];
    args.recordSize = strtol(argv[1], NULL, 10);
    args.recordNumber = strtol(argv[2], NULL, 10);

    int j = 0;
    for(int i = 3; i < argc; i++) {

        args.opNumber += 1;
        if (strcmp(argv[i], "copy") == 0) {
            args.operations = (char***) realloc(args.operations, args.opNumber*sizeof(char**));
            args.operations[j] = (char**) malloc(3*sizeof(char*));
            args.operations[j][0] = argv[i];
            args.operations[j][1] = argv[i + 1];
            args.operations[j][2] = argv[i + 2];
            i += 2;

        } else {
            args.operations = (char***) realloc(args.operations, args.opNumber*sizeof(char**));
            args.operations[j] = (char**) malloc(2*sizeof(char*));

            args.operations[j][0] = argv[i];
            args.operations[j][1] = argv[i + 1];
            i += 1;
        }
        j += 1;
    }
/*
    for(int i = 0; i < args.opNumber; i++) {
        printf("%i %s %s\n",args.opNumber, args.operations[i][0], args.operations[i][1]);
    }
    printf("%i %i\n", args.recordSize, args.recordNumber);
*/


    return args;
}


int main( int argc, char* argv[] )
{


    struct parsedArgs args = parseArgs(argc, argv);


    for (int i = 0; i < args.opNumber; i++) {



            struct timeval start, uStart, sStart;
            struct timeval end, uEnd, sEnd;
            struct rusage usage;


            gettimeofday(&start, 0);
            getrusage(RUSAGE_SELF, &usage);
            uStart = usage.ru_utime;
            sStart = usage.ru_stime;

            if (strcmp(args.operations[i][0], "generate") == 0) {
                 printf("Generating file with random content to %s\n",args.operations[i][1] );
                 generateFile(args.operations[i][1], args.recordSize, args.recordNumber);
            }
            else if (strcmp(args.operations[i][0], "sort") == 0) {
                 printf("Sorting file %s\n",args.operations[i][1] );
                 sortFile(args.operations[i][1], args.recordSize, args.recordNumber);
            }
            else if (strcmp(args.operations[i][0], "copy") == 0) {
                 printf("Coping %s to %s\n",args.operations[i][1], args.operations[i][2]);
                 copyFile(args.operations[i][1], args.operations[i][2],
                          args.recordSize, args.recordNumber);
            }


            gettimeofday(&end, 0);
            getrusage(RUSAGE_SELF, &usage);
            uEnd = usage.ru_utime;
            sEnd = usage.ru_stime;

            long real = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
            long user = (uEnd.tv_sec-uStart.tv_sec)*1000000 + uEnd.tv_usec-uStart.tv_usec;
            long systemT = (sEnd.tv_sec-sStart.tv_sec)*1000000 + sEnd.tv_usec-sStart.tv_usec;

            printf(" Real time: %li milisecs\n"
                   " User time: %li milisecs\n"
                   " System time: %li milisecs\n"
                   "%s\n \n",
                   real,
                   user,
                   systemT,
                   args.operations[i][0]);

    }




}
