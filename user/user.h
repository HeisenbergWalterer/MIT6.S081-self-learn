struct stat;
struct rtcdate;

// system calls（直接向内核请求服务的接口，会触发从用户模式到内核模式的切换）
/*
    系统调用的流程：
    1. 用户调用：fork() （user.h 中声明的接口）
    2. 汇编转换：通过汇编存根切换到监督者模式
    3. 内核分发：根据系统调用号找到对应的处理函数
    4. 内核执行：执行实际的内核逻辑（创建进程、管理内存等）
*/

// 进程管理
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int*);
int getpid(void);
int exec(char*, char**);
int kill(int);

// 文件系统
int open(const char*, int);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int fstat(int fd, struct stat*);
int unlink(const char*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);

// 特殊文件
int pipe(int*);
int mknod(const char*, short, short);

// 内存管理
char* sbrk(int);
int sleep(int);
int uptime(void);

int trace(int);

struct sysinfo;
int sysinfo(struct sysinfo *);

// ulib.c 用户库函数（在用户空间实现的函数，可能会调用系统调用，但本身不是系统调用）
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...);
void printf(const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
