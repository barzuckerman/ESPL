#include <stdio.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include "LineParser.h"


void execute(cmdLine *pCmdLine);
int toInt (char *str);
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
                //-----------------Task 1b-------------------
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
               else if(strcmp(pcmdl->arguments[0], "stop") == 0){
                    if (pcmdl->argCount < 2){
                        fprintf(stderr, "missing process id.\n");
                    }
                    else{
                        int pid = toInt(pcmdl->arguments[1]);
                        kill(pid, SIGSTOP);
                    }
               }
               else if(strcmp(pcmdl->arguments[0], "wake") == 0){
                    if (pcmdl->argCount < 2){
                        fprintf(stderr, "missing process id.\n");
                    }
                    else{
                        int pid = toInt(pcmdl->arguments[1]);
                        kill(pid, SIGCONT);
                    }
               }
               else if(strcmp(pcmdl->arguments[0], "term") == 0){
                    if (pcmdl->argCount < 2){
                        fprintf(stderr, "missing process id.\n");
                    }
                    else{
                        int pid = toInt(pcmdl->arguments[1]);
                        kill(pid, SIGINT);
                    }
               }
               else{
                execute(pcmdl);
               }
               freeCmdLines(pcmdl);
            }
        } else {
            perror("getcwd() error");
        }
    }
    return 0;
}

 void execute(cmdLine *pCmdLine){
    pid_t pid = fork(); // duplicate current process
    
    
    if (pid == 0){
        //-----------------Task 1a-------------------
        if (debug){ 
        fprintf(stderr,"PID: %d\n",pid);
        fprintf(stderr,"Executing command: %s\n",pCmdLine->arguments[0]);
        }
        //-----------------Task 3-------------------
        if (pCmdLine->inputRedirect != NULL) { 
            int fileOpen = open(pCmdLine->inputRedirect, O_RDONLY | O_CREAT, 0777); //0777 -> for everyone
            if (fileOpen == -1) {
                _exit(EXIT_FAILURE);
            }
            close(fileOpen); 
        }

        if (pCmdLine->outputRedirect != NULL) {
            int fileOut = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT, 0777);
            if (fileOut == -1) {
                _exit(EXIT_FAILURE);
            }
            close(fileOut);
        }
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

 int toInt (char *str){
    int num;
    sscanf(str, "%d", &num);
    return num;
 }