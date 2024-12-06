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

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0
//------------struct declaration------------
typedef struct process
{
    cmdLine *cmd;         // the parsed command line
    pid_t pid;            // the process id that is running the command
    int status;           // status of the process: RUNNING/SUSPENDED/TERMINATED
    struct process *next; // next process in chain
} process;
//------------functions declaration------------
void execute(cmdLine *pCmdLine);
int toInt(char *str);
void executePipeline(cmdLine *pcmd1, cmdLine *pcmd2);
void addProcess(process **process_list, cmdLine *cmd, pid_t pid);
void printProcessList(process **process_list);
void freeProcessList(process* process_list);
void updateProcessList(process **process_list);
//------------global------------
int debug = 0;
process *process_list;


int main(int argc, char **argv)
{
    char cwd[PATH_MAX];
    char input[2048];
    if (argc > 1)
    { // Check if an argument is passed to prevent segmentation faults
        if (strcmp(argv[1], "-d") == 0)
            debug = 1;
        else if (strcmp(argv[1], "+d") == 0)
            debug = 0;
    }
    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("Current working directory: %s\n", cwd);
            if (fgets(input, sizeof(input), stdin) != NULL)
            {
                cmdLine *pcmdl = parseCmdLines(input);
                if (strcmp(pcmdl->arguments[0], "quit") == 0)
                {
                    return 0;
                }
                else if (strcmp(pcmdl->arguments[0], "cd") == 0)
                {
                    if (chdir(pcmdl->arguments[1]) == -1)
                    {
                        if (errno == ENOENT)
                        {
                            fprintf(stderr, "Error: Directory does not exist.\n");
                        }
                        else if (errno == EACCES)
                        {
                            fprintf(stderr, "Error: Permission denied.\n");
                        }
                        else
                        {
                            fprintf(stderr, "Error: %s\n", strerror(errno));
                        }
                    }
                }
                else if (strcmp(pcmdl->arguments[0], "stop") == 0)
                {
                    if (pcmdl->argCount < 2)
                    {
                        fprintf(stderr, "missing process id.\n");
                    }
                    else
                    {
                        int pid = toInt(pcmdl->arguments[1]);
                        kill(pid, SIGSTOP);
                    }
                }
                else if (strcmp(pcmdl->arguments[0], "wake") == 0)
                {
                    if (pcmdl->argCount < 2)
                    {
                        fprintf(stderr, "missing process id.\n");
                    }
                    else
                    {
                        int pid = toInt(pcmdl->arguments[1]);
                        kill(pid, SIGCONT);
                    }
                }
                else if (strcmp(pcmdl->arguments[0], "term") == 0)
                {
                    if (pcmdl->argCount < 2)
                    {
                        fprintf(stderr, "missing process id.\n");
                    }
                    else
                    {
                        int pid = toInt(pcmdl->arguments[1]);
                        kill(pid, SIGINT);
                    }
                }
                else if (strcmp(pcmdl->arguments[0], "procs") == 0)
                {
                    printProcessList(process_list);
                }
                else
                {
                    execute(pcmdl);
                }
                freeCmdLines(pcmdl);
            }
        }
        else
        {
            perror("getcwd() error");
        }
    }
    return 0;
}

void execute(cmdLine *pCmdLine)
{
    if (pCmdLine->next)
    { // If there's a next command, handle piping
        executePipeline(pCmdLine, pCmdLine->next);
    }

    else
    {
        pid_t pid = fork(); // duplicate current process
        if (pid < 0)
        {
            perror("fork failed");
        }

        if (pid == 0)
        {

            if (pCmdLine->inputRedirect != NULL)
            {
                int fileOpen = open(pCmdLine->inputRedirect, O_RDONLY | O_CREAT, 0777); // 0777 -> for everyone
                if (fileOpen == -1)
                {
                    _exit(EXIT_FAILURE);
                }
                dup2(fileOpen, STDIN_FILENO);
                close(fileOpen);
            }

            if (pCmdLine->outputRedirect != NULL)
            {
                int fileOut = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT, 0777);
                if (fileOut == -1)
                {
                    _exit(EXIT_FAILURE);
                }
                dup2(fileOut, STDOUT_FILENO);
                close(fileOut);
            }
            execvp(pCmdLine->arguments[0], pCmdLine->arguments);
            perror("execv failed");
            _exit(EXIT_FAILURE);
        }
        else
        {
            if (debug)
            {
                fprintf(stderr, "PID: %d\n", pid);
                fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
            }

            addProcess(&process_list, pCmdLine, pid);

            if (pCmdLine->blocking)
            {
                waitpid(pid, NULL, 0);
            }
        }
    }
}

int toInt(char *str)
{
    int num;
    sscanf(str, "%d", &num);
    return num;
}

