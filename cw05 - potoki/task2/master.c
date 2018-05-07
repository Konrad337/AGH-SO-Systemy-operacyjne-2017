
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_WORD_LEN 40
#define BUFFER_SIZE 1024

struct parsedArgs {
    int fifoHandle;
};

char* pathForFinisher;

struct parsedArgs parseArgs (char* path) {

    pathForFinisher = path;

    mode_t mode = 0777;
    mkfifo(path, mode);
    int fifoHandle = open(path, O_RDONLY);
    printf("%i\n", fifoHandle);


        if(!fifoHandle ) {
            printf("Couldn't create fifo at %s, shutting down.\n", path);
            exit(EXIT_FAILURE);
        }

    struct parsedArgs args;

    args.fifoHandle = fifoHandle;

    printf("%i\n", fifoHandle);

    return args;
}

void finish(int signal) {

    remove(pathForFinisher);
    printf("%c%cCleaning and exiting \n", 8, 8);
    exit(EXIT_SUCCESS);

}

int main( int argc, char* argv[] ) {


    struct sigaction grandFinale;
    grandFinale.sa_handler = finish;
    grandFinale.sa_flags = 0;
    sigaction(SIGINT, &grandFinale, NULL);



    if(argc != 2)
        exit(EXIT_FAILURE);

    struct parsedArgs args = parseArgs(argv[1]);

    char* buffer = (char*) malloc(PIPE_BUF);

    while(1) {
        if (read(args.fifoHandle, buffer, PIPE_BUF) > 0)
            printf("%s\n", buffer);
    }


    return 0;
}
