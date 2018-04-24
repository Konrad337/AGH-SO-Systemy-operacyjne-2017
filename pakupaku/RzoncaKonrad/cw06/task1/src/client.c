#include "client_server.h"


struct parsed_args {
    int op_number;
    char*** operations;
};

struct parsed_args parse_args (int argc, char* argv[]) {

    struct parsed_args args;
    args.op_number = 0;
    args.operations = (char***) malloc(sizeof(char**));

    int j = 0;
    for(int i = 1; i < argc; i++) {

        args.op_number += 1;
        if (strcmp(argv[i], "CALC") == 0) {
            args.operations = (char***) realloc(args.operations, args.op_number*sizeof(char**));
            args.operations[j] = (char**) malloc(4*sizeof(char*));
            args.operations[j][0] = argv[i];
            args.operations[j][1] = argv[i + 1];
            args.operations[j][2] = argv[i + 2];
            args.operations[j][3] = argv[i + 3];
            i += 3;

        } else {
            args.operations = (char***) realloc(args.operations, args.op_number*sizeof(char**));
            args.operations[j] = (char**) malloc(4*sizeof(char*));

            args.operations[j][0] = argv[i];
            if (strcmp(argv[i], "MIRROR") == 0) {
                args.operations[j][1] = argv[i + 1];
                i += 1;
            }
        }
        j += 1;
    }
    /*
    for(int i = 0; i < args.op_number; i++) {
        printf("%i %s %s\n",args.op_number, args.operations[i][0], args.operations[i][1]);
    }
    */



    return args;
}

int msg_q_id = -1;

void clean() {
    printf("Cleaning and exiting \n");
    if(msg_q_id != -1)
        if(msgctl(msg_q_id, IPC_RMID, NULL))
            printf("Cleaning failed\n");
}


void finish(int signal) {

    printf("%c%cCleaning and exiting \n", 8, 8);
    clean();
    exit(EXIT_SUCCESS);

}

void read_msg_from_server(int q_id, void* buffer, int size, long type, int flags) {
    while(msgrcv(q_id, buffer, size, type, flags | IPC_NOWAIT) == -1) {
        if (errno != ENOMSG) {
            printf("Failed to communicate with server with errno: %s\n", strerror(errno));
            clean();
            exit(EXIT_FAILURE);
        } else {
            printf(KWHT "Waiting for server response...\n" RESET);
            sleep(2);
        }
    }
}

int main( int argc, char* argv[] ) {

    struct sigaction grand_finale;
    grand_finale.sa_handler = finish;
    grand_finale.sa_flags = 0;
    sigemptyset(&grand_finale.sa_mask);
    sigaction(SIGINT, &grand_finale, NULL);

    struct parsed_args args = parse_args(argc, argv);

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

    int private_q_id = msgget(key + getpid(), IPC_CREAT | IPC_EXCL | 0660);
    if(private_q_id == -1) {
        printf("Client could not create queneu");
        exit(EXIT_FAILURE);
    }
    msg_q_id = private_q_id;
    struct msg_int initial_msg;
    initial_msg . mtype = 10;
    initial_msg . msg = key + getpid();
    initial_msg . pid = getpid();

    if(msgsnd(q_id, &initial_msg, MSG_INT_SIZE,0)) {
            printf("Failed to communicate with server with errno: %s\n", strerror(errno));
            clean();
            exit(EXIT_FAILURE);
    }
    struct msg_int msg_int_buffer;
    int client_id;

    read_msg_from_server(private_q_id, &msg_int_buffer, MSG_INT_SIZE, 1, 0);

    client_id = msg_int_buffer.msg;
    printf(KGRN "Got client q id number: %i, from server with pid %i\nStarting sending requests...\n" RESET, client_id, msg_int_buffer.pid);

    struct msg_operation* op_msg = (struct msg_operation*) malloc(sizeof(struct msg_operation));

    for(int i = 0; i < args.op_number; i++) {
        op_msg -> client_id = client_id;
        if(strcmp(args.operations[i][0], "MIRROR") == 0) {
             op_msg -> mtype = 2;
             strcpy(op_msg->op, args.operations[i][1]);
        }
        else if (strcmp(args.operations[i][0], "CALC") == 0) {
             op_msg -> mtype = 3;
             strcpy(op_msg->op, args.operations[i][1]);
             op_msg -> data1 = strtol(args.operations[i][2], NULL, 10);
             op_msg -> data2 = strtol(args.operations[i][3], NULL, 10);
        }
        else if (strcmp(args.operations[i][0], "TIME") == 0) {
             op_msg -> mtype = 4;
        }
        else if (strcmp(args.operations[i][0], "END") == 0) {
             op_msg -> mtype = 5;
        }
        else {
            printf(KRED "Uknown command\n" RESET);
            continue;
        }
        printf("Sending %s \n", args.operations[i][0]);

        if(msgsnd(q_id, op_msg, MSG_OP_SIZE, 0)) {
                printf("Failed to send request %s to server with errno: %s\n", args.operations[i][0], strerror(errno));
        } else if (strcmp(args.operations[i][0], "END") != 0) {
            read_msg_from_server(private_q_id, op_msg, MSG_OP_SIZE, 2, 0);
            printf(KGRN "Got response: %s\n" RESET, op_msg->op);
        }
    }



    free(op_msg);
    for(int i = 0; i < args.op_number; i++)
        free(args.operations[i]);
    free(args.operations);



    clean();
    return 0;
}
