#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    int pipe1[2], pipe2[2];
    pipe(pipe1); // 父 -> 子
    pipe(pipe2); // 子 -> 父
    int pid = fork();

    if(pid == 0) { // 子进程
        char byte;
        if(read(pipe1[0], &byte, 1) == 1) {
            printf("%d: received ping\n", getpid());
            write(pipe2[1], &byte, 1);
        }
        exit(0);
    } else {      // 父进程
        char sent_byte = 'p', receive_byte;
        write(pipe1[1], &sent_byte, 1);
        if(read(pipe2[0], &receive_byte, 1) == 1) {
            printf("%d: received pong\n", getpid());
        }
        wait(0);
    }
    exit(0);
}