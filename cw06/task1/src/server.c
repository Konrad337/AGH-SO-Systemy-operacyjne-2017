#include "client_server.h"
#include <math.h>
#include <limits.h>


int msg_q_id = -1;

void clean() {
    if(msg_q_id != -1)
        if(msgctl(msg_q_id, IPC_RMID, NULL))
            printf("Cleaning failed\n");
}

struct client_info {
    int* pid;
    int* q_id;
    int* key;
    int size;
    int actual_size;
};

struct parsed_args {
    int fifo_handle;
};


void add_client_info(struct client_info* info, int pid, int key) {
    info -> actual_size++;
    if(info -> actual_size >= info -> size) {
         info -> size *= 2;
         info -> pid = (int*) realloc(info -> pid, sizeof(int) * info -> size);
         info -> key = (int*) realloc(info -> key, sizeof(int) * info -> size);
         info -> q_id = (int*) realloc(info -> q_id, sizeof(int) * info -> size);
    }
    info -> pid[info -> actual_size-1] = pid;
    info -> key[info -> actual_size-1] = key;

    info -> q_id[info -> actual_size-1] = msgget(key, 0);
    if(info -> q_id[info -> actual_size-1] == -1) {
            printf("Failed to connect to q with key : %#010x\n", key);
    }
    else {
        struct msg_int msg;
        msg . mtype = 1;
        msg . msg = info -> actual_size-1;
        msg . pid = getpid();
        msgsnd(info -> q_id[info -> actual_size-1], &msg, MSG_INT_SIZE,0);
        printf("Connected to q with id: %i\n", info -> q_id[info -> actual_size-1]);
}
}

void change_pemissions(int q_id, int perms) {

    struct msqid_ds* q_data_buffer = (struct msqid_ds*) malloc(sizeof(struct msqid_ds));
    msgctl(q_id, IPC_STAT, q_data_buffer);
    q_data_buffer->msg_perm.mode = perms;
    if(msgctl(q_id, IPC_SET, q_data_buffer)) {
            printf("Failed to change perms with errno: %s\n", strerror(errno));
            //clean();
            //exit(EXIT_FAILURE);
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


char *strrev(char *str)
{
    int i = strlen(str) - 1, j = 0;

    char ch;
    while (i > j)
    {
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
        i--;
        j++;
    }
    return str;
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

    int q_id = msgget(key, IPC_CREAT | 0660);
    if(q_id == -1) {
        printf("Could not create quene with key %#010x\n    , trying again\n", key);
        q_id = msgget(key, IPC_CREAT | 0660);
        if(q_id == -1) {
            printf("Failed yet again with key %#010x, exiting\n", key);
            exit(EXIT_FAILURE);
        }
    }
    printf("Created queue with id %i \n", q_id);
    msg_q_id = q_id;

    struct msg_int msg_int_buffer;
    struct msg_operation msg_op_buffer;
    struct client_info info;
    info.size = 10;
    info.actual_size = 0;
    info.pid = (int*) malloc(sizeof(int)*10);
    info.key = (int*) malloc(sizeof(int)*10);
    info.q_id = (int*) malloc(sizeof(int)*10);

    int ended = 0;
    int empty = 0;
    while(!ended || !empty) {
        empty = 0;
        if(msgrcv(q_id, &msg_int_buffer, MSG_INT_SIZE, 10, IPC_NOWAIT) > 0) {
             printf("Got key %#010x from pid %i \n", msg_int_buffer.msg, msg_int_buffer.pid);
             add_client_info(&info,  msg_int_buffer.pid, msg_int_buffer.msg);
        }
        else {
            if(msgrcv(q_id, &msg_op_buffer, MSG_OP_SIZE, -6, IPC_NOWAIT) > 0 ) {
                printf("Got operetion request from client with id %i\n", msg_op_buffer.client_id);
                 if(msg_op_buffer.mtype == 2) {
                     printf("Doing MIRROR\n");
                      strcpy(msg_op_buffer.op, strrev(msg_op_buffer.op));
                 }
                 else if(msg_op_buffer.mtype == 3) {
                     printf("Doing CALC\n");
                     int answer;
                     if(strcmp(msg_op_buffer.op, "ADD") == 0)
                         answer = msg_op_buffer.data1 + msg_op_buffer.data2;
                     else if(strcmp(msg_op_buffer.op, "DIV") == 0)
                          answer = msg_op_buffer.data1 / msg_op_buffer.data2;
                     else if(strcmp(msg_op_buffer.op, "MUL") == 0)
                          answer = msg_op_buffer.data1 * msg_op_buffer.data2;
                     else if(strcmp(msg_op_buffer.op, "SUB") == 0)
                          answer = msg_op_buffer.data1 - msg_op_buffer.data2;
                     else {
                         answer = 0;
                     }

                     char* str = (char*) malloc(10*sizeof(char));
                     sprintf(str,"%i",answer);
                      strcpy(msg_op_buffer.op, str);

                 }
                 else if(msg_op_buffer.mtype == 4) {
                     printf("Doing DATE\n");
                     FILE* dateFile;
                     char* buffer = (char*) malloc(PIPE_BUF);
                     dateFile = popen("date", "r");
                     fread(buffer, 1, PIPE_BUF, dateFile);
                     strcpy(msg_op_buffer.op, buffer);
                 }
                 else if(msg_op_buffer.mtype == 5) {
                     printf("Doing END\n");
                      ended = 1;
                      continue;
                 }
                 msg_op_buffer.mtype = 2;

                 msgsnd(info.q_id[msg_op_buffer.client_id], &msg_op_buffer, MSG_OP_SIZE,0);
                 printf("Send back op output to client with id %i\n", msg_op_buffer.client_id);
             }
             else
                 empty = 1;
        }


    }


    clean();
    return 0;
}
