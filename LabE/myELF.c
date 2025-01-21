#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

#define MAX_LEN 5
#define MAX_FILES 2
#define BUFFER_SIZE 1024

//-----------------------struct-----------------------

typedef struct fun_desc
{
    char *name;
    void (*fun)();
} FunDesc;


typedef struct{
    int fd;
    int size;
    void *map_start;
    char *mapped;
    char fileName[BUFFER_SIZE];
} ELF_File;

//-----------------------functions-----------------------
void ToggleDebugMode();
void examineELF();
void prinSectionNames();
void printSymbols();
void checkFileForMerge();
void mergeELFfiles();
void quit();

//-----------------------vars-----------------------
int debug_mode = 0;
ELF_File elf_files[MAX_FILES];
int file_count = 0;

//-----------------------main-----------------------
int main(int argc, char **argv)
{
    char input[MAX_LEN];
    FunDesc arr[] = {{"Toggle Debug Mode", ToggleDebugMode}, {"Examine ELF File", examineELF},{"Print Section Names", prinSectionNames} ,{"Print Symbols", printSymbols},{"Check Files for Merge", checkFileForMerge},{"Merge ELF Files", mergeELFfiles}, {"Quit", quit}, {NULL, NULL}};
    int bound = sizeof(arr) / sizeof(arr[0]) ;
    
    while (!feof(stdin))
    {
        if (debug_mode == 1)
        {
            fprintf(stderr, "Debug: file count: %d\n", file_count);
        }
        fprintf(stdout, "Choose action: \n");
        int num_input = 0;
        // printing the menu
        int i = 0;
        while (arr[i].name != NULL)
        {
            fprintf(stdout, "%d) %s\n", i, arr[i].name);
            i++;
        }

        fprintf(stdout, "option number: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        { // read the number
            quit();
            return 0;
        }
        input[strcspn(input, "\n")] = '\0'; // remove the last char of the empty char
        if (sscanf(input, "%d", &num_input) != 1)
        {
            quit();
            return 0; // If sscanf fails to parse an integer, exit
        } // convert to int
        if (num_input < 0 || num_input >= bound)
        { // check the number is ok
            printf("not in bounds\n");
        }
        else{
            arr[num_input].fun();
            printf("DONE.\n");
        }
    }
    return 0;
}

void ToggleDebugMode()
{
    if (debug_mode == 1)
    {
        debug_mode = 0;
        printf("Debug flag now off\n");
    }
    else
    {
        debug_mode = 1;
    printf("Debug flag now on\n");
    }
}

void examineELF(){
    if(file_count > 1){
        fprintf(stderr, "Can't open more than 3 files\n");
        return;
    }
    char filename[256];
    printf("Enter ELF filename: ");
    scanf("%s", filename);

    ELF_File *elf = &elf_files[file_count++];
    elf->fd = open(filename, O_RDONLY);
    if (elf->fd == -1){
        fprintf(stderr, "failed to open file");
        return;
    }
    struct stat st;
    if(fstat(elf->fd, &st) == -1){
        fprintf(stderr, "failed to get file size");
        elf->fd = -1;
        return;
    }
    elf->map_start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, elf->fd, 0);
    if(elf->map_start == MAP_FAILED){
        fprintf(stderr, "failed to map file");
        close(elf->fd);
        return;
    }
    
    Elf32_Ehdr *header = (Elf32_Ehdr *)elf->map_start;
    if (header->e_ident[0] != ELFMAG0 || header->e_ident[1] != ELFMAG1 ||
        header->e_ident[2] != ELFMAG2 || header->e_ident[3] != ELFMAG3) {
        fprintf(stderr, "Not a valid ELF file.\n");
        munmap(elf->map_start, elf->size);
        close(elf->fd);
        return;
    }
    //init the elf struct with the file info
    elf->size = st.st_size;
    strcpy(elf->fileName, filename);

    printf("Bytes 1,2,3 of the magic number: %c%c%c\n", header->e_ident[1], header->e_ident[2], header->e_ident[3]);
    printf("The data encoding scheme of the object file: %d\n", header->e_ident[5]);
    // printf("The data encoding scheme of the object file: %s\n", (header->e_ident[EI_DATA] == ELFDATA2LSB) ? "2's complement, little endian" : "2's complement, big endian");
    printf("Entry point: 0x%x\n", header->e_entry);
    printf("The file offset in which the section header table resides: %u\n", header->e_shoff);
    printf("The number of section header entries: %u\n", header->e_shnum);
    printf("The size of each section header entry: %u\n", header->e_shentsize);
    printf("The file offset in which the program header table resides: %u\n", header->e_phoff);
    printf("The number of program header entries: %u\n", header->e_phnum);
    printf("The size of each program header entry: %u\n", header->e_phentsize);

}

void prinSectionNames(){
    printf("Not implemented yet.\n");
}

void printSymbols(){
    printf("Not implemented yet.\n");
}

void checkFileForMerge(){
    printf("Not implemented yet.\n");
}

void mergeELFfiles(){
    printf("Not implemented yet.\n");
}

void quit(){
    for (int i = 0; i < file_count; i++)
    {
        munmap(elf_files[i].map_start, elf_files[i].size);
        close(elf_files[i].fd);
    }
    if (debug_mode==1)
    {
        fprintf(stderr,"Debug:quiting\n");
    }
    exit(0);
}