#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int pipeFileDescriptors[2];
    pid_t child;
    char buffer[100];

    if (pipe(pipeFileDescriptors) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }
    
    // fprintf(stderr, "(parent_process>forking...)\n");
    child = fork();

    if (child == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (child == 0) { // succed
        // Close unused read end
        close(pipeFileDescriptors[0]);
        const char *message = "hello";
        write(pipeFileDescriptors[1], message, strlen(message) + 1);
        // Close write end after writing
        close(pipeFileDescriptors[1]);

        exit(EXIT_SUCCESS);
    } else { // Parent process
        // Close the write end of the pipe
        close(pipeFileDescriptors[1]);
        read(pipeFileDescriptors[0], buffer, sizeof(buffer)); // Read message
        printf("Received message: %s\n", buffer);
        close(pipeFileDescriptors[0]);
    }


    return 0;
}