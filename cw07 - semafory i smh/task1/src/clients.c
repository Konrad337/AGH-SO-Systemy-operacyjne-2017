#include "client_server.h"

void clean() {
        if (shared_memory_adress != NULL)
            if(shmdt(shared_memory_adress) == -1)
                perror("Could not free shared memory: ");
}

void finish(int signal) {

    printf("%c%cCleaning and exiting \n", 8, 8);
    clean();
    exit(EXIT_SUCCESS);


}

void haircut_time(int client_number) {

    struct timespec cur_time;


    semaphore_op(-1, 1);
    //printf("%i\n", semctl(semaphore_id, 1, GETVAL, NULL));
    semaphore_op2(0, 1, 2);
    clock_gettime(CLOCK_REALTIME, &cur_time);
    printf(KGRN "%-10i %li.%-20li Haircut taken, leaving" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec);
    semaphore_op(0, 2);
    semaphore_op(-1, 1);

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

    int m_id = shmget(key, 0, 0660);
    shared_memory_id = m_id;
    if(m_id == -1) {
        perror(KRED "Failed with key :" RESET);
        exit(EXIT_FAILURE);
    }

    printf("Joined shared memory with id %i \n", m_id);
    int* m_pointer = (int*) shmat(m_id, NULL, 0);
    shared_memory_adress = m_pointer;

    key = ftok(cwd, DESIRED_KEY_NUMBER2);
    int s_id = semget(key, 0, 0660);


    semaphore_id = s_id;
    if(s_id == -1) {
        perror(KRED "Failed with key :" RESET);
        exit(EXIT_FAILURE);
    }
    printf("Opened semaphore with id %i \n", s_id);


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

        semaphore_op2(0, 1, 0);

        if(m_pointer[SLEEPY_OLD_MAN] == 0 ) {
            if(m_pointer[CHAIRS_TAKEN] < m_pointer[CHAIRS]) {
                int taken = m_pointer[CHAIRS_TAKEN]++;

                int i = 0;
                int qfix = 1;
                while(i < (int)(FIFO_END - FIFO_START) && m_pointer[FIFO_START + i] != 0 || qfix < m_pointer[CHAIRS_TAKEN]) {

                    if(m_pointer[FIFO_START + i] > 0)
                        qfix++;
                    i++;
                    }
                //printf("%i %i %i\n", i, qfix, m_pointer[CHAIRS_TAKEN]);
                m_pointer[FIFO_START + i] = client_number;

                clock_gettime(CLOCK_REALTIME, &cur_time);
                printf(KYEL "%-10i %li.%-20li Seat number %i taken" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec, taken);

                semaphore_op(-1, 0);
                while(getpid() != m_pointer[NEXT]);

                m_pointer[GHOST_CHAIR]++;

                semaphore_op2(0, 1, 0);
                semaphore_op2(0, 1, 2);
                clock_gettime(CLOCK_REALTIME, &cur_time);
                printf(KBLU "%-10i %li.%-20li Invited to haircut" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec);
                m_pointer[GHOST_CHAIR]--;
                m_pointer[CHAIRS_TAKEN]--;
                m_pointer[FIFO_START + i] = 0;
                semaphore_op(-1, 0);
                semaphore_op(1, 3);
                haircut_time(client_number);

                done++;
            }
            else {


                clock_gettime(CLOCK_REALTIME, &cur_time);
                printf(KYEL "%-10i %li.%-20li No empty seats" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec);

                semaphore_op(-1, 0);
            }

        } else {

            m_pointer[SLEEPY_OLD_MAN] = 0;
            m_pointer[WAKER] = getpid();

            semaphore_op(1, 3);

            clock_gettime(CLOCK_REALTIME, &cur_time);
            printf(KBLU "%-10i %li.%-20li Waking up barber %i" RESET "\n", client_number, cur_time.tv_sec, cur_time.tv_nsec, m_pointer[SLEEPY_MAN_PID]);
            done++;


            semaphore_op(-1, 4);
            semaphore_op(-1, 0);
            semaphore_op(0, 3);

            semaphore_op2(0, 1, 2);
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
