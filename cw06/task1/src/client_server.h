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

#define DESIRED_KEY_NUMBER 42
#define MSG_INT_SIZE sizeof(struct msg_int) - sizeof(long)
struct msg_int {
  long mtype;
  int msg;
};

#endif
