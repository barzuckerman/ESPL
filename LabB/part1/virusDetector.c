#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define VIRUS_NAME 16
#define MAGIC_NUM 4
#define ENDIAN_L 0
#define ENDIAN_B 1
#define MAX_LEN 5
#define BUFFER_SIZE 10240
#define MIN(a, b) ((a) < (b) ? (a) : (b))

//-----------------------struct-----------------------
typedef struct virus
{
    unsigned short SigSize;
    char virusName[VIRUS_NAME];
    unsigned char *sig;
} virus;

typedef struct link link;
struct link
{
    link *nextVirus;
    virus *vir;
};

typedef struct fun_desc
{
    char *name;
    void (*fun)();
} FunDesc;

//-----------------------functions-----------------------
virus *readVirus(FILE *file);
void printVirus(virus *virus, FILE *output);
int magicNumber(FILE *file);
void list_print(link *virus_list, FILE *);
link *list_append(link *virus_list, virus *data);
void list_free(link *virus_list);
void loadSigatures();
void printSigatures();
void detectViruses();
void fixFile();
void quit();
void detect_virus(char *buffer, unsigned int size, link *virus_list);
void neutralize_virus(char *fileName, int signatureOffset);

//-----------------------vars-----------------------
FILE *file = NULL;
int endian = ENDIAN_L;
link *sigLinkList;
char buffer[BUFFER_SIZE];
//-----------------------main-----------------------
int main(int argc, char **argv)
{
    char input[MAX_LEN];
    FunDesc arr[] = {{"Load signatures", loadSigatures}, {"Print signatures", printSigatures}, {"Detect viruses", detectViruses}, {"Fix file", fixFile}, {"Quit", quit}, {NULL, NULL}};
    int bound = sizeof(arr) / sizeof(arr[0]) - 1;

    while (!feof(stdin))
    {

        fprintf(stdout, "Choose an option: \n");
        int num_input = 0;
        // printing the menu
        int i = 1;
        while (arr[i - 1].name != NULL)
        {
            fprintf(stdout, "%d) %s\n", i, arr[i - 1].name);
            i++;
        }

        fprintf(stdout, "option number: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        { // read the number
            return 0;
        }
        input[strcspn(input, "\n")] = '\0'; // remove the last char of the empty char
        if (sscanf(input, "%d", &num_input) != 1)
        {
            return 0; // If sscanf fails to parse an integer, exit
        } // convert to int
        if (num_input < 0 || num_input >= bound)
        { // check the number is ok
            printf("Not within bounds\n");
            return 0;
        }

        arr[num_input - 1].fun();

        printf("DONE.\n");
    }
    return 0;
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
    fprintf(output, "virus name: \n");
    for (int i = 0; i < VIRUS_NAME; i++)
    {
        if (virus->virusName[i] != 0)
        {
            fprintf(output, "%i ", virus->virusName[i]);
        }
    }
    fprintf(output, "\n");
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
    link *l = (link *)malloc(sizeof(link));
    if (!l)
    {
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
void list_free(link *virus_list)
{
    while (virus_list->nextVirus != NULL)
    {
        link *temp = virus_list;
        // free the virus data
        if (virus_list->vir->sig)
        {
            free(virus_list->vir->sig);
        }
        free(virus_list->vir);
        // free the virus link
        virus_list = virus_list->nextVirus;
        free(temp);
    }
    free(virus_list->vir);
    free(virus_list);
}

void loadSigatures()
{
    if (file)
    {
        list_free(sigLinkList);
        sigLinkList = NULL;
        fclose(file);
    }
    char filename[256];
    printf("Enter signature file name: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL)
    { // read the number
        return;
    }
    filename[strcspn(filename, "\n")] = '\0'; // remove the last char of the empty char

    file = fopen(filename, "rb"); // read from the file
    if (file == NULL)
    { // if the file was't opened successfully
        fprintf(stderr, "can't open this file \n");
        return;
    }
    if (magicNumber(file) == 0)
    {
        fprintf(stderr, "magic number is incorrect \n");
        fclose(file);
        return;
    }

    virus *v;
    while ((v = readVirus(file)) != NULL)
    {
        sigLinkList = list_append(sigLinkList, v);
    }
}

void printSigatures()
{
    if (sigLinkList != NULL)
    {
        list_print(sigLinkList, stdout);
    }
}

void detectViruses()
{
    char suspectedFileName[256];
    printf("Enter suspected file name: ");
    if (fgets(suspectedFileName, sizeof(suspectedFileName), stdin) == NULL)
    { // read the file name
        return;
    }
    suspectedFileName[strcspn(suspectedFileName, "\n")] = '\0'; // remove the last char

    FILE *suspectedFile = fopen(suspectedFileName, "rb"); // read from the file
    if (!suspectedFile)
    { // if the file was't opened successfully
        fprintf(stderr, "can't open this file \n");
        return;
    }
    size_t bytes = fread(buffer, 1, BUFFER_SIZE, suspectedFile);
    fclose(suspectedFile);
    detect_virus(buffer, MIN(bytes,BUFFER_SIZE), sigLinkList);
}

void detect_virus(char *buffer, unsigned int size, link *virus_list)
{
    link *currVirus = virus_list;
    while (currVirus != NULL)
    {
        unsigned short sigSize = currVirus->vir->SigSize;
        for (unsigned int i = 0; i <= size - sigSize; i++) {
            if (memcmp(&buffer[i], currVirus->vir->sig, sigSize) == 0) {
                printf("Virus detected!\n");
                printf("The starting byte location in the suspected file: %u\n", i);
                printf("The virus name: %s\n", currVirus->vir->virusName);
                printf("The size of the virus signature: %d\n", sigSize);
            }
        }
        currVirus = currVirus->nextVirus;

    }
}

void fixFile()
{
    int hasVirus = 0;
    char suspectedFileName[256];
    printf("Enter suspected file name: ");
    if (fgets(suspectedFileName, sizeof(suspectedFileName), stdin) == NULL)
    { // read the file name
        return;
    }
    suspectedFileName[strcspn(suspectedFileName, "\n")] = '\0'; // remove the last char

    FILE *suspectedFile = fopen(suspectedFileName, "rb"); // read from the file
    if (!suspectedFile)
    { // if the file was't opened successfully
        fprintf(stderr, "can't open this file \n");
        return;
    }
    size_t bytes = fread(buffer, 1, BUFFER_SIZE, suspectedFile);
    fclose(suspectedFile);

    link *currVirus = sigLinkList;
    while (currVirus != NULL)
    {
        unsigned short sigSize = currVirus->vir->SigSize;
        for (unsigned int i = 0; i <= MIN(bytes,BUFFER_SIZE) - sigSize; i++) {
            if (memcmp(&buffer[i], currVirus->vir->sig, sigSize) == 0) {
                if(hasVirus == 0)
                    hasVirus = 1;
                neutralize_virus(suspectedFileName, i);
                
            }
        }
        currVirus = currVirus->nextVirus;

    }
    if (hasVirus)
    {
        printf("Virus neutralized!\n");
    }
    else
    {
        printf("No virus detected!\n");
    }
    
}

void neutralize_virus(char *fileName, int signatureOffset){
    FILE *fileToRead = fopen(fileName, "rb+");
    if (fileToRead == NULL) {
        fprintf(stdout, "Failed to open file for neutralization");
        return;
    }

    if (fseek(fileToRead, signatureOffset, SEEK_SET) != 0) {
        fprintf(stdout, "Failed to seek in file");
        fclose(file);
        return;
    }

    unsigned char ret = 0xC3;  // RET instruction in x86
    if (fwrite(&ret, 1, 1, fileToRead) != 1) {
        fprintf(stdout, "Failed to write RET instruction");
    }

    fclose(fileToRead);
}

void quit()
{
    list_free(sigLinkList);
    sigLinkList = NULL;
    fclose(file);
    exit(0);
}
