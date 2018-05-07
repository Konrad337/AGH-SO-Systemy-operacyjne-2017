#include "client_server.h"
#include <math.h>
#include <limits.h>

int* m_pointer;

void clean() {
    if (shared_memory_adress != NULL)
        if( munmap(shared_memory_adress, SHARED_MEMORY_SIZE) == -1)
            perror("Could not free shared memory: ");
    shm_unlink(DESIRED_KEY_NUMBER);
    sem_close(semaphore_id_0);
    sem_close(semaphore_id_1);
    sem_close(semaphore_id_2);
    sem_close(semaphore_id_3);
    sem_close(semaphore_id_4);
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


    semaphore_op(1, semaphore_id_1);


    struct timespec cur_time;
    clock_gettime(CLOCK_REALTIME, &cur_time);
    printf(KGRN "%-10i %li.%-20li Inviting client to haircut" RESET "\n", id, cur_time.tv_sec, cur_time.tv_nsec);
    m_pointer[NEXT] = id;
    semaphore_op(-1, semaphore_id_3);

    semaphore_op(0, semaphore_id_1);
    m_pointer[NEXT] = 0;
    clock_gettime(CLOCK_REALTIME, &cur_time);
    printf(KGRN "%-10i %li.%-20li Beginning haircut" RESET "\n", id, cur_time.tv_sec, cur_time.tv_nsec);
    semaphore_op(-1, semaphore_id_2);
    semaphore_op(1, semaphore_id_1);
    clock_gettime(CLOCK_REALTIME, &cur_time);
    printf(KCYN "%-10i %li.%-20li Ending haircut" RESET "\n", id, cur_time.tv_sec, cur_time.tv_nsec);
    semaphore_op(-1, semaphore_id_2);
    semaphore_op(0, semaphore_id_1);



}

void do_nothing(int signal, siginfo_t* info, void* ucontext) {


    }


int main( int argc, char* argv[] ) {

    struct timespec cur_time;

    struct sigaction grand_finale;
    grand_finale.sa_handler = finish;
    grand_finale.sa_flags = 0;
    sigaction(SIGINT, &grand_finale, NULL);

    struct sigaction sleep_time;
    sleep_time.sa_sigaction = do_nothing;
    sleep_time.sa_flags = SA_SIGINFO;
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask,SIGUSR1);
    sigdelset(&mask,SIGINT);
    sleep_time.sa_mask = mask;
    sigaction(SIGUSR1, &sleep_time, NULL);

    if(argc < 2)
        exit(EXIT_FAILURE);
    int chairs =  strtol(argv[1], NULL, 10);

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
       perror(KRED "getcwd() error :" RESET );
       exit(EXIT_FAILURE);
    }

    int m_id = shm_open(DESIRED_KEY_NUMBER, O_RDWR | O_CREAT | O_EXCL, 0660);
    shared_memory_id = ftruncate(m_id, SHARED_MEMORY_SIZE);
    if(m_id == -1 && shared_memory_id == -1) {
        perror(KRED "Failed with key :" RESET);
        exit(EXIT_FAILURE);
    }
    printf("Created shared memory with id %i \n", m_id);
    m_pointer = (int*) mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, m_id, 0);
    shared_memory_adress = m_pointer;
    for(unsigned int i = 0; i < FIFO_END; i++) {
        m_pointer[i] = 0;
    }

    sem_t* s_id_0 = sem_open(DESIRED_KEY_NUMBER2, O_RDWR | O_CREAT | O_EXCL , 0660, 0);
    sem_t* s_id_1 = sem_open(DESIRED_KEY_NUMBER3, O_RDWR | O_CREAT | O_EXCL, 0660, 0);
    sem_t* s_id_2 = sem_open(DESIRED_KEY_NUMBER4, O_RDWR | O_CREAT | O_EXCL, 0660, 0);
    sem_t* s_id_3 = sem_open(DESIRED_KEY_NUMBER5, O_RDWR | O_CREAT | O_EXCL, 0660, 0);
    sem_t* s_id_4 = sem_open(DESIRED_KEY_NUMBER6, O_RDWR | O_CREAT | O_EXCL, 0660, 0);


    semaphore_id_0 = s_id_0;
    semaphore_id_1 = s_id_1;
    semaphore_id_2 = s_id_2;
    semaphore_id_3 = s_id_3;
    semaphore_id_4 = s_id_4;

    printf("Created semaphores \n");


    unsigned int client_number;
    m_pointer[CHAIRS] = chairs;
    m_pointer[GHOST_CHAIR] = 0;
    m_pointer[SLEEPY_MAN_PID] = (int) getpid();

    while(1) {



        semaphore_op2(0, 1, semaphore_id_0);

        if(m_pointer[CHAIRS_TAKEN] == 0 && m_pointer[GHOST_CHAIR] == 0) {

            clock_gettime(CLOCK_REALTIME, &cur_time);
            printf(KBLU "%-10i %li.%-20li Getting some sleep" RESET "\n", 0 ,cur_time.tv_sec, cur_time.tv_nsec);
            m_pointer[SLEEPY_OLD_MAN] = 1;
            semaphore_op(1, semaphore_id_4);
            semaphore_op(-1, semaphore_id_0);
            semaphore_op(0, semaphore_id_4);
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
            semaphore_op(-1, semaphore_id_0);


            haircut_time(client_number);


       }



    }


    clean();
    return 0;
}
