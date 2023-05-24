#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <iostream>
#include <sys/wait.h>
#include <string.h>

#define EMPTY 0
#define FULL 1
#define MUTEX 2
#define BUF_SIZE 32
#define PRODUCTS 100

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};


int main(){
    int semid;
    int semval;
    struct sembuf semops;
    union semun semarg;
    //create a semaphore set with 3 semaphores
    if((semid = semget(IPC_PRIVATE, 3, 0666 | IPC_CREAT)) == -1){
        perror("semget");
        exit(1);
    }
    // initialize the semaphores values to 0
    semarg.val = BUF_SIZE;
    if(semctl(semid, EMPTY, SETVAL, semarg) == -1){
        perror("semctl");
        exit(1);
    }
    semarg.val = 0;
        if(semctl(semid, FULL, SETVAL, semarg) == -1){
                perror("semctl");
        exit(1);
        }
    semarg.val = 1;
        if(semctl(semid, MUTEX, SETVAL, semarg) == -1){
                perror("semctl");
        exit(1);
        }
    //creating a shared memory
    int shm_id = shmget(IPC_PRIVATE, BUF_SIZE * sizeof(int), IPC_CREAT | 0666);
    if(shm_id == -1){
        perror("Couldn't create the shm");
        exit(1);
    }
    int* shared_value = (int*) shmat(shm_id, NULL, 0);
    for(int i = 0;i < BUF_SIZE; ++i){
    *(shared_value + i) = 0;
    }
    pid_t producer, consumer;

    if(producer == 0){
        //some code for producer
        for(int i = 0;i < PRODUCTS; ++i){
            //blocking the semaphore mutex
            semops.sem_num = MUTEX;
            semops.sem_op = -1;
            semops.sem_flg = 0;
            if(semop(semid, &semops, 1) == -1){
                perror("semop");
                exit(1);
            }
            //check if there is empty slot
            semops.sem_num = EMPTY;
            semops.sem_op = -1;
            semops.sem_flg = 0;
            if(semop(semid, &semops, 1) == -1){
                perror("semop");
                exit(1);
            }
            //production
            for(int j = 0;j < BUF_SIZE; ++j){
                if(*(shared_value + j) == 0){
                    ++(*(shared_value + j));
                    break;
                }
            }
            //indicates the availability of a full slot
            semops.sem_num = FULL;
            semops.sem_op = 1;
            semops.sem_flg = 0;
            if(semop(semid, &semops, 1) == -1){
                perror("semop");
                exit(1);
            }
            //opening the mutex semaphore
            semops.sem_num = MUTEX;
            semops.sem_op = 1;
            semops.sem_flg = 0;
            if(semop(semid, &semops, 1) == -1){
                perror("semop");
                exit(1);
            }
        }
        //detaching shm
            if(shmdt(shared_value) < 0){
                    std::cout<< "ERROR: Cannot detach the shm segment!!\n";
                    exit(1);
            }

    }else if(consumer == 0){
        //some code for consumer
        for(int i = 0;i < PRODUCTS;++i){
                        //blocking the semaphore mutex
                        semops.sem_num = MUTEX;
                        semops.sem_op = -1;
                        semops.sem_flg = 0;
                        if(semop(semid, &semops, 1) == -1){
                                perror("semop");
                                exit(1);
                        }

            //check if there is full slot
                        semops.sem_num = FULL;
                        semops.sem_op = -1;
                        semops.sem_flg = 0;
                        if(semop(semid, &semops, 1) == -1){
                                perror("semop");
                                exit(1);
                        }
                        //consuming
                        for(int j = 0;j < BUF_SIZE; ++j){
                                if(*(shared_value + j) == 1){
                                        --(*(shared_value + j));
                    std::cout<<"Retrieved the product: "<<i<<std::endl;
                                        break;
                                }
                        }
                        //indicates the availability of an empty slot
                        semops.sem_num = EMPTY;
                        semops.sem_op = 1;
                        semops.sem_flg = 0;
                        if(semop(semid, &semops, 1) == -1){
                                perror("semop");
                                exit(1);
                        }
                        //opening the mutex semaphore
                        semops.sem_num = MUTEX;
                        semops.sem_op = 1;
                        semops.sem_flg = 0;
                        if(semop(semid, &semops, 1) == -1){
                                perror("semop");
                                exit(1);
                        }

        }
        //detaching shm
            if(shmdt(shared_value) < 0){
                    std::cout<< "ERROR: Cannot detach the shm segment!!\n";
                    exit(1);
            }
    }
    wait(NULL);
    wait(NULL);

    //remove the semaphore set
    if(semctl(semid, 0, IPC_RMID, semarg) == -1){
        perror("semctl");
        exit(1);
    }
    if(shmctl(shm_id, IPC_RMID, NULL) == -1){
        perror("shmctl");
        exit(1);
    }
    return 0;
}
