
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
#include <fcntl.h>

#define MAX_WORD_LEN 40
#define BUFFER_SIZE 1024

struct parsedArgs {
    int fileHandle;
    int N;
};

struct parsedArgs parseArgs (char** argv) {

    int fileHandle = open(argv[1], O_WRONLY);



        if(!fileHandle) {
            printf("Couldn't open %s, shutting down.\n", argv[1]);
            exit(EXIT_FAILURE);
        }

    struct parsedArgs args;

    args.fileHandle = fileHandle;
    args.N = strtol(argv[2], NULL, 10);

    return args;
}


int main( int argc, char* argv[] ) {

    if(argc != 3)
        exit(EXIT_FAILURE);
    struct parsedArgs args = parseArgs(argv);

    int pid = getpid();
    char* pidString = (char*) malloc(PIPE_BUF);
    sprintf(pidString, "%d", pid);
    printf("%s\n", pidString);


    char* buffer = (char*) malloc(PIPE_BUF);
    FILE* dateFile;
    srand(time(NULL));

    for(int i = 0; i < args.N; i++) {
        dateFile = popen("date", "r");
        fread(buffer, 1, PIPE_BUF, dateFile);
        write(args.fileHandle, pidString, PIPE_BUF);
        write(args.fileHandle, buffer, PIPE_BUF);
        pclose(dateFile);
        sleep(rand()%5);
    }



    return 0;
}
