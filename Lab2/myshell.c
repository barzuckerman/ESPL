#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include "LineParser.h"
void execute(cmdLine *pCmdLine);

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
               execute(pcmdl);
               freeCmdLines(pcmdl);
               
            }
        } else {
            perror("getcwd() error");
        }
    }
    return 0;

}

 void execute(cmdLine *pCmdLine){
    if (execv(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
            perror("execv failed");
            exit(1);
    }
 }