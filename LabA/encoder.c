#include <stdio.h>
#include <string.h>




int encode(int c){
    return c;
}

int main(int argc, char *argv[]) {
    FILE *infile = stdin; 
    FILE *outfile = stdout; 
    int debug = 1; //default: on
    for(int i = 1; i<argc; i++){
        if(strcmp(argv[i], "-D") == 0 )//which means they are equal
            debug = 0;
        else if (strcmp(argv[i], "+D") == 0)
            debug = 1;

        if(debug){
            fprintf(stderr, i, argv[i]);

        }
        int c;
        while ((c = fgetc(infile)) != EOF) {
            c = encode(c);
            fputc(c, outfile);
        }
        
    }

    return 0; //exist "normally"
}