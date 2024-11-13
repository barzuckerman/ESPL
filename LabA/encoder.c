#include <stdio.h>
#include <string.h>


char encode(int c, int add , int sign);

int main(int argc, char *argv[]) {
    FILE *infile = stdin; 
    FILE *outfile = stdout;
    char *eNums = NULL; 
    int debug = 1, digit = 0, c ,sign = 1; //default debug: on, default sign: +


    for(int i = 1; i<argc; i++){
        //----------part1
        if(strcmp(argv[i], "-D") == 0 )
            debug = 0;
        else if (strcmp(argv[i], "+D") == 0) 
            debug = 1;

        //----------part2
        else if (argv[i][0] == '+' && argv[i][1] == 'E' && argv[i][2] != 0){
            sign = 1;
            eNums = argv[i] + 2;
        }

        else if (argv[i][0] == '-' && argv[i][1] == 'E' && argv[i][2] != 0){
            sign = -1;
            eNums = argv[i] + 2;
        }

        if(debug){
            fprintf(stderr, i, argv[i]);
        }

        int counter = 0;
        while(1){
            while ((c = fgetc(infile)) != '\n') {
                if (feof(infile)) //end of the file
                {
                    if (outfile == stdout && infile != stdin)
                        printf("\n");
                    //exist "normally"
                    fclose(infile);
                    fclose(outfile);
                    return 0;
                }
                 else if (eNums == NULL)
                    fputc((char)c, outfile);
                else
                {
                    fputc(encode((char)c, eNums[counter] - '0', sign), outfile);
                    if (!eNums[++counter])
                        i = 0;
                }
            }

             if (eNums != NULL && !eNums[++counter]) //incase I went out the while loop
                counter = 0; //reset the counter

            fputc('\n', outfile);
        }
        
        
    }

    return 0; 
}

char encode(int c, int add , int sign){
    int enc;

    if (c >= 'a' && c <= 'z')
        enc = ((c -'a' + add*sign +26) %26) +'a' ;

    else if(c >= 'A' && c <= 'Z')
        enc =((c -'A' + add*sign) %26) +'A' ;

    else if (c >= '0' && c <= '9')
        enc = ((c -'0' + add*sign +10) %10) +'0' ;

    return c;
}