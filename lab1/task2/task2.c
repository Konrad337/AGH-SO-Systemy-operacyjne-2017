
#include <stdio.h>
#include <stdlib.h>
#include "../task1/carray.h"
#include "data.h"
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>


struct parsedArgs {
    int arraySize;
    int blockSize;
    int isStatic;
    char*** operations;
    int opNumber;
};

struct parsedArgs parseArgs (int argc, char* argv[]) {
    if(argc < 6)
        exit(EXIT_FAILURE);
    struct parsedArgs args;
    args.opNumber = 0;
    args.operations = (char***) malloc(sizeof(char**));
    args.operations[0] = (char**) malloc(sizeof(char*)*3);

    args.arraySize = strtol(argv[1], NULL, 10);
    args.blockSize = strtol(argv[2], NULL, 10);
    if (strcmp(argv[3], "dynamic") == 0) {
        args.isStatic = 0;
    } else if (strcmp(argv[3], "static") == 0) {
        args.isStatic = 1;
    } else {
        printf("Wrong type of allocation!\n");
        exit(EXIT_FAILURE);
    }
    int j = 0;
    for(int i = 4; i < argc; i++) {

        args.opNumber += 1;
        if (strcmp(argv[i], "create_table") == 0) {
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
    printf("%i %i\n", args.arraySize, args.blockSize);

    */

    return args;
}



void searchElement(struct array mainArray, int value) {

}

void removeBlocks(struct array mainArray, int number) {
    for (int i = 0; i < number; i++) {
        removeCharBlock(mainArray);
    }
}

void add(struct array mainArray, int number) {
    int j = 0;
    for (int i = 0; i < number; i++) {
        addCharBlock(mainArray ,data[j]);
        j += 1;
        if (j >= 1000) j = 0;
    }
}

void createTable(int sizeA, int sizeB) {
    struct array mainArray = createArrayOfStringPtrs(sizeA, sizeB);
    add(mainArray, sizeA);
}



int main( int argc, char* argv[] )
{
    #ifdef DYNAMIC
        lib = dlopen("./libs/libcArrayShared.so", RTLD_LAZY);
        //void (*print)(Block*) = dlsym(lib, "print")
        //void (*delete_all)(Block*) = dlsym(lib, "delete_all");
    #endif



    struct parsedArgs args = parseArgs(argc, argv);
    struct array mainArray = createArrayOfStringPtrs(args.arraySize, args.blockSize);

    for (int i = 0; i < args.opNumber; i++) {



            struct timeval start, uStart, sStart;
            struct timeval end, uEnd, sEnd;
            struct rusage usage;


            gettimeofday(&start, 0);
            getrusage(RUSAGE_SELF, &usage);
            uStart = usage.ru_utime;
            sStart = usage.ru_stime;

            if (strcmp(args.operations[i][0], "create_table") == 0)
                createTable(strtol(args.operations[i][1], NULL, 10), strtol(args.operations[i][2], NULL, 10));
            if (strcmp(args.operations[i][0], "search_element") == 0)
                searchElement (mainArray, strtol(args.operations[i][1], NULL, 10));
            if (strcmp(args.operations[i][0], "removeBlocks") == 0)
                removeBlocks(mainArray, strtol(args.operations[i][1], NULL, 10));
            if (strcmp(args.operations[i][0], "add") == 0)
                add(mainArray, strtol(args.operations[i][1], NULL, 10));
            if (strcmp(args.operations[i][0], "removeBlocks_and_add") == 0) {
                removeBlocks(mainArray, strtol(args.operations[i][1], NULL, 10));
                add(mainArray, strtol(args.operations[i][1], NULL, 10));
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
