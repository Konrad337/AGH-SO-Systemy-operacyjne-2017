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
    doubleSigTSTP.sa_handler = doNothing;
    doubleSigTSTP.sa_flags = 0;
    sigaction(SIGTSTP, &doubleSigTSTP, NULL);
    sigset_t mask;
    sigsuspend(&mask);
    signal(SIGTSTP, sigTSTP);
}




int main( int argc, char* argv[] ) {

     signal(SIGINT, sigINT);
     signal(SIGTSTP, sigTSTP);

     time_t rawtime;
     struct tm *currentTime;


     while(1) {

         time(&rawtime);
         currentTime = localtime(&rawtime);
         printf("%.2i:%.2i:%.2i\n"
               , currentTime -> tm_hour
               , currentTime -> tm_min
               , currentTime -> tm_sec);
         sleep(1);
     }
}
