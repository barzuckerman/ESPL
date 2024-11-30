#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define VIRUS_NAME 16
#define MAGIC_NUM 4
#define ENDIAN_L 0
#define ENDIAN_B 1

typedef struct link
{
    link *nextVirus;
    virus *vir;
} link;

typedef struct virus
{
    unsigned short SigSize;
    char virusName[VIRUS_NAME];
    unsigned char *sig;
} virus;

virus *readVirus(FILE *file);
void printVirus(virus *virus, FILE *output);
int magicNumber(FILE *file);
int endian = ENDIAN_L;

void list_print(link *virus_list, FILE *);
/* Print the data of every link in list to the given stream. Each item followed by a newline character. */
link *list_append(link *virus_list, virus *data);
/* Add a new link with the given data to the list (at the end CAN ALSO AT BEGINNING), and return a pointer to the list (i.e., the first link in the list). If the list is null - create a new entry and return a pointer to the entry. */
void list_free(link *virus_list);
/* Free the memory allocated by the list. */

int main(int argc, char **argv)
{
    FILE *file = fopen(argv[1], "rb"); // read from the file
    if (file == NULL)
    { // if the file was't opened successfully
        fprintf(stderr, "can't open this file \n");
        return 0;
    }
    if (magicNumber(file) == 0)
    {
        fprintf(stderr, "magic number is incorrect \n");
        fclose(file);
        return 0;
    }

    virus *v;
    FILE *out = fopen("out.txt", "w"); /// read from the file
    if (out == NULL)
    { // if the file was't opened successfully
        fprintf(stderr, "can't open a file \n");
    }
    while ((v = readVirus(file)) != NULL)
    {

        printVirus(v, out);
        // Clean up the allocated virus structure
        if (v->sig)
        {
            free(v->sig);
        }
        free(v);
    }

    fclose(file);
    return 1;
}

// this function receives a file pointer and returns a virus* that represents the next virus in the file
virus *readVirus(FILE *file)
{
    virus *v = (virus *)malloc(sizeof(virus));
    if (!v)
    {
        perror("Failed to allocate memory for virus");
        return NULL;
    }

    memset(v->virusName, 0, VIRUS_NAME); // Ensure zero-ed for safety

    if (fread(v, sizeof(unsigned char), sizeof(unsigned short) + VIRUS_NAME, file) != sizeof(unsigned short) + VIRUS_NAME)
    {
        free(v);
        return NULL;
    }

    if (endian == ENDIAN_B)
        v->SigSize = ((v->SigSize) >> 8) | ((v->SigSize) << 8);

    if (v->SigSize > 0)
    {
        v->sig = (unsigned char *)malloc(v->SigSize);
        if (!v->sig)
        {
            perror("Failed to allocate memory for signature");
            free(v);
            return NULL;
        }
        if (fread(v->sig, sizeof(unsigned char), v->SigSize, file) != v->SigSize)
        {
            free(v->sig);
            free(v);
            return NULL;
        }
    }
    else
    {
        v->sig = NULL;
    }

    return v;
}

// this function receives a virus and a pointer to an output file. The function prints the virus to the given output. It prints the virus name (in ASCII), the virus signature length (in decimal), and the virus signature (in hexadecimal representation).
void printVirus(virus *virus, FILE *output)
{
    fprintf(output, "virus name: ");
    for(int i = 0; i<VIRUS_NAME; i++){
        if(virus->virusName[i] !=0){
            fprintf(output, "%i ", virus->virusName[i]);
        }
    }
    // fprintf(output, "virus name: %s", virus->virusName);
    // fprintf(output, "\n");
    fprintf(output, "virus length: %d\n", virus->SigSize);
    fprintf(output, "virus signature: \n");
    for (int i = 0; i < virus->SigSize; i++)
    {
        fprintf(output, "%02X ", virus->sig[i]); // Print each byte in hexadecimal
    }
    fprintf(output, "\n");
    fprintf(output, "\n");
}

int magicNumber(FILE *file)
{
    char magic[MAGIC_NUM + 1] = {0}; // Include space for null terminator
    if (fread(magic, sizeof(char), MAGIC_NUM, file) != MAGIC_NUM)
    {
        return 0;
    }
    if (strcmp(magic, "VIRL") == 0)
    {
        endian = ENDIAN_L;
        return 1;
    }
    else if (strcmp(magic, "VIRB") == 0)
    {
        endian = ENDIAN_B;
        return 1;
    }
    return 0;
}

// Print the data of every link in list to the given stream. Each item followed by a newline character.
void list_print(link *virus_list, FILE *output)
{
    link *curr = virus_list;
    while (curr != NULL)
    {
        printVirus(curr->vir, output);
        curr = curr->nextVirus;
    }
}

// Add a new link with the given data to the list (at the end CAN ALSO AT BEGINNING), and return a pointer to the list (i.e., the first link in the list). If the list is null - create a new entry and return a pointer to the entry.
link *list_append(link *virus_list, virus *data)
{
    link *l = (link*)malloc(sizeof(link));
    if (!l) {
        perror("Failed to allocate memory for new list node");
        exit(EXIT_FAILURE);
    }
    if (virus_list == NULL)
    {
        l->nextVirus = NULL;
        l->vir = data;
    }
    else
    {
        l->nextVirus = virus_list;
        l->vir = data;
    }
    return l;
}

// Free the memory allocated by the list.
void list_free(link *virus_list){
    while(virus_list->nextVirus != NULL){
        link *temp = virus_list;
        free(virus_list->vir);
        virus_list = virus_list->nextVirus;
        free(temp);
    }
    free(virus_list->vir);
    free(virus_list);
}
