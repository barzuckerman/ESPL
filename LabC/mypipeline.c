#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char **argv)
{
    int pipeFileDescriptors[2];
    pid_t child1, child2;

    if (pipe(pipeFileDescriptors) == -1)
    {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "(parent_process>forking...)\n");
    child1 = fork();
    fprintf(stderr, "(parent_process>created process with id: %d )\n",child1);
    if (child1 == -1)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (child1 == 0)
    { // succed
        // close standart output
        close(STDOUT_FILENO);
        
        // dup write end
        fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe...)\n");
        dup(pipeFileDescriptors[1]);

        // Close write end after writing
        close(pipeFileDescriptors[1]);

        // Execute ls -l
        fprintf(stderr, "(child1>going to execute cmd: ...)\n");
        char *args[] = {"ls", "-l", NULL};
        execvp(args[0], args); // Execute ls -l
        perror("execv failed");

        // exit(EXIT_SUCCESS);
    }
    else
    { // Parent process
        // Close the write end of the pipe
        fprintf(stderr, "(parent_process>closing the write end of the pipe...)\n");
        close(pipeFileDescriptors[1]);
    }
    fprintf(stderr, "(parent_process>forking...)\n");
    child2 = fork();
    fprintf(stderr, "(parent_process>created process with id: %d )\n",child2);


    if (child2 == -1)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (child2 == 0)
    { // succed
        // close standart input
        close(STDIN_FILENO);

        // dup read end
        fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe...)\n");
        dup(pipeFileDescriptors[0]);

        // Close write end after writing
        close(pipeFileDescriptors[0]);

        // Execute tail -n 2
        fprintf(stderr, "(child2>going to execute cmd: ...)\n");
        char *args[] = {"tail", "-n", "2", NULL};
        execvp(args[0], args);
        perror("execv failed");

        // exit(EXIT_SUCCESS);
    }
    else
    { // Parent process
        // Close the read end of the pipe
        fprintf(stderr, "(parent_process>closing the read end of the pipe...)\n");
        close(pipeFileDescriptors[0]);
    }
    fprintf(stderr, "(parent_process>waiting for child processes to terminate...)\n");
    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    fprintf(stderr, "(parent_process>exiting...)\n");
    return 0;
}