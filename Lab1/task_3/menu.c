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
char* map(char *array, int array_length, char (*f) (char));


int menu(){
    char str[20000];
    char carray [5] = {'\0'};
    char input[20000];
    FunDesc arr[] = {{"cprt", cprt},{"encrypt", encrypt},{"decrypt", decrypt},{"xprt", xprt},{"dprt", dprt},{NULL, NULL}};
    int bound = sizeof(arr) / sizeof(arr[0]);
    fprintf(stdout,"Select operation from the following menu:\n");
    while(!feof(stdin)){
        if(fgets(str, sizeof(str), stdin) == NULL)
            break;
        strncpy(carray, str, sizeof(carray) - 1);
        int i = 0;
        int num_input = 0;
        
        while (arr[i].name != NULL) {
            fprintf(stdout,"%d. %s\n", i + 1, arr[i].name);
            i++;
        }
        fprintf(stdout,"select operation number:\n");
        
        if (fgets(input, sizeof(input), stdin) == NULL) { //check i can read
            return 0;
        }
        input[strcspn(input, "\n")] = '\0'; //remove the last char of the empty char
        num_input = strtol(input, NULL, 10); //convert to int

        if(num_input < 0 || num_input >= bound){ //check the number is ok
            printf("Not within bounds\n");
            return 0;
        }
    
        printf("Within bounds\n");
        
        char *curr = map(carray, sizeof(carray), arr[num_input].fun); //alocate a dynemic var
        memcpy(carray, curr, 20000);
        free(curr);
    }
    return 0;
    
    
    
}