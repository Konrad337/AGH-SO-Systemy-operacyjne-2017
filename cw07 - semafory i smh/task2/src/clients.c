#include "client_server.h"

void clean() {
    if (shared_memory_adress != NULL)
        if( munmap(shared_memory_adress, SHARED_MEMORY_SIZE) == -1)
            perror("Could not free shared memory: ");

}

void finish(int signal) {

    printf("%c%cCleaning and exiting \n", 8, 8);
    clean();
    exit(EXIT_SUCCESS);


}

void do_nothing(int signal, siginfo_t* info, void* ucontext) {
    }


void haircut_time(int client_number) {

    struct timespec cur_time;

    semaphore_op(-1, semaphore_id_1);
    //printf("%i\n", semctl(semaphore_id, 1, GETVAL, NULL));
    semaphore_op2(0, 1, semaphore_id_2);
    clock_gettime(CLOCK_REALTIME, &cur_time);
    printf(KGRN "%-10i %li.%-20li Haircut taken, leaving" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec);
    semaphore_op(0, semaphore_id_2);
    semaphore_op(-1, semaphore_id_1);

}

int main( int argc, char* argv[] ) {

    if(argc < 3)
        exit(EXIT_FAILURE);
    int clients_n =  strtol(argv[1], NULL, 10);
    int cuts_n = strtol(argv[2], NULL, 10);
    int done = 0;
    int pid = 1;
    int client_number = 0;
    struct timespec cur_time;
    //int* all_pids = (int*) malloc(sizeof(int) * clients_n);

    struct sigaction grand_finale;
    grand_finale.sa_handler = finish;
    grand_finale.sa_flags = 0;
    sigemptyset(&grand_finale.sa_mask);
    sigaction(SIGINT, &grand_finale, NULL);

    struct sigaction haircut_times;
    haircut_times.sa_sigaction = do_nothing;
    haircut_times.sa_flags = SA_SIGINFO;
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask,SIGUSR1);
    sigdelset(&mask,SIGINT);
    sigaction(SIGUSR1, &haircut_times, NULL);

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
       perror(KRED "getcwd() error :" RESET );
       exit(EXIT_FAILURE);
    }

    int m_id = shm_open(DESIRED_KEY_NUMBER, O_RDWR, 0660);
    shared_memory_id = ftruncate(m_id, SHARED_MEMORY_SIZE);
    if(m_id == -1) {
        perror(KRED "Failed with key :" RESET);
        exit(EXIT_FAILURE);
    }

    printf("Joined shared memory with id %i \n", m_id);
    int* m_pointer = (int*) mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, m_id, 0);
    shared_memory_adress = m_pointer;

    sem_t* s_id_0 = sem_open(DESIRED_KEY_NUMBER2, O_RDWR, 0660, 0);
    sem_t* s_id_1 = sem_open(DESIRED_KEY_NUMBER3, O_RDWR, 0660, 0);
    sem_t* s_id_2 = sem_open(DESIRED_KEY_NUMBER4, O_RDWR, 0660, 0);
    sem_t* s_id_3 = sem_open(DESIRED_KEY_NUMBER5, O_RDWR, 0660, 0);
    sem_t* s_id_4 = sem_open(DESIRED_KEY_NUMBER6, O_RDWR, 0660, 0);


    semaphore_id_0 = s_id_0;
    semaphore_id_1 = s_id_1;
    semaphore_id_2 = s_id_2;
    semaphore_id_3 = s_id_3;
    semaphore_id_4 = s_id_4;

    printf("Opened semaphores\n");


    for(int i = 0; i < clients_n; i++) {
        if(pid != 0)
            pid = fork();
        else
            break;
        if(pid == 0)
            client_number = getpid();
    }


    if(pid == 0)
    while(done < cuts_n) {

        semaphore_op2(0, 1, semaphore_id_0);

        if(m_pointer[SLEEPY_OLD_MAN] == 0 ) {
            if(m_pointer[CHAIRS_TAKEN] < m_pointer[CHAIRS]) {
                int taken = m_pointer[CHAIRS_TAKEN]++;

                int i = 0;
                while(i < (int)(FIFO_END - FIFO_START) && m_pointer[FIFO_START + i] != 0)
                    i++;
                m_pointer[FIFO_START + i] = client_number;

                clock_gettime(CLOCK_REALTIME, &cur_time);
                printf(KGRN "%-10i %li.%-20li Seat number %i taken" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec, taken);

                semaphore_op(-1, semaphore_id_0);
                while(getpid() != m_pointer[NEXT]);

                m_pointer[GHOST_CHAIR]++;

                semaphore_op2(0, 1, semaphore_id_0);
                semaphore_op2(0, 1, semaphore_id_2);
                clock_gettime(CLOCK_REALTIME, &cur_time);
                printf(KBLU "%-10i %li.%-20li Invited to haircut" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec);
                m_pointer[GHOST_CHAIR]--;
                m_pointer[CHAIRS_TAKEN]--;
                m_pointer[FIFO_START + i] = 0;
                semaphore_op(-1, semaphore_id_0);
                semaphore_op(1, semaphore_id_3);
                haircut_time(client_number);

                done++;
            }
            else {
                semaphore_op(-1, semaphore_id_0);

                clock_gettime(CLOCK_REALTIME, &cur_time);
                printf(KYEL "%-10i %li.%-20li No empty seats" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec);


            }

        } else {

            m_pointer[SLEEPY_OLD_MAN] = 0;
            m_pointer[WAKER] = (int) getpid();

            semaphore_op(1, semaphore_id_3);

            clock_gettime(CLOCK_REALTIME, &cur_time);
            printf(KBLU "%-10i %li.%-20li Waking up barber %i" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec, m_pointer[SLEEPY_MAN_PID]);
            done++;


            semaphore_op(-1, semaphore_id_4);
            semaphore_op(-1, semaphore_id_0);
            semaphore_op(0, semaphore_id_3);

            semaphore_op2(0, 1, semaphore_id_2);
            clock_gettime(CLOCK_REALTIME, &cur_time);
            printf(KBLU "%-10i %li.%-20li Invited to haircut" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec);
            haircut_time(client_number);

        }



    }


    errno = 0;
    if(pid > 0)
    while (errno != ECHILD) {
        wait(NULL);
    }

    clean();
    return 0;
}
