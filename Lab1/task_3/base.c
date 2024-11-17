#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int menu();
char my_get(char c);
char cprt(char c);
char encrypt(char c);
char decrypt(char c);
char xprt(char c);
char dprt(char c);
char* map(char *array, int array_length, char (*f) (char));

char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  /* TODO: Complete during task 2.a */
  for(int i = 0 ; i<array_length ; i++){
    mapped_array[i] = f(array[i]);
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
    printf(".");
  return c;
}
/* Gets a char c and returns its encrypted form by adding 1 to its value. If c is not between 0x1F and 0x7E it is returned unchanged */
char encrypt(char c){
  if( c>=0x1F && c<=0x7E)
    c = c + 1;
  return c;
}
/* Gets a char c and returns its decrypted form by reducing 1 from its value. If c is not between 0x21 and 0x7F it is returned unchanged */
char decrypt(char c){
  if( c>=0x21 && c<=0x7F)
    c = c - 1;
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

int main(int argc, char **argv){
  /* TODO: Test your code */
  // int base_len = 5;
  // char arr1[base_len];
  // char* arr2 = map(arr1, base_len, my_get);
  // char* arr3 = map(arr2, base_len, dprt);
  // char* arr4 = map(arr3, base_len, xprt);
  // char* arr5 = map(arr4, base_len, encrypt);
  // free(arr2);
  // free(arr3);
  // free(arr4);
  // free(arr5);
  return menu();
}
