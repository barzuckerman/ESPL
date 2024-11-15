#include <stdio.h>
#include <string.h>


char encode(int c, int add , int sign);

int main(int argc, char *argv[]) {
    FILE *infile = stdin; 
    FILE *outfile = stdout;
    char *key = NULL; 
    int debug = 1,c ,sign = 1; //default debug: on, default sign: +


    for(int i = 1; i<argc; i++){
        //----------part1
        if(argv[i][0] =='-' && argv[i][1] == 'D')
            debug = 0;
        else if (argv[i][0] =='+' && argv[i][1] == 'D') 
            debug = 1;

        //----------part2
        else if (argv[i][0] == '+' && argv[i][1] == 'E'){
            sign = 1;
            key = argv[i] + 2;
        }
        else if (argv[i][0] == '-' && argv[i][1] == 'E'){
            sign = -1;
            key = argv[i] + 2;
        }

        //----------part3
        else if(argv[i][0] == '-' && argv[i][1] == 'i'){
            infile = fopen(argv[i] + 2, "r"); ///read from the file
            if (infile == NULL) { //if the file was't opened successfully
                fprintf(stderr,"can't open this file \n");
            }
        }

        else if(argv[i][0] == '-' && argv[i][1] == 'o'){
            outfile = fopen(argv[i] + 2, "w"); ///read from the file
            if (outfile == NULL) { //if the file was't opened successfully
                fprintf(stderr,"can't open a file \n");
            }
        }

        if (debug && argv[i][1] != 'D') {
            fprintf(stderr, "%s\n", argv[i]);
        }
    }

        int counter = 0;
        while ((c = fgetc(infile)) != EOF) { //until the end of the file
            if (key == NULL){
                fputc(c, outfile);
            } 
            else{
                fputc(encode(c, key[counter] - '0', sign), outfile);
                counter = (key[++counter] == '\0') ? 0 : counter;  // cycle through key numbers encoding
            }

        }

    // close files
    fclose(infile);
    fclose(outfile);
    return 0;
}

/*encode function using mod in order to create cycling pattern*/
char encode(int c, int add , int sign){
    if (c >= 'a' && c <= 'z')
        c = ((c -'a' + add*sign +26) %26) +'a' ;

    else if(c >= 'A' && c <= 'Z')
        c =((c -'A' + add*sign + 26) %26) +'A' ;

    else if (c >= '0' && c <= '9')
        c = ((c -'0' + add*sign +10) %10) +'0' ;
    return c;
}