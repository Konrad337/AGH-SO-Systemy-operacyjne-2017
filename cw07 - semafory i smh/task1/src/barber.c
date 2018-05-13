#include "client_server.h"
#include <math.h>
#include <limits.h>

int* m_pointer;

void clean() {
    if (shared_memory_adress != NULL)
        if(shmdt(shared_memory_adress) == -1)
            perror("Could not free shared memory: ");
    shmctl(shared_memory_id, IPC_RMID, NULL);
    semctl(semaphore_id, 1, IPC_RMID, NULL);
}

struct client_info {
    int* pid;
    int* q_id;
    int* key;
    int size;
    int actual_size;
};

void finish(int signal) {

    printf("%c%cCleaning and exiting \n", 8, 8);
    clean();
    exit(EXIT_SUCCESS);

}

void haircut_time(int id) {


    semaphore_op(1, 1);


    struct timespec cur_time;
    clock_gettime(CLOCK_REALTIME, &cur_time);
    printf(KGRN "%-10i %li.%-20li Inviting client to haircut" RESET "\n", id, cur_time.tv_sec, cur_time.tv_nsec);
    m_pointer[NEXT] = id;
    semaphore_op(-1, 3);

    semaphore_op(0, 1);
    m_pointer[NEXT] = 0;
    clock_gettime(CLOCK_REALTIME, &cur_time);
    printf(KGRN "%-10i %li.%-20li Beginning haircut" RESET "\n", id, cur_time.tv_sec, cur_time.tv_nsec);
    semaphore_op(-1, 2);
    semaphore_op(1, 1);
    clock_gettime(CLOCK_REALTIME, &cur_time);
    printf(KCYN "%-10i %li.%-20li Ending haircut" RESET "\n", id, cur_time.tv_sec, cur_time.tv_nsec);
    semaphore_op(-1, 2);
    semaphore_op(0, 1);



}

int main( int argc, char* argv[] ) {

    struct timespec cur_time;

    struct sigaction grand_finale;
    grand_finale.sa_handler = finish;
    grand_finale.sa_flags = 0;
    sigaction(SIGINT, &grand_finale, NULL);

    if(argc < 2)
        exit(EXIT_FAILURE);
    int chairs =  strtol(argv[1], NULL, 10);

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
       perror(KRED "getcwd() error :" RESET );
       exit(EXIT_FAILURE);
    }
    key_t key = ftok(cwd, DESIRED_KEY_NUMBER);
    if(key == -1) {
        perror(KRED "Could not create key :" RESET);
        exit(EXIT_FAILURE);
    }

    int m_id = shmget(key, SHARED_MEMORY_SIZE, IPC_CREAT | 0660);
    shared_memory_id = m_id;
    if(m_id == -1) {
        perror(KRED "Failed with key :" RESET);
        exit(EXIT_FAILURE);
    }
    printf("Created shared memory with id %i \n", m_id);
    m_pointer = (int*) shmat(m_id, NULL, 0);
    shared_memory_adress = m_pointer;
    for(unsigned int i = 0; i < FIFO_END; i++) {
        m_pointer[i] = 0;
    }

    key = ftok(cwd, DESIRED_KEY_NUMBER2);
    int s_id = semget(key, 5, IPC_EXCL | IPC_CREAT | 0666);
    union semun arg;
    arg.val = 0;
    semctl(s_id, 0, SETVAL, arg.val);
    semctl(s_id, 1, SETVAL, arg.val);
    semctl(s_id, 2, SETVAL, arg.val);
    semctl(s_id, 3, SETVAL, arg.val);
    semctl(s_id, 4, SETVAL, arg.val);

    semaphore_id = s_id;
    if(s_id == -1) {
        perror(KRED "Failed with key :" RESET);
        exit(EXIT_FAILURE);
    }
    printf("Created semaphore with id %i \n", s_id);


    unsigned int client_number;
    m_pointer[CHAIRS] = chairs;
    m_pointer[GHOST_CHAIR] = 0;
    m_pointer[SLEEPY_MAN_PID] = getpid();

    while(1) {



        semaphore_op2(0, 1, 0);

        if(m_pointer[CHAIRS_TAKEN] == 0 && m_pointer[GHOST_CHAIR] == 0) {

            clock_gettime(CLOCK_REALTIME, &cur_time);
            printf(KBLU "%-10i %li.%-20li Getting some sleep" RESET "\n", 0 ,cur_time.tv_sec, cur_time.tv_nsec);
            m_pointer[SLEEPY_OLD_MAN] = 1;
            semaphore_op(1, 4);
            semaphore_op(-1, 0);
            semaphore_op(0, 4);
            client_number = m_pointer[WAKER];
            clock_gettime(CLOCK_REALTIME, &cur_time);
            printf(KBLU "%-10i %li.%-20li Waken up" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec);

            haircut_time(client_number);
        }
        else {
            int i = 0;
            while(i < (int) (FIFO_END - FIFO_START) && m_pointer[FIFO_START + i] == 0)
                i++;
            //printf("%i %i\n", i,  m_pointer[FIFO_START + i]);
            client_number = m_pointer[FIFO_START + i];
            semaphore_op(-1, 0);


            haircut_time(client_number);


       }



    }


    clean();
    return 0;
}
