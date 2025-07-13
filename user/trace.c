#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/*
  mask用于指定要跟踪的系统调用：
  mask 的每一位对应一个系统调用号
  如果某一位被设置为1，就跟踪对应的系统调用
  如果某一位是0，就不跟踪对应的系统调用
*/

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

  // 参数数量至少为3个：程序名、mask（数字）、要执行的命令（命令名+参数）
  // eg: trace 32 grep hello README
  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  if (trace(atoi(argv[1])) < 0) { // 调用trace系统调用（trace有返回值）
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }
  
  // 执行目标程序
  for(i = 2; i < argc && i < MAXARG; i++){
    nargv[i-2] = argv[i];
  }
  exec(nargv[0], nargv);
  exit(0);
}
