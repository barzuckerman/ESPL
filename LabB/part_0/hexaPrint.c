
#include <stdio.h>
#include <string.h>
#define SIZE 128

void PrintHex(const unsigned char *buffer, size_t length){
    for (size_t i = 0; i<length; ++i){
        printf("%02x", buffer[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {

    FILE *infile = fopen(argv[0], "r"); ///read from the file
    if (infile == NULL) { //if the file was't opened successfully
        fprintf(stderr,"can't open this file \n");
    }

    size_t bytes;
    unsigned char buffer[SIZE];
    while((bytes = fread(buffer, sizeof(char), SIZE, infile))>0){
        PrintHex(buffer,bytes);
    }

    fclose(infile);
    return 0;

}