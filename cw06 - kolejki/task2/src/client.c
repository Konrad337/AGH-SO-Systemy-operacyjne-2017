#include "client_server.h"


struct parsed_args {
    int op_number;
    char*** operations;
};

mqd_t msg_q_id = -1;
char* gq_name = NULL;

void clean() {
    if(msg_q_id != -1)
        if(mq_close(msg_q_id))
            printf("Cleaning failed\n");
    if(gq_name != NULL)
        if(mq_unlink(gq_name))
            printf("Cleaning failed\n");
}


struct parsed_args parse_args (int argc, char* argv[]) {

    struct parsed_args args;
    args.op_number = 0;
    args.operations = (char***) malloc(sizeof(char**));

    int j = 0;
    for(int i = 1; i < argc; i++) {

        args.op_number += 1;
        if (strcmp(argv[i], "CALC") == 0) {
            if (argc - i < 4) {
                printf("Need more agrs for CALC\n");
                clean();
                exit(EXIT_FAILURE);
            }
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
                if (argc - i < 2) {
                    printf("Need more agrs for MIRROR\n");
                    clean();
                    exit(EXIT_FAILURE);
                }
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



void finish(int signal) {

    printf("%c%cCleaning and exiting \n", 8, 8);
    clean();
    exit(EXIT_SUCCESS);

}

void read_msg_from_server(int q_id, char* buffer, int size) {
    while(mq_receive(q_id, buffer, size, NULL) == -1) {
        if (errno != EAGAIN) {
            printf("Failed to communicate with server with errno: %s\n", strerror(errno));
            clean();
            exit(EXIT_FAILURE);
        } else {
            printf(KWHT "Waiting for server response...\n" RESET);
            sleep(1);
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

    GET_SERVER_Q_NAME;
    mqd_t q_id = mq_open(server_q_name, O_WRONLY );
    if(q_id == -1) {
            printf("Failed with name %s, exiting\n", server_q_name);
            clean();
            exit(EXIT_FAILURE);
    }
    printf("Connected to q with id %i\n", q_id);

    srand(time(NULL));
    char * q_name = (char*) malloc(Q_NAME_MAX_SIZE*sizeof(char));
    sprintf(q_name,"/q%c%id", rand() % ('z' - 'a') + 'a', getpid() | rand());

    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_flags   = 0;
    attr.mq_curmsgs   = 0;
    mqd_t private_q_id = mq_open(q_name, O_RDONLY | O_EXCL | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &attr);

    if(private_q_id == -1) {
        printf(KRED "Client could not create q" RESET "\n");
        clean();
        exit(EXIT_FAILURE);
    }
    msg_q_id = private_q_id;
    gq_name = q_name;
    char* initial_msg = (char*) malloc(sizeof(char)*MAX_MSG_SIZE);
    initial_msg[MSG_TYPE] = 10;
    strcpy(&initial_msg[MSG], q_name);
    char* pid = (char*) malloc(sizeof(char)*10);
    sprintf(pid, "%i", getpid());
    strcpy(&initial_msg[MSG_PS_ID], pid);

    if(mq_send(q_id, initial_msg, MAX_MSG_SIZE, 0)) {
            printf("Failed to communicate with server with errno: %s\n", strerror(errno));
            clean();
            exit(EXIT_FAILURE);
    }
    char* msg_op_buffer = (char*) malloc(sizeof(char)*MAX_MSG_SIZE);
    int client_id;

    read_msg_from_server(private_q_id, msg_op_buffer, MAX_MSG_SIZE);

    client_id = strtol(&msg_op_buffer[MSG], NULL, 10);
    int server_pid = strtol(&msg_op_buffer[MSG_PS_ID], NULL, 10);
    printf(KGRN "Got client q id number: %i, from server with pid %i\nStarting sending requests..." RESET "\n", client_id, server_pid);

    for(int i = 0; i < args.op_number; i++) {
        msg_op_buffer[CLIENT_ID] = client_id;
        if(strcmp(args.operations[i][0], "MIRROR") == 0) {
             msg_op_buffer[MSG_TYPE] = 2;
             strcpy(&msg_op_buffer[MSG], args.operations[i][1]);
        }
        else if (strcmp(args.operations[i][0], "CALC") == 0) {
            if(strlen(args.operations[i][2]) >= 6 ||  strlen(args.operations[i][3]) >= 6) {
                printf(KRED "Integer to long in CALC operation, not doin that" RESET "\n");
                continue;
            }
            msg_op_buffer[MSG_TYPE] = 3;
            strcpy(&msg_op_buffer[MSG], args.operations[i][1]);
            strcpy(&msg_op_buffer[DATA1], args.operations[i][2]);
            strcpy(&msg_op_buffer[DATA2], args.operations[i][3]);

        }
        else if (strcmp(args.operations[i][0], "TIME") == 0) {
             msg_op_buffer[MSG_TYPE] = 4;
        }
        else if (strcmp(args.operations[i][0], "END") == 0) {
             msg_op_buffer[MSG_TYPE] = 5;
        }
        else {
            printf(KRED "Uknown command\n" RESET);
            continue;
        }
        printf("Sending %s \n", args.operations[i][0]);

        if(mq_send(q_id, msg_op_buffer, MAX_MSG_SIZE, 0)) {
                printf("Failed to send request %s to server with errno: %s\n", args.operations[i][0], strerror(errno));
        } else if (strcmp(args.operations[i][0], "END") != 0) {
            read_msg_from_server(private_q_id, msg_op_buffer, MAX_MSG_SIZE);
            printf(KGRN "Got response: %s\n" RESET, &msg_op_buffer[MSG]);
        }
    }

    union sigval sigrtmin_1_info;
    sigrtmin_1_info.sival_int = private_q_id;
    sigqueue(server_pid, SIGRTMIN + 1, sigrtmin_1_info);

    free(msg_op_buffer);
    for(int i = 0; i < args.op_number; i++)
        free(args.operations[i]);
    free(args.operations);


    clean();
    return 0;
}
