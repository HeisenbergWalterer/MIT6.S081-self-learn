#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h" // 为了使用MAXARG

int main(int argc, char *argv[]) {
    char out[MAXARG]; // 用作缓冲区

    // 将输出读取到out
    char *p = out;
    while(read(0, p, 1) > 0) {
        p++;
    }
    *p = '\0';

    // 储存每个参数的起始位置
    char *args[MAXARG]; // 指针数组
    int index = 0;

    if(*out != '\0') {
        args[index] = out;
    }
    
    for(int i = 1; out[i] != '\0'; i++) {
        if(out[i] == '\n' && out[i + 1] != '\0') {
            out[i] = '\0';
            args[++index] = &out[i + 1];
        }
    }

    // 去掉每个参数末尾的换行符
    for(int i = 0; i <= index; i++) {
        int len = strlen(args[i]);
        if(len > 0 && args[i][len - 1] == '\n') {
            args[i][len - 1] = '\0';
        }
    }

    // 为每一行执行一次命令（即fork exec一次）
    for(int i = 0; i <= index; i++) {
        if(fork() == 0) {
            // 构建参数数组
            char *exec_argv[MAXARG]; // 指针数组
            int arg_count = 0;

            for(int j = 1; j < argc; j++) {
                exec_argv[arg_count++] = argv[j];
            }
            exec_argv[arg_count++] = args[i]; // 从标准输入读取的额外参数
            exec_argv[arg_count] = 0; // NULL 结尾

            exec(argv[1], exec_argv); // exec需要完整的程序参数，包括程序名本身
            exit(1);
        }
        wait(0);
    }

    exit(0);
}