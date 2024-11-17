#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_LEN 5

typedef struct fun_desc {
char *name;
char (*fun)(char);
}FunDesc;


char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  /* TODO: Complete during task 2.a */
  for(int i = 0 ; i<array_length ; i++){
    mapped_array[i] = (*f)(array[i]);
  }
  return mapped_array;
}

/* Ignores c, reads and returns a character from stdin using fgetc. */
char my_get(char c){
  return fgetc(stdin);
}
/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns the value of c unchanged. */
char cprt(char c){
  if( c>=0x20 && c<=0x7E)
    printf("%c in ASCII: %i\n", c, c);
  else
    printf(".\n");
  return c;
}
/* Gets a char c and returns its encrypted form by adding 1 to its value. If c is not between 0x1F and 0x7E it is returned unchanged */
char encrypt(char c){
  if( c>=0x1F && c<=0x7E)
    c++;
  return c;
}
/* Gets a char c and returns its decrypted form by reducing 1 from its value. If c is not between 0x21 and 0x7F it is returned unchanged */
char decrypt(char c){
  if( c>=0x21 && c<=0x7F)
    c--;
  return c;
}

/* xprt prints the value of c in a hexadecimal representation followed by a new line, and returns c unchanged. */
char xprt(char c){
  printf("%c in hexa: %x\n", c, c);
  return c;
}

/* dprt prints the value of c in a decimal representation followed by a new line, and returns c unchanged. */
char dprt(char c){
  printf("%c in decimal: %d\n", c, c);
  return c;
}

int menu(){
    char carray [MAX_LEN] = {'\0'};
    char input[MAX_LEN];
    FunDesc arr[] = {{"my_get", my_get},{"cprt", cprt},{"encrypt", encrypt},{"decrypt", decrypt},{"xprt", xprt},{"dprt", dprt},{NULL, NULL}};
    int bound = sizeof(arr) / sizeof(arr[0])-1;

    while(!feof(stdin)){

        fprintf(stdout,"Select operation from the following menu:\n");
        int num_input = 0;
        //printing the menu
        int i = 0;
        while (arr[i].name != NULL) {
            fprintf(stdout,"%d. %s\n", i , arr[i].name);
            i++;
        }

        fprintf(stdout,"option number: ");
        if (fgets(input, sizeof(input), stdin) == NULL) { //read the number
            return 0;
        }
        input[strcspn(input, "\n")] = '\0'; //remove the last char of the empty char
        num_input = strtol(input, NULL, 10); //convert to int
        if(num_input < 0 || num_input >= bound){ //check the number is ok
            printf("Not within bounds\n");
            return 0;
        }
    
        printf("Within bounds\n");

        char *curr = map(carray, sizeof(carray), arr[num_input].fun); //alocate a dynamic var
        
        
        if(curr !=NULL){
            memcpy(carray, curr, sizeof(carray));
            //  for(int j =0; j <sizeof(carray); j++)//FIXME:
            //     printf("carray %d: %c\n",j, carray[j]);//FIXME:
            free(curr);
        }

        printf("DONE.\n");
    }
    return 0;

}

int main(int argc, char **argv){ 
    menu();
    return 0 ; 
}