void executePipeline(cmdLine *pcmd1, cmdLine *pcmd2)
{
    int pipeFileDescriptors[2];
    pid_t child1, child2;

    if (pipe(pipeFileDescriptors) == -1)
    {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    child1 = fork();
    if (child1 == -1)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (child1 == 0)
    {
        if (pcmd1->inputRedirect != NULL)
        {
            int inputFile = open(pcmd1->inputRedirect, O_RDONLY);
            if (inputFile == -1)
            {
                perror("Failed to open input file");
                exit(EXIT_FAILURE);
            }
            dup2(inputFile, STDIN_FILENO);
            close(inputFile);
        }

        close(STDOUT_FILENO);                          // close standart output
        dup(pipeFileDescriptors[1]);                   // dup write end
        close(pipeFileDescriptors[0]);                 // Close read end
        close(pipeFileDescriptors[1]);                 // Close write end after duplicating
        execvp(pcmd1->arguments[0], pcmd1->arguments); // execute first command
        perror("execv failed");
        // exit(EXIT_SUCCESS);
    }
    else{
        addProcess(&process_list, pcmd1, child1);
    }
    
    child2 = fork();
    if (child2 == -1)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (child2 == 0)
    {
        if (pcmd2->outputRedirect != NULL)
        {
            int outputFile = open(pcmd2->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if (outputFile == -1)
            {
                perror("Failed to open output file");
                exit(EXIT_FAILURE);
            }
            dup2(outputFile, STDOUT_FILENO);
            close(outputFile);
        }

        close(STDIN_FILENO);                           // close standart input
        dup(pipeFileDescriptors[0]);                   // dup read end
        close(pipeFileDescriptors[1]);                 // Close write end
        close(pipeFileDescriptors[0]);                 // Close read end after duplicating
        execvp(pcmd2->arguments[0], pcmd2->arguments); // execute seconed command
        perror("execv failed");
        // exit(EXIT_SUCCESS);
    }
    else{    // parent
        addProcess(&process_list, pcmd2, child2);
    }

    close(pipeFileDescriptors[0]);
    close(pipeFileDescriptors[1]);
    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);
}
/*Receive a process list (process_list), a command (cmd), and the process id (pid) of the process running the command.
Note that process_list is a pointer to a pointer so that we can insert at the beginning of the list if we wish*/
void addProcess(process **process_list, cmdLine *cmd, pid_t pid)
{
    process *new_process = (process *)malloc(sizeof(process));
    if (!new_process)
    {
        perror("Failed to allocate memory for new process node");
        exit(EXIT_FAILURE);
    }
    new_process->cmd = cmd;
    new_process->pid = pid;
    new_process->status = RUNNING; // Assuming new processes are initially running
    new_process->next = *process_list;
    *process_list = new_process;
}

/*print the processes.*/
void printProcessList(process **process_list)
{
    updateProcessList(process_list);
    process *curr = *process_list;
    int count = 1;
    printf("Index\t\tPID\t\tCommand\t\tSTATUS\n");
    while (curr != NULL)
    {
        // Print each process
        printf("%d\t\t%d\t\t%s\t\t", count++, curr->pid, curr->cmd->arguments[0]);
        switch (curr->status)
        {
        case TERMINATED:
            printf("Terminated\n");
            break;
        case RUNNING:
            printf("Running\n");
            break;
        case SUSPENDED:
            printf("Suspended\n");
            break;
        default:
            break;
        }

        if (curr->status == TERMINATED) {
            process* toDelete = curr;
            curr = curr->next;
            freeCmdLines(toDelete->cmd);
            free(toDelete);
            continue;
        }
        curr = curr->next;
    }
}

/*free all memory allocated for the process list.*/
void freeProcessList(process* process_list){
    while (process_list != NULL) {
        process *temp = process_list;
        process_list = process_list->next;
        freeCmdLines(temp->cmd);
        free(temp);
    }
}

/*go over the process list, and for each process check if it is done, you can use waitpid with the option WNOHANG*/
void updateProcessList(process **process_list){
    process *curr = *process_list;
    int status;
    pid_t result;

    while (curr != NULL) {
        result = waitpid(curr->pid, &status, WNOHANG);
        
        if (result == 0) {
            //still running
            curr = curr->next;
            continue;
        } else if (result == -1) {
            perror("waitpid failed");
            return;
        }
        
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            // terminated
            curr->status = TERMINATED;
        } else if (WIFSTOPPED(status)) {
            // stopped
            curr->status = SUSPENDED;
        } else if (WIFCONTINUED(status)) {
            // resumed
            curr->status = RUNNING;
        }

        curr = curr->next;
    }
}

/*find the process with the given id in the process_list and change its status to the received status.*/
void updateProcessStatus(process* process_list, int pid, int status){
    while (process_list!=NULL)
    {
        if(process_list->pid == pid){
            process_list->status = status;
            return;
        }
        process_list = process_list->next;
    }
}