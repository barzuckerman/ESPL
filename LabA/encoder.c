#include <stdio.h>
#include <string.h>


char encode(int c, int add , int sign);

int main(int argc, char *argv[]) {
    FILE *infile = stdin; 
    FILE *outfile = stdout;
    char *key = NULL; 
    int debug = 1, digit = 0, c ,sign = 1; //default debug: on, default sign: +


    for(int i = 1; i<argc; i++){
        //----------part1
        if(argv[i][0] =="-" && argv[i][1] == 'D' && argv[i][2] != 0 )
            debug = 0;
        else if (argv[i][0] =="+" && argv[i][1] == 'D' && argv[i][2] != 0) 
            debug = 1;

        //----------part2
        else if (argv[i][0] == '+' && argv[i][1] == 'E' && argv[i][2] != 0){
            sign = 1;
            key = argv[i] + 2;
        }

        else if (argv[i][0] == '-' && argv[i][1] == 'E' && argv[i][2] != 0){
            sign = -1;
            key = argv[i] + 2;
        }

        //----------part3
        else if(argv[i][0] == '-' && argv[i][1] == 'i'&& argv[i][2] != 0){
            infile = fopen(argv[i] + 2, "r"); ///read from the file
            if (infile == NULL) { //if the file was't opened successfully
                fprintf(stderr,"can't open this file \n");
            }
        }

        else if(argv[i][0] == '-' && argv[i][1] == 'o'&& argv[i][2] != 0){
            outfile = fopen(argv[i] + 2, "w"); ///read from the file
            if (outfile == NULL) { //if the file was't opened successfully
                fprintf(stderr,"can't open a file \n");
            }
        }

        if(debug){
            fprintf(stderr, "%s\n", argv[i]);
        }

        int counter = 0;
        while(1){
            while ((c = fgetc(infile)) != '\n') {
                if (feof(infile)) //end of the file
                {
                    if (outfile == stdout && infile != stdin)
                        printf("\n");
                    //exist "normally" and close the files
                    fclose(infile);
                    fclose(outfile);
                    return 0;
                }
                 else if (key == NULL)
                    fputc((char)c, outfile);
                else
                {
                    fputc(encode((char)c, key[counter] - '0', sign), outfile);
                    if (!key[++counter])
                        counter = 0;
                }
            }

             if (key != NULL && !key[++counter]) //incase I went out the while loop
                counter = 0; //reset the counter

            fputc('\n', outfile);
        }
        
        
    }

    return 0; 
}

char encode(int c, int add , int sign){
    if (c >= 'a' && c <= 'z')
        c = ((c -'a' + add*sign +26) %26) +'a' ;

    else if(c >= 'A' && c <= 'Z')
        c =((c -'A' + add*sign) %26) +'A' ;

    else if (c >= '0' && c <= '9')
        c = ((c -'0' + add*sign +10) %10) +'0' ;
    return c;
}