#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>



struct parsedArgs {
    int L;
    int Type;
};

int pings = 0;

struct parsedArgs parseArgs (int argc, char* argv[]) {
    if(argc != 3)
        exit(EXIT_FAILURE);
    struct parsedArgs args;

    args.L = strtol(argv[1], NULL, 10);
    args.Type = strtol(argv[2], NULL, 10);

    //printf("%i %i \n", args.childrenNum, args.childrenMin);


    return args;
}

void doNothing(int sigN) { return; }


void sigINT(int signum) {


    struct sigaction finish;
    finish.sa_handler = doNothing;
    finish.sa_flags = 0;
    sigemptyset (&finish.sa_mask);
    sigaction(SIGUSR2, &finish, NULL);
    kill(0, SIGUSR2);

    printf("%c%cRecieved SIGINT - sent kill signal to child\n", 8, 8);



    errno = 0;
    while (errno != ECHILD)
       wait(NULL);

    exit(EXIT_FAILURE);
}




void dealWithPing(int signal, siginfo_t* info, void* ucontext) {

    pings++;
    #ifdef ADDITIONAL_INFO
    printf("Child got pinged for %i time\n", pings);
    #endif
    sleep(1);
    kill(info -> si_pid, signal);

}


void dealWithPingParent(int signal, siginfo_t* info, void* ucontext) {

    pings++;
    #ifdef ADDITIONAL_INFO
    printf("Got pinged from child, pinging it back\n");
    #endif
    kill(info -> si_pid, signal);

}


void printPingInfo(int signal, siginfo_t* info, void* ucontext) {

    #ifdef ADDITIONAL_INFO
    printf("Got pinged from child\n");
    #endif

}

void exitChild(int sig) {
    printf("Child recieved exit signal ENDING CHILD PROCESS\n");
    exit(EXIT_SUCCESS);
}

int main( int argc, char* argv[] ) {





    struct parsedArgs args = parseArgs(argc, argv);

    struct sigaction waitForChildSetUp;
    struct sigaction prev;
    waitForChildSetUp.sa_handler = doNothing;
    waitForChildSetUp.sa_flags = 0;
    sigaction(SIGUSR1, &waitForChildSetUp, &prev);
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask,SIGUSR1);


    int pid = 0;
    pid = fork();

    //printf("%i\n", pid);



    if(pid == 0) {

        struct sigaction getSIGINT;
        getSIGINT.sa_handler = doNothing;
        getSIGINT.sa_flags = 0;
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask,SIGUSR2);
        sigaction(SIGINT, &getSIGINT, NULL);



        struct sigaction pinged;
        pinged.sa_sigaction = dealWithPing;
        pinged.sa_flags = SA_SIGINFO ;
        sigemptyset (&pinged.sa_mask);
        sigaction(SIGUSR1, &pinged, NULL);
        if(args.Type == 3)
            sigaction(SIGRTMIN + 1, &pinged, NULL);


        struct sigaction finish;
        finish.sa_handler = exitChild;
        finish.sa_flags = 0;
        sigemptyset (&finish.sa_mask);
        sigaction(SIGUSR2, &finish, NULL);
        if(args.Type == 3)
            sigaction(SIGRTMIN + 2, &finish, NULL);


        kill(getppid(), SIGUSR1);

        while(pings < args.L);

    } else
    if(pid > 0) {

        signal(SIGINT, sigINT);

        sigsuspend(&mask);
        sigaction(SIGUSR1, &prev, NULL);


        if(args.Type == 1) {


            struct sigaction backPing;
            backPing.sa_sigaction = printPingInfo;
            backPing.sa_flags = SA_SIGINFO | SA_NODEFER;
            sigaction(SIGUSR1, &backPing, NULL);

            for(int i = 0; i < args.L; i++) {
                kill(pid, SIGUSR1);
                printf("Sent SIGUSR1 to child\n");
                sleep(1);
            }


            kill(pid, SIGUSR2);
            printf("Sent SIGUSR2 to child\n");

        } else
        if(args.Type == 2) {

            struct sigaction backPing;
            backPing.sa_sigaction = dealWithPingParent;
            backPing.sa_flags = SA_SIGINFO | SA_NODEFER;
            sigaction(SIGUSR1, &backPing, NULL);
            kill(pid, SIGUSR1);
            printf("Sent SIGUSR1 to child\n");
            while(pings < args.L);
            kill(pid, SIGUSR2);
            printf("Sent SIGUSR2 to child\n");

        } else
        if(args.Type == 3) {

            struct sigaction backPing;
            backPing.sa_sigaction = printPingInfo;
            backPing.sa_flags = SA_SIGINFO | SA_NODEFER;
            sigaction(SIGRTMIN + 1, &backPing, NULL);

            for(int i = 0; i < args.L; i++) {
                kill(pid, SIGRTMIN + 1);
                printf("Sent SIGRTMIN + 1 to child\n");
                sleep(1);

            }

            kill(pid, SIGRTMIN + 2);
            printf("Sent SIGRTMIN + 2 to child\n");

        }
    } else printf("No child created");


    errno = 0;
    while (errno != ECHILD) {
        wait(NULL);
    }
}
