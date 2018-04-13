#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>



struct parsedArgs {
    int childrenNum;
    int childrenMin;
};

int reqNum = 0;


struct parsedArgs parseArgs (int argc, char* argv[]) {
    if(argc != 3)
        exit(EXIT_FAILURE);
    struct parsedArgs args;

    args.childrenNum = strtol(argv[1], NULL, 10);
    args.childrenMin = strtol(argv[2], NULL, 10);

    //printf("%i %i \n", args.childrenNum, args.childrenMin);


    return args;
}

void sigINT(int signum) {

    errno = 0;
    while (errno != ECHILD)
       wait(NULL);
    printf("%c%cOdebrano sygnał SIGINT\n", 8, 8);
    exit(EXIT_FAILURE);
}


void doNothing(int sigN) { return; }
void recievePass(int sigN) { printf("Child with pid %i received pass\n", getpid()); }

int childFun() {



    struct sigaction waitForSig;
    waitForSig.sa_handler = doNothing;
    waitForSig.sa_flags = 0;
    sigaction(SIGUSR2, &waitForSig, NULL);

    srand(getpid());
    int sleepTime = rand() % 10;
    sleep(sleepTime);



    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask,SIGUSR2);
    sigdelset(&mask,SIGINT);
    kill(getppid(), SIGUSR1);
    sigsuspend(&mask);

    kill(getppid(), rand() % (SIGRTMAX - SIGRTMIN) + SIGRTMIN);
    return sleepTime;
}

void dealWithRequest(int sig) {
    reqNum++;
    #ifdef ADDITIONAL_INFO
        printf("Got request number %i\n", reqNum);
    #endif
}

void dealWithFurtherRequest(int signal, siginfo_t* info, void* ucontext) {


    if(fork() == 0) {
        kill(info -> si_pid, SIGUSR2);
        #ifdef ADDITIONAL_INFO
            printf("Got request number %i\n", reqNum);
            printf("Pass send to child %i\n", info -> si_pid);
        #endif
        exit(EXIT_SUCCESS);
    } //else wait(NULL);
    reqNum++;

}

void getPinged(int signal, siginfo_t* info, void* ucontext) {

     #ifdef ADDITIONAL_INFO
     printf("Got pinged from process %i with signal %i\n", info -> si_pid, signal);
     #endif

}


int main( int argc, char* argv[] ) {

    srand(time(NULL));

    signal(SIGINT, sigINT);
    signal(SIGUSR2, doNothing);
    struct sigaction gettingReq;
    gettingReq.sa_handler = dealWithRequest;
    gettingReq.sa_flags = 0;
    sigaction(SIGUSR1, &gettingReq, NULL);


    struct sigaction gettingPinged;
    for(int i = SIGRTMIN; i <= SIGRTMAX; i++) {
        gettingPinged.sa_sigaction = getPinged;
        gettingPinged.sa_flags = SA_SIGINFO;
        sigaction(i, &gettingPinged, NULL);
    }

    struct parsedArgs args = parseArgs(argc, argv);
    int pid = 1;
    int sleepTime;

    for(int i = 0; i < args.childrenNum; i++) {
        if (pid > 0) {

            pid = fork();

            #ifdef ADDITIONAL_INFO
                if (pid > 0)
                    printf("Created child process with pid: %i\n", pid);
            #endif

            if (pid == 0)
                sleepTime = childFun();
            if (pid < 0)
                exit(EXIT_FAILURE);

        }
    }

    if(pid > 0) {

        while(reqNum < args.childrenMin);

        #ifdef ADDITIONAL_INFO
        printf("Sending pass to children\n");
        #endif

        struct sigaction dealWithFurtherRequests;
        dealWithFurtherRequests.sa_sigaction = dealWithFurtherRequest;
        dealWithFurtherRequests.sa_flags = SA_SIGINFO | SA_NODEFER;
        sigemptyset (&dealWithFurtherRequests.sa_mask);
        sigaction(SIGUSR1, &dealWithFurtherRequests, NULL);
        kill(0, SIGUSR2);


}


    if(pid == 0) {
        printf("Child with pid %i ended work with sleep time %i\n", getpid(), sleepTime);
    } else {

    errno = 0;
    while (errno != ECHILD) {
        sleep(1);
        wait(NULL);
        kill(0, SIGUSR2);
    }
}

}
