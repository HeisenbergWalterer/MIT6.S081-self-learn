#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path) // 格式化文件名
{
  static char buf[DIRSIZ+1];
  char *p;

  // 找到路径中最后一个 '/' 之后的字符（文件名部分）
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // 返回空格填充的文件名(将文件名用空格填充到长度为文件名最大长度DIRSIZ)
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path) // 用于列出文件或目录的内容
{
  char buf[512], *p;
  int fd;
  struct dirent de; // 目录项结构
  struct stat st;   // 文件状态结构

  // 打开文件或目录
  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  // 获取文件状态信息
  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // 根据文件类型分别处理
  switch(st.type){
  case T_FILE:
    // 如果是普通文件，直接打印文件信息
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    // 如果是目录，列出目录中的所有文件
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }

    // 目录最后加上/
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }

      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    // 如果没有参数，列出当前目录
    ls(".");
    exit(0);
  }

  // 如果有参数，列出每个指定的文件/目录
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
