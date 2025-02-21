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

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\x1B[0m"

#define DESIRED_KEY_NUMBER 42
#define MSG_INT_SIZE sizeof(struct msg_int) - sizeof(long)
#define MSG_OP_SIZE sizeof(struct msg_operation) - sizeof(long)
#define MAX_MSG_SIZE 64

struct msg_int {
  long mtype;
  int msg;
  int pid;
};
struct msg_operation {
  long mtype;
  char op[MAX_MSG_SIZE];
  int data1;
  int data2;
  int client_id;
};

#endif
