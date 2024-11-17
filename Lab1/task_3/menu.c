#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#define MAX_LEN 20000;

typedef struct fun_desc {
char *name;
char (*fun)(char);
}FunDesc;

char my_get(char c);
char cprt(char c);
char encrypt(char c);
char decrypt(char c);
char xprt(char c);
char dprt(char c);


void menu(){
    char str[20000];
    char carray [5] = {'\0'};
    FunDesc arr[] = {{"cprt", cprt},{"encrypt", encrypt},{"decrypt", decrypt},{"xprt", xprt},{"dprt", dprt},{NULL, NULL}};
    fprintf(stdout,"Select operation from the following menu:\n");
    while(!feof(stdin)){
        if(fgets(str, sizeof(str), stdin) == NULL)
            break;
        int i = 0;
        while (arr[i].name != NULL) {
            fprintf(stdout,"%d. %s\n", i + 1, arr[i].name);
            i++;
        }
        fprintf(stdout,"Select operation from the following menu:\n");

    }
    return 0;
    
    
    
}