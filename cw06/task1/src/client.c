#include "client_server.h"


struct parsedArgs {
    int fileHandle;
    int N;
};

int msg_q_id = -1;

void clean() {
    printf("Cleaning and exiting \n");
    if(msg_q_id != -1)
        if(msgctl(msg_q_id, IPC_RMID, NULL))
            printf("Cleaning failed\n");
}


struct parsedArgs parseArgs (char** argv) {
    struct parsedArgs args;
    return args;
}




int main( int argc, char* argv[] ) {

    struct passwd *pw = getpwuid(getuid());
    char* homedir = pw->pw_dir;

    key_t key = ftok(homedir, DESIRED_KEY_NUMBER);
    if(key == -1) {
        printf("Could not create key %#010x\n", key);
        exit(EXIT_FAILURE);
    }

    int q_id = msgget(key, 0);
    if(q_id == -1) {
            printf("Failed with key %#010x, exiting\n", key);
            exit(EXIT_FAILURE);
    }
    printf("Connected to q with id %i\n", q_id);

    int private_q_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL);
    if(private_q_id == -1) {
        printf("Client could not create quene");
        exit(EXIT_FAILURE);
    }
    msg_q_id = private_q_id;
    struct msg_int initial_msg;
    initial_msg . mtype = 1;
    initial_msg . msg = private_q_id;

    if(msgsnd(q_id, &initial_msg, MSG_INT_SIZE,0)) {
            printf("Failed to communicate with server with errno: %s\n", strerror(errno));
            clean();
            exit(EXIT_FAILURE);
    }



    clean();
    return 0;
}
