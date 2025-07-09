#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"  // 为了使用 DIRSIZ

// 通过path获取文件名
char* getFileName(char *path) {
    char *p;
    for(p = path + strlen(path); p >= path && *p != '/'; p--);

    return p + 1;
}

void find(char *path, char *target_name)
{
    char buf[512], *p;
    int fd;
    struct dirent de; // 目录项结构
    struct stat st;   // 文件状态结构
    
    // 打开目录或文件
    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    /* 在第一次调用该函数时，path对应的一定是目录，但之后就可能是目录或文件 */

    // 获取文件或目录状态信息
    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 根据文件类型分别处理
    switch(st.type){
        case T_FILE: // 文件
            if(strcmp(getFileName(path), target_name) == 0) {
                    printf("%s\n", path);
                }
            break;

        case T_DIR: // 目录
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                return;
            }

            // 添加路径分隔符
            strcpy(buf, path); // buf是新路径指针
            p = buf+strlen(buf);
            *p++ = '/';

            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0) // 跳过空的目录项
                continue;

                // 跳过 "." 和 ".." 目录
                if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;

                // 构建完整的文件路径
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                
                // 递归查找
                find(buf, target_name);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    char *path, *target_name;

    if(argc != 3) {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }

    path = argv[1];
    target_name = argv[2];

    find(path, target_name);

    exit(0);
}