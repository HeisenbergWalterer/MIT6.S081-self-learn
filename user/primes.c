#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void addNumber(int write_fd) {
    for(int i = 2; i <= 35; i++) {
        write(write_fd, &i, sizeof(i)); // &i为指向i的指针
    }
    close(write_fd); // 写完记得关闭管口
}

// 通过递归实现逐步筛选prime
void prime_filter(int read_fd) {
    int prime;
    if(read(read_fd, &prime, sizeof(prime)) == 0) {
        close(read_fd);
        return;
    }
    printf("prime %d\n", prime);

    int pipe_next[2];
    pipe(pipe_next);

    if(fork() == 0) { // 子进程
        close(pipe_next[1]); // 在后续筛选中，对于这个pipe_next只读
        close(read_fd);
        prime_filter(pipe_next[0]);
        exit(0);
    } 
    else {          // 父进程
        close(pipe_next[0]);

        int num;
        while(read(read_fd, &num, sizeof(num)) != 0) {
            if (num % prime != 0) {
                write(pipe_next[1], &num, sizeof(num));
            }
        }

        // 文件描述符用完要关
        close(read_fd);
        close(pipe_next[1]);
        wait(0);
    }

}

int main(int argc, char *argv[]) {
    int pipe_fd[2];
    pipe(pipe_fd);

    if(fork() == 0) { // 向管道中添加初始数字2-35
        close(pipe_fd[0]);
        addNumber(pipe_fd[1]);    // 只写则把读关闭
        exit(0);
    } else {
        close(pipe_fd[1]);
        prime_filter(pipe_fd[0]); // 只读则把写关闭
        wait(0);
    }
    
    exit(0);
}