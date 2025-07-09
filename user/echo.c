#include "kernel/types.h" // 基本类型定义
#include "kernel/stat.h"  // 文件状态定义
#include "user/user.h"    // 用户程序的系统调用声明

int
main(int argc, char *argv[])
{
  int i;

  for(i = 1; i < argc; i++){ // 从i = 1开始，因为argv[0]是程序名
    write(1, argv[i], strlen(argv[i]));
    if(i + 1 < argc){
      write(1, " ", 1);
    } else {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
