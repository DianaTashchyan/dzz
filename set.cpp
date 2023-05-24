#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <string>

#define TABLE_SIZE 300

int main(){
        std::string pathname = "shm_file_2";
        key_t key = ftok(pathname.c_str(), 'a');
        if(key < 0){
                std::cout<<"ERROR: Cannor generate a key!!\n";
                exit(1);
        }
        bool exist = false;
        int shmid = shmget(key, 300*sizeof(bool), 0600 | IPC_CREAT | IPC_EXCL);
        if(shmid < 0){
                if(errno != EEXIST){
                        std::cout<<"ERROR: Cannot create shared memory segment!!\n";
                        exit(1);
                }
                else{
                        shmid = shmget(key, 300*sizeof(bool), 0600);
                        exist = true;
                }
        }
        bool* shared_arr = (bool*) shmat(shmid, NULL, 0);
        if(shared_arr == (bool*) -1){
                std::cout<<"ERROR: Cannot attach shared mem segment!!\n";
                perror("shmat");
                exit(errno);
        }
        shared_arr[0] = false;
        shared_arr[1] = false;
        for (int i = 2; i*i <= TABLE_SIZE; i++){
            if (shared_arr[i]){
                    for (int j = i*i; j <= TABLE_SIZE; j += i){
                        shared_arr[j] = false;
                    }
            }
        }
    for(int i = 0;i<TABLE_SIZE;++i){
        std::cout<<shared_arr[i]<<" ";
    }
        if(shmdt(shared_arr) < 0){
                std::cout<<"ERROR: Cannot detach the shm segment!!\n";
                exit(1);
        }
    return 0;
}
