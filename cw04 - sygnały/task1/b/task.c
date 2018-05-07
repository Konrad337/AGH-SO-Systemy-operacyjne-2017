#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>


void sigINT(int signum) {
    printf("%c%cOdebrano sygnał SIGINT\n", 8, 8);
    exit(EXIT_FAILURE);
}


void doNothing(int sigN) { return; }

void sigTSTP(int signum) {
    printf("%c%cOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n", 8, 8);
    struct sigaction doubleSigTSTP;
    struct sigaction prev;
    doubleSigTSTP.sa_handler = doNothing;
    doubleSigTSTP.sa_flags = 0;
    sigaction(SIGTSTP, &doubleSigTSTP, &prev);
    sigset_t mask;
    sigsuspend(&mask);

    sigaction(SIGTSTP, &prev, NULL);

}


void countTime() {

    if(!fork()) {
        while(1) {
            execlp("date","date",NULL);
            exit(EXIT_SUCCESS);
        }
    }
}

int main( int argc, char* argv[] ) {

    signal(SIGINT, sigINT);
    struct sigaction changeChild;
    changeChild.sa_handler = sigTSTP;
    changeChild.sa_flags = 0;
    sigaction(SIGTSTP, &changeChild, NULL);



    while(1) {
        countTime();
        sleep(1);
    }
}
