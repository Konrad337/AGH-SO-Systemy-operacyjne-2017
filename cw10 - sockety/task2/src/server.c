#include "client_server.h"
#include <math.h>
#include <limits.h>


pthread_t server_thread;
pthread_t server_thread2;

int op_number = 0;
int socket_file_descriptor;
int socket_file_descriptor_name;
int epfd;
struct epoll_event* ev;

void clean() {
    printf("%c%cCleaning\n", 8, 8);
    pthread_cancel(server_thread);
    pthread_cancel(server_thread2);
    shutdown(socket_file_descriptor, SHUT_RDWR);
    shutdown(socket_file_descriptor_name, SHUT_RDWR);
    close(socket_file_descriptor);
    close(socket_file_descriptor_name);
    close(epfd);
}

void finish(int signal) {

    clean();
    printf("%c%cExiting \n", 8, 8);
    exit(EXIT_SUCCESS);

}

struct server_init_info {
    int port_num;
    char* socket_path;
};

struct client_info {
    int client_number;
    int requests_sent;
    char** client_name;
    int* client_fd;
    struct sockaddr** client_adress;
    socklen_t* client_adress_len;
} global_client_info;


void *run_server_name(void *args) {

    struct server_init_info* init_info = (struct server_init_info*) args;


    struct sockaddr_un server_socket;
    struct sockaddr_un sa;
    strcpy(server_socket.sun_path, init_info -> socket_path);
    server_socket.sun_family = AF_UNIX;

    printf(KBLU "Creating socket with SOCK_DGRAM" RESET "\n");
    socket_file_descriptor_name = socket(AF_UNIX, SOCK_DGRAM, 0);
    unlink(init_info -> socket_path);
    if(bind(socket_file_descriptor_name, (struct sockaddr *)&server_socket, sizeof(server_socket)) == -1) {
        perror(KRED "Couldn't bind socket" RESET );
        finish(0);
    }
    socklen_t *sa_len = (socklen_t*) malloc(sizeof(socklen_t));
    *sa_len = sizeof(sa);
    if(getsockname(socket_file_descriptor_name, (struct sockaddr *)&sa, sa_len) == -1) {
        perror(KRED "Couldn't read socket adress" RESET);
        finish(0);
    }
    printf(KBLU "Got socket with adress %s" RESET "\n", sa.sun_path);
    printf("Socket file descriptor = %d\n", socket_file_descriptor_name);

    struct message* buf = (struct message*) malloc(sizeof(struct message));
    int clientfd;
    printf("Waiting for clients\n");

    int client_name_exists = 0;



    while(1) {

        //clientfd = accept(socket_file_descriptor, (struct sockaddr *)&client_addr, &client_addr_len);
        struct sockaddr_un client_socket;
        socklen_t client_addr_len = sizeof(struct sockaddr_un);
        recvfrom(socket_file_descriptor_name, buf, sizeof(struct message), 0, (struct sockaddr *)&client_socket, &client_addr_len);
        printf("Recived msg with type: %d from socket %s\n", buf -> msg_type, client_socket.sun_path);
        if(buf -> msg_type == 0) {
            clientfd = socket_file_descriptor_name;
            client_name_exists = 0;
            for(int i = 0; i < global_client_info.client_number; i++) {
                if (strcmp(buf -> string_msg, global_client_info.client_name[i]) == 0) {
                    printf("Client with name: %s already exists with place %i\n", global_client_info.client_name[i], i);
                    client_name_exists = 1;
                    break;
                }
            }
            if(!client_name_exists) {
                strcpy(global_client_info.client_name[global_client_info.client_number], buf -> string_msg);
                global_client_info.client_fd[global_client_info.client_number] = clientfd;

                global_client_info.client_adress[global_client_info.client_number] = (struct sockaddr*) &client_socket;
                global_client_info.client_adress_len[global_client_info.client_number] = client_addr_len;

                global_client_info.client_number++;
                printf("Registering client with name: %s\n", buf -> string_msg);
                buf -> msg_type = 10;
                sendto(clientfd, buf, sizeof(struct message), 0, (struct sockaddr *)&client_socket, client_addr_len);
            } else {
                buf -> msg_type = 11;
                sendto(clientfd, buf, sizeof(struct message), 0, (struct sockaddr *)&client_socket, client_addr_len);
            }
        } else {
            printf("Operation number %i resulted with return val: %f\n", buf -> client_num, buf -> answer);
        }

    }

    return NULL;
}

