#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <linux/limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include "LineParser.h"
#include "Looper.c"
void execute(cmdLine *pCmdLine);
int debug = 0;

int main() {
    char cwd[PATH_MAX];
    char input[2048];
    while (1){
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Current working directory: %s\n", cwd);
            if(fgets(input, sizeof(input),stdin)!=NULL){
               cmdLine* pcmdl = parseCmdLines(input);
               if(strcmp(pcmdl->arguments[0], "quit") == 0){
                    return 0;
               }
               else if(strcmp(pcmdl->arguments[0], "-d") == 0){
                    debug = 1;
               }
               else if(strcmp(pcmdl->arguments[0], "+d") == 0){
                    debug = 0;
               }
               else if(strcmp(pcmdl->arguments[0], "cd") == 0){ 
                    if(chdir(pcmdl->arguments[1]) == -1){
                        if (errno == ENOENT) {
                            fprintf(stderr, "Error: Directory does not exist.\n");
                        } else if (errno == EACCES) {
                            fprintf(stderr, "Error: Permission denied.\n");
                        } else {
                            fprintf(stderr, "Error: %s\n", strerror(errno));
                        }
                    }
               }
               execute(pcmdl);
               freeCmdLines(pcmdl);
               
            }
        } else {
            perror("getcwd() error");
        }
    }
}

 void execute(cmdLine *pCmdLine){
    pid_t pid = fork(); // duplicate current process
    if (debug){
        fprintf(stderr,"PID: %d\n",pid);
        fprintf(stderr,"Executing command: %s\n",pCmdLine->arguments[0]);
    }
    
    if (pid == 0){
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        perror("execv failed");
        _exit(EXIT_FAILURE);
    }
    else if (pid < 0){
        perror("fork failed");
    }
    else{
        if (pCmdLine->blocking){
            waitpid(pid, NULL, 0);
        }
    }
 }