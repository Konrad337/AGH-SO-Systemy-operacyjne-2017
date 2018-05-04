#include "client_server.h"

void* shared_memory_adress = NULL;
int shared_memory_id = 0;

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

int main( int argc, char* argv[] ) {

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

    int m_id = shmget(key, 0, IPC_CREAT | 0660);
    shared_memory_id = m_id;
    if(m_id == -1) {
        perror(KRED "Failed with key :" RESET);
        exit(EXIT_FAILURE);
    }

    printf("Joined shared memory with id %i \n", m_id);

    int* m_pointer = (int*) shmat(m_id, NULL, SHM_RDONLY);
    shared_memory_adress = m_pointer;

    printf("%i\n", m_pointer[0]);

    clean();
    return 0;
}
