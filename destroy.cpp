#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <string>

int main() {
        std::string pathname = "shm_file_2";
        key_t key = ftok(pathname.c_str(), 'a');
        if(key < 0) {
                std::cout << "ERROR: Cannot generate a key!!\n";
                exit(errno);
        }
        int shmid = shmget(key, 300 * sizeof(bool), 0600);
        shmctl(shmid, IPC_RMID, NULL);

        return 0;
}