void *run_server_ip(void *args) {

    struct server_init_info* init_info = (struct server_init_info*) args;


    struct sockaddr_in server_socket;
    struct sockaddr_in sa;
    server_socket.sin_addr.s_addr = INADDR_ANY;
    server_socket.sin_family = AF_INET;
    server_socket.sin_port = htons(init_info -> port_num);

    printf(KBLU "Creating socket with SOCK_DGRAM" RESET "\n");
    socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(bind(socket_file_descriptor, (struct sockaddr *)&server_socket, sizeof(server_socket)) == -1) {
        perror(KRED "Couldn't bind socket" RESET "\n");
        finish(0);
    }
    socklen_t *sa_len = (socklen_t*) malloc(sizeof(socklen_t));
    *sa_len = sizeof(sa);
    if(getsockname(socket_file_descriptor, (struct sockaddr *)&sa, sa_len) == -1) {
        perror(KRED "Couldn't read socket adress" RESET "\n");
        finish(0);
    }
    printf(KBLU "Got socket with adress %s:%d" RESET "\n", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
    printf("Socket file descriptor = %d\n", socket_file_descriptor);

    struct message* buf = (struct message*) malloc(sizeof(struct message));
    int clientfd;
    printf("Waiting for clients\n");

    int client_name_exists = 0;



    while(1) {

        //clientfd = accept(socket_file_descriptor, (struct sockaddr *)&client_addr, &client_addr_len);
        struct sockaddr_in client_socket;
        socklen_t client_addr_len = sizeof(struct sockaddr_in);
        recvfrom(socket_file_descriptor, buf, sizeof(struct message), 0, (struct sockaddr *)&client_socket, &client_addr_len);
        printf("Recived msg with type: %d from socket %s:%d\n", buf -> msg_type, inet_ntoa(client_socket.sin_addr), ntohs(client_socket.sin_port));
        if(buf -> msg_type == 0) {
            clientfd = socket_file_descriptor;
            client_name_exists = 0;
            for(int i = 0; i < global_client_info.client_number; i++) {
                if (strcmp(buf -> string_msg, global_client_info.client_name[i]) == 0) {
                    printf("Client with name: %s already exists with place %i\n", global_client_info.client_name[i], i);
                    client_name_exists = 1;
                    break;
                }
            }
            if(!client_name_exists) {
                strcpy(global_client_info.client_name[global_client_info.client_number], buf -> string_msg);
                global_client_info.client_fd[global_client_info.client_number] = clientfd;

                global_client_info.client_adress[global_client_info.client_number] = (struct sockaddr*) &client_socket;
                global_client_info.client_adress_len[global_client_info.client_number] = client_addr_len;

                global_client_info.client_number++;
                printf("Registering client with name: %s\n", buf -> string_msg);
                buf -> msg_type = 10;
                sendto(clientfd, buf, sizeof(struct message), 0, (struct sockaddr *)&client_socket, client_addr_len);
            } else {
                buf -> msg_type = 11;
                sendto(clientfd, buf, sizeof(struct message), 0, (struct sockaddr *)&client_socket, client_addr_len);
            }
        } else {
            printf("Operation number %i resulted with return val: %f\n", buf -> client_num, buf -> answer);
        }

    }

    return NULL;
}
void *run_answer_server(void *args) {
    struct message* buf = (struct message*) malloc(sizeof(struct message));
    while(1) {
        struct epoll_event event;
        int nfds = epoll_wait(epfd, &event, 1, 100);
        if(nfds < 0) {
            printf("Epoll error %i\n", epfd);
        }
        for(int i = 0; i < nfds; i++) {
            int fd = event.data.fd;

            recv(fd, buf, sizeof(struct message), MSG_PEEK);
            if(buf -> msg_type == 1)
                recv(fd, buf, sizeof(struct message), 0);
            //printf("Recived msg with type: %d\n", buf -> msg_type);
            printf("Operation number %i resulted with return val: %f\n", buf -> client_num, buf -> answer);
        }
    }
}



int main( int argc, char* argv[] ) {


    struct sigaction grand_finale;
    grand_finale.sa_handler = finish;
    grand_finale.sa_flags = 0;
    sigaction(SIGINT, &grand_finale, NULL);

    epfd = epoll_create1(0);
    if(epfd < 0)
        printf("Epoll create error \n");


    if(argc != 3) {
        printf(KRED "Bad args" RESET "\n");
        exit(EXIT_FAILURE);
    }
    struct server_init_info server_info;
    server_info.port_num = strtol(argv[2], NULL, 10);
    server_info.socket_path = argv[1];
    global_client_info.client_fd = (int*) malloc (MAX_CLIENTS * sizeof(int));
    global_client_info.client_adress = (struct sockaddr**) malloc (MAX_CLIENTS * sizeof(struct sockaddr*));
    global_client_info.client_adress_len = (socklen_t*) malloc (MAX_CLIENTS * sizeof(socklen_t));

    global_client_info.client_name = (char**) malloc (MAX_CLIENTS * sizeof(char*));
    ev = (struct epoll_event*) malloc (MAX_CLIENTS * sizeof(struct epoll_event));
    for(int i = 0; i < MAX_CLIENTS; i++) {
        global_client_info.client_name[i] = (char*) malloc (MAX_MSG_LENGTH * sizeof(char));
    }
    global_client_info.client_number = 0;
    global_client_info.requests_sent = 0;

    pthread_create(&server_thread, NULL, run_server_ip, &server_info);
    pthread_create(&server_thread2, NULL, run_server_name, &server_info);

    struct operation op;
    op.operation = (char*) malloc(sizeof(char) * MAXIMUM_INPUT_LENGTH);
    struct message* buf = (struct message*) malloc(sizeof(struct message));
    while(1) {
        scanf("%s %d %d", op.operation, &op.op1, &op.op2);
        if(global_client_info.client_number == 0) {
            printf("Can't send op to no clients\n");
            continue;
        }
        buf -> msg_type = 0;
        buf -> client_num = op_number;
        strcpy(buf -> string_msg, op.operation);
        buf -> int_msg1 = op.op1;
        buf -> int_msg2 = op.op2;
        int senttonum = buf -> client_num % global_client_info.client_number;
        sendto(global_client_info.client_fd[senttonum], buf, sizeof(struct message), 0,
            (struct sockaddr *)global_client_info.client_adress[senttonum],
            global_client_info.client_adress_len[senttonum]);
        printf("Sent req number %i to client %i\n",
            buf -> client_num,
            senttonum);
        op_number++;
    }


    pthread_join(server_thread, NULL);
    clean();
    return 0;
}
