#include "client_server.h"

int msg_q_id = -1;

void clean() {
    if(msg_q_id != -1)
        if(msgctl(msg_q_id, IPC_RMID, NULL))
            printf("Cleaning failed\n");
}

struct client_info {
    int size;
    int actual_size;
};

struct parsed_args {
    int fifo_handle;
};


void add_client_info(struct client_info* info) {
    info -> actual_size++;
    if(info -> actual_size >= info -> size) {
         info -> size *= 2;
    }
}


struct parsed_args parse_args (char* path) {
    struct parsed_args args;
    return args;
}

void finish(int signal) {

    printf("%c%cCleaning and exiting \n", 8, 8);
    clean();
    exit(EXIT_SUCCESS);

}

int main( int argc, char* argv[] ) {


    struct sigaction grand_finale;
    grand_finale.sa_handler = finish;
    grand_finale.sa_flags = 0;
    sigaction(SIGINT, &grand_finale, NULL);

    struct passwd *pw = getpwuid(getuid());
    char* homedir = pw->pw_dir;

    key_t key = ftok(homedir, DESIRED_KEY_NUMBER);
    if(key == -1) {
        printf("Could not create key %#010x\n", key);
        exit(EXIT_FAILURE);
    }

    int q_id = msgget(key, IPC_CREAT);
    if(q_id == -1) {
        printf("Could not create quene with key %#010x\n    , trying again\n", key);
        q_id = msgget(key, IPC_CREAT);
        if(q_id == -1) {
            printf("Failed yet again with key %#010x, exiting\n", key);
            exit(EXIT_FAILURE);
        }
    }
    printf("Created queue with id %i \n", q_id);
    msg_q_id = q_id;

    struct msqid_ds* q_data_buffer = (struct msqid_ds*) malloc(sizeof(struct msqid_ds));
    msgctl(q_id, IPC_STAT, q_data_buffer);
    q_data_buffer->msg_perm.mode = 777;
    if(msgctl(q_id, IPC_SET, q_data_buffer)) {
            printf("Failed to change perms with errno: %s\n", strerror(errno));
            clean();
            exit(EXIT_FAILURE);
    }

    struct msg_int msg_int_buffor;

    while(1) {
        if(msgrcv(q_id, &msg_int_buffor, MSG_INT_SIZE, 1, 0) > 0)
             printf("%i\n", msg_int_buffor.msg);
    }

    clean();
    return 0;
}
