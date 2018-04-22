#include "client_server.h"
#include <math.h>
#include <limits.h>


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

struct client_info {
    int* pid;
    mqd_t* q_id;
    char** name;
    int size;
    int actual_size;
};

void add_client_info(struct client_info* info, int pid, char *name) {
    info -> actual_size++;
    if(info -> actual_size >= info -> size) {
         info -> size *= 2;
         info -> pid = (int*) realloc(info -> pid, sizeof(int) * info -> size);
         info -> name = (char**) realloc(info -> name, sizeof(char*) * info -> size);
         info -> q_id = (int*) realloc(info -> q_id, sizeof(int) * info -> size);
    }
    info -> pid[info -> actual_size-1] = pid;
    info -> name[info -> actual_size-1] = name;

    info -> q_id[info -> actual_size-1] =  mq_open(name, O_WRONLY);
    if(info -> q_id[info -> actual_size-1] == -1) {
            printf(KRED "Failed to connect to q with name : %s\n" RESET, name);
    }
    else {
        char* msg = (char*) malloc(sizeof(char)*MAX_MSG_SIZE);;
        msg[0] = 1;
        msg[MSG] = info -> actual_size-1;
        msg[MSG_PS_ID] = getpid();
        if(mq_send(info -> q_id[info -> actual_size-1], msg, MAX_MSG_SIZE, 0)) {
                printf("Failed to communicate with client with errno: %s\n", strerror(errno));
                clean();
                exit(EXIT_FAILURE);
        }
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

    GET_SERVER_Q_NAME;
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_flags   = 0;
    attr.mq_curmsgs   = 0;
    mqd_t q_id = mq_open(server_q_name, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &attr);


    if(q_id == -1) {
        printf(KRED "Could not create quene with name %s\n, exiting\n" RESET, server_q_name);
        exit(EXIT_FAILURE);
    }

    printf("Created queue with id %i \n", q_id);
    msg_q_id = q_id;
    gq_name = server_q_name;

    char* msg_op_buffer = (char*) malloc(sizeof(char)*MAX_MSG_SIZE);
    struct client_info info;
    info.size = 10;
    info.actual_size = 0;
    info.pid = (int*) malloc(sizeof(int)*10);
    info.name = (char**) malloc(sizeof(char*)*10);
    info.q_id = (int*) malloc(sizeof(int)*10);

    int ended = 0;
    int empty = 0;
    while(!ended || !empty) {
        empty = 0;
        if(mq_receive(q_id, msg_op_buffer, MAX_MSG_SIZE, NULL) > 0) {
             if(msg_op_buffer[MSG_TYPE] == 10) {
             printf("Got q name %s from pid %i \n", &msg_op_buffer[MSG], msg_op_buffer[MSG_PS_ID]);
             add_client_info(&info, msg_op_buffer[MSG_PS_ID], &msg_op_buffer[MSG]);
         }
         else {
                printf("Got operetion request from client with id %i\n", msg_op_buffer[1]);
                 if(msg_op_buffer[MSG_TYPE] == 2) {
                     printf("Doing MIRROR\n");
                      strcpy(&msg_op_buffer[MSG], strrev(&msg_op_buffer[MSG]));
                 }
                 else if(msg_op_buffer[MSG_TYPE] == 3) {
                     printf("Doing CALC\n");
                     float answer;
                     if(strcmp(&msg_op_buffer[MSG], "ADD") == 0)
                         /*answer = (float)&msg_op_buffer[DATA1] + (float)&msg_op_buffer[DATA2];
                     else if(strcmp(&msg_op_buffer[MSG], "DIV") == 0)
                          //answer = (float)&msg_op_buffer[DATA1] / (float)&msg_op_buffer[DATA2];
                     else if(strcmp(&msg_op_buffer[MSG], "MUL") == 0)
                          //answer = (float)&msg_op_buffer[DATA1] * (float)&msg_op_buffer[DATA2];
                     else if(strcmp(&msg_op_buffer[MSG], "SUB") == 0)
                          //answer = (float)&msg_op_buffer[DATA1] - (float)&msg_op_buffer[DATA2];
                     else {*/
                         answer = 0;


                     char* str = (char*) malloc(MAX_MSG_SIZE*sizeof(char));
                     sprintf(str,"%f",answer);
                     strcpy(&msg_op_buffer[MSG], str);

                 }
                 else if(msg_op_buffer[MSG_TYPE] == 4) {
                     printf("Doing DATE\n");
                     FILE* dateFile;
                     char* buffer = (char*) malloc(PIPE_BUF);
                     dateFile = popen("date", "r");
                     fread(buffer, 1, PIPE_BUF, dateFile);
                     strcpy(&msg_op_buffer[MSG], buffer);
                 }
                 else if(msg_op_buffer[MSG_TYPE] == 5) {
                     printf("Doing END\n");
                      ended = 1;
                      continue;
                 }
                 msg_op_buffer[MSG_TYPE] = 2;
                 int client_id = msg_op_buffer[CLIENT_ID];
                 if(mq_send(info.q_id[client_id], msg_op_buffer, MAX_MSG_SIZE, 0)) {
                         printf("Failed to communicate with client with errno: %s\n", strerror(errno));
                         clean();
                         exit(EXIT_FAILURE);
                 } else

                 printf(KGRN "Send back op output to client with id %i\n" RESET, (int)msg_op_buffer[CLIENT_ID]);
             }
         }
             else
                 empty = 1;



    }

    free(server_q_name);
    clean();
    return 0;
}
