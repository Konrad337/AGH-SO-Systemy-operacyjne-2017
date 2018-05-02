#ifndef CLIENT_SERVER_H_
#define CLIENT_SERVER_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <mqueue.h>


#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\x1B[0m"

#define DESIRED_KEY_NUMBER 42
#define MAX_MSG_SIZE 64
#define Q_NAME_MAX_SIZE 255 + 64
#define GET_SERVER_Q_NAME char * server_q_name = (char*) malloc(Q_NAME_MAX_SIZE*sizeof(char)); \
                          getcwd(server_q_name, Q_NAME_MAX_SIZE); \
                          sprintf(server_q_name,"/q_SERVER"); \

#define MSG_TYPE 0
#define DATA1 4
#define DATA2 12
#define CLIENT_ID 1
#define MSG_PS_ID 18
#define MSG 28


#endif
