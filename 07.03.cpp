#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <cstdlib>

#define SEMAPHORE_KEY 5678

using namespace std;

void performIncrement(int semid, int* sharedValue) {
    struct sembuf sem_lock = {0, -1, SEM_UNDO};
    struct sembuf sem_unlock = {0, 1, SEM_UNDO};
    for (int i = 0; i < 10000; i++) {
        semop(semid, &sem_lock, 1);
        (*sharedValue)++;
        semop(semid, &sem_unlock, 1);
    }
}

int main() {
    int semid;
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } sem_arg;
    semid = semget(SEMAPHORE_KEY, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }
    sem_arg.val = 1;
    if (semctl(semid, 0, SETVAL, sem_arg) == -1) {
        perror("semctl");
        exit(1);
    }
    pid_t pid;
    for (int i = 0; i < 2; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            int* sharedValue = (int*)shmat(semid, NULL, 0);
            if (sharedValue == (int*)(-1)) {
                perror("shmat");
                exit(1);
            }
            performIncrement(semid, sharedValue);
            shmdt(sharedValue);
            exit(0);
        }
    }
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }
    int finalValue = semctl(semid, 0, GETVAL, sem_arg);
    cout << "Final value: " << finalValue <<endl;

    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(1);
    }

    return 0;
}
