#include "util.h"

#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_GETDENTS 141
#define SYS_CLOSE 6
#define STDOUT 1
#define O_RDONLY 0

#define BUFFER_SIZE 8192

struct linux_dirent {
    unsigned long ino;
    unsigned long off;
    unsigned short len;
    char name[];
};

extern int system_call();

int main(int argc, char *argv[], char *envp[]) {
    char buffer[BUFFER_SIZE];
    int fileDir, bytes_read, bpos;
    struct linux_dirent *file;

    //Open the current directory
    fileDir = system_call(SYS_OPEN, ".", O_RDONLY);
    if (fileDir < 0) {
        system_call(SYS_WRITE, STDOUT, "Error opening directory\n", 24);
        return 0x55;
    }

    //read
    bytes_read = system_call(SYS_GETDENTS, fileDir, buffer, BUFFER_SIZE);
    if (bytes_read < 0) {
        system_call(SYS_WRITE, STDOUT, "Error reading directory\n", 24);
        system_call(SYS_CLOSE, fileDir);
        return 0x55;
    }

    //directory entries iteration
    for (bpos = 0; bpos < bytes_read;) {
        file = (struct linux_dirent *)(buffer + bpos);
        if(strcmp(file->name,".")!=0 && strcmp(file->name,"..")!=0){
            system_call(SYS_WRITE, STDOUT, file->name, strlen(file->name));
            system_call(SYS_WRITE, STDOUT, "\n", 1);
        }
        bpos += file->len;
    }

    //close the directory
    system_call(SYS_CLOSE, fileDir);
    return 0;
}
