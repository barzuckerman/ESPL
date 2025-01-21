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
void printSectionNames();
void printSymbols();
void checkMerge();
void mergeELFfiles();
void quit();
const char* sectionTypeToString(unsigned int type);
void scanSymbols(ELF_File *file1, ELF_File *file2);
int assertOneSymbolTable(Elf32_Ehdr *header1, Elf32_Ehdr *header2, Elf32_Shdr *sectionHeaders1, Elf32_Shdr *sectionHeaders2);
Elf32_Shdr* getSymbolTable(Elf32_Ehdr *header, Elf32_Shdr *sectionHeaders);
void mergeELFfiles();
void filePrintSymbols(ELF_File file);

//-----------------------vars-----------------------
int debug_mode = 1;
ELF_File elf_files[MAX_FILES];
int file_count = 0;

//-----------------------main-----------------------
int main(int argc, char **argv)
{
    char input[MAX_LEN];
    FunDesc arr[] = {{"Toggle Debug Mode", ToggleDebugMode}, {"Examine ELF File", examineELF},{"Print Section Names", printSectionNames} ,{"Print Symbols", printSymbols},{"Check Files for Merge", checkMerge},{"Merge ELF Files", mergeELFfiles}, {"Quit", quit}, {NULL, NULL}};
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
    if (fgets(filename, sizeof(filename), stdin) != NULL) {
        filename[strcspn(filename, "\n")] = '\0'; // Remove newline
    }
    ELF_File *elf = &elf_files[file_count];
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
        elf->fd = -1;
        return;
    }
    
    Elf32_Ehdr *header = (Elf32_Ehdr *)elf->map_start;
    if (header->e_ident[0] != ELFMAG0 || header->e_ident[1] != ELFMAG1 ||
        header->e_ident[2] != ELFMAG2 || header->e_ident[3] != ELFMAG3) {
        fprintf(stderr, "Not a valid ELF file.\n");
        munmap(elf->map_start, elf->size);
        close(elf->fd);
        elf->fd = -1;
        return;
    }
    //init the elf struct with the file info
    elf->size = st.st_size;
    strcpy(elf->fileName, filename);

    printf("\nBytes 1,2,3 of the magic number: %c%c%c\n", header->e_ident[1], header->e_ident[2], header->e_ident[3]);
    printf("The data encoding scheme of the object file: %s\n", (header->e_ident[5] == ELFDATA2LSB) ? "little endian" : "big endian");
    printf("Entry point: 0x%x\n", header->e_entry);
    printf("The file offset in which the curSection header table resides: %u\n", header->e_shoff);
    printf("The number of curSection header entries: %u\n", header->e_shnum);
    printf("The size of each curSection header entry: %u\n", header->e_shentsize);
    printf("The file offset in which the program header table resides: %u\n", header->e_phoff);
    printf("The number of program header entries: %u\n", header->e_phnum);
    printf("The size of each program header entry: %u\n\n", header->e_phentsize);
    file_count++;

}

void printSectionNames(){
    if (file_count == 0){
        fprintf(stderr, "No file to print curSection names\n");
        return;
    }
    
    for (int i = 0; i < file_count; i++){
        if (elf_files[i].fd == -1){
            fprintf(stderr, "File %s not found\n", elf_files[i].fileName);
            continue;
        }
        printf("\nFile: %s\n", elf_files[i].fileName);
        printf("[index]  section_name           section_address        section_offset  section_size  section_type\n");

        Elf32_Ehdr *header = (Elf32_Ehdr *)elf_files[i].map_start;
        if(header->e_shoff != 0){   // the file has curSection header table.
            Elf32_Shdr *sectionHeader = (Elf32_Shdr*)(elf_files[i].map_start + header->e_shoff);
            Elf32_Shdr *stSectionHeader = (Elf32_Shdr*)(sectionHeader + header->e_shstrndx); 
            char* stringTable = (char*)(elf_files[i].map_start + stSectionHeader->sh_offset);

            for(size_t j = 0; j < header->e_shnum; j++){
                Elf32_Shdr* curSection = (Elf32_Shdr*)(sectionHeader + j);
                printf("[%02d]     %-20s   %08x               %06x         %06x       %s\n", j, (char*)(stringTable + curSection->sh_name), curSection->sh_addr, curSection->sh_offset, curSection->sh_size, sectionTypeToString(curSection->sh_type));
            }
            if(debug_mode){
                fprintf(stderr, "\n----DEBUG: The file contains %d curSection headers, beginning at offset 0x%x\n", header->e_shnum, header->e_shoff);
                fprintf(stderr, "----DEBUG: The string table is located at index %d in the curSection header table, starting at address %p\n", header->e_shstrndx, stSectionHeader);
                fprintf(stderr, "----DEBUG: The string table begins at offset 0x%x\n", stSectionHeader->sh_offset);
            }        
        }
    }
}

const char* sectionTypeToString(unsigned int type) {
    switch (type) {
        case SHT_NULL: return "NULL";
        case SHT_PROGBITS: return "PROGBITS";
        case SHT_SYMTAB: return "SYMTAB";
        case SHT_STRTAB: return "STRTAB";
        case SHT_RELA: return "RELA";
        case SHT_HASH: return "HASH";
        case SHT_DYNAMIC: return "DYNAMIC";
        case SHT_NOTE: return "NOTE";
        case SHT_NOBITS: return "NOBITS";
        case SHT_REL: return "REL";
        case SHT_SHLIB: return "SHLIB";
        case SHT_DYNSYM: return "DYNSYM";
        case 0x6FFFFFFF: return "VERSYM";
        case 0x6FFFFFFE: return "VERNEED";
        default: return "UNKNOWN";
    }
}


void printSymbols() {
    if (file_count == 0) {
        fprintf(stderr, "No file to print symbols from\n");
        return;
    }

    for (int i = 0; i < file_count; i++) {
        if (elf_files[i].fd == -1) {
            fprintf(stderr, "File %s not found\n", elf_files[i].fileName);
            continue;
        }

        ELF_File *elf = &elf_files[i];
        Elf32_Ehdr *header = (Elf32_Ehdr *)elf->map_start;

        // Locate the symbol table curSection and the string table curSection
        Elf32_Shdr *sectionHeaders = (Elf32_Shdr *)(elf->map_start + header->e_shoff);
        Elf32_Shdr *stringTableSectionHeaders = (Elf32_Shdr*)(sectionHeaders + header->e_shstrndx);
        char *stringTable = (char *)(elf->map_start + stringTableSectionHeaders->sh_offset);

        // Find the symbol table and string table
        for (size_t j = 0; j < header->e_shnum; j++) {
            Elf32_Shdr *curSection = (Elf32_Shdr*)(sectionHeaders + j);

            if (curSection->sh_type == SHT_SYMTAB || curSection->sh_type == SHT_DYNSYM) {
                // This curSection is the symbol table
                printf("\nSymbols in file: %s\n", elf->fileName);
                printf("[index] name                    value      size    type    bind    shndx\n");
                
                // Iterate through the symbols in the symbol table
                char *symbolsStringTable = (char*)(elf->map_start + (sectionHeaders + curSection->sh_link)->sh_offset);
                Elf32_Sym *symbols = (Elf32_Sym *)(elf->map_start + curSection->sh_offset);
                size_t numSymbols = curSection->sh_size / curSection->sh_entsize;
                
                for (size_t k = 0; k < numSymbols; k++) {
                    Elf32_Sym *symbol = (Elf32_Sym*)(symbols + k);
                    const char *symbolName = "";

                    // Determine the symbol name based on its st_name field
                    if (symbol->st_name) {
                        symbolName = (char*)(symbolsStringTable + symbol->st_name);
                    } else if (symbol->st_shndx < header->e_shnum) {
                        // If st_shndx is valid, get the name from section header
                        symbolName = (char*)(stringTable + ((Elf32_Shdr*)(sectionHeaders + symbol->st_shndx))->sh_name);
                    }

                    printf("[%02zu] %-20s          %08x      %06u    %s    %s    %d\n",
                           k,
                           symbolName,
                           symbol->st_value,
                           symbol->st_size,
                           (ELF32_ST_TYPE(symbol->st_info) == STT_FUNC) ? "FUNC" : 
                           (ELF32_ST_TYPE(symbol->st_info) == STT_OBJECT) ? "OBJECT" : "OTHER",
                           (ELF32_ST_BIND(symbol->st_info) == STB_GLOBAL) ? "GLOBAL" :
                           (ELF32_ST_BIND(symbol->st_info) == STB_LOCAL) ? "LOCAL" : "OTHER",
                           symbol->st_shndx);
                }
                if(debug_mode){
                    fprintf(stderr,"\n----DEBUG: Size of the symbol table: 0x%x bytes\n", curSection->sh_size);
                    fprintf(stderr,"----DEBUG: Total number of symbols: %d\n", numSymbols);
                    fprintf(stderr,"----DEBUG: Symbol table name: %s\n", (char*)(stringTable + curSection->sh_name));
                }
            }
        }

        if (stringTable == NULL) {
            fprintf(stderr, "No symbol table found in file: %s\n", elf->fileName);
        }
    }
}

void checkMerge() {
    if (file_count != 2) {
        fprintf(stderr, "Error: Exactly 2 ELF files must be opened for merging.\n");
        return;
    }

    scanSymbols(&elf_files[0], &elf_files[1]);
    scanSymbols(&elf_files[1], &elf_files[0]);
}

void scanSymbols(ELF_File *file1, ELF_File *file2){
    // Extract the ELF headers
    Elf32_Ehdr *header1 = (Elf32_Ehdr *)file1->map_start;
    Elf32_Ehdr *header2 = (Elf32_Ehdr *)file2->map_start;

    // Extract the ELF sectionHeaders
    Elf32_Shdr *sectionHeaders1 = (Elf32_Shdr *)(file1->map_start + header1->e_shoff);
    Elf32_Shdr *sectionHeaders2 = (Elf32_Shdr *)(file2->map_start + header2->e_shoff);
    
    // Check for exactly one symbol table in each
    if (assertOneSymbolTable(header1, header2, sectionHeaders1, sectionHeaders2) == -1) {
        return;
    }

    Elf32_Shdr *symtab1 = getSymbolTable(header1, sectionHeaders1);
    Elf32_Shdr *symtab2 = getSymbolTable(header2, sectionHeaders2);

    char *symStringTab1 = (char*)(file1->map_start + (sectionHeaders1 + symtab1->sh_link)->sh_offset);
    char *symStringTab2 = (char*)(file2->map_start + (sectionHeaders2 + symtab2->sh_link)->sh_offset);

    // Check symbols in SYMTAB1 against SYMTAB2
    Elf32_Sym *symbols1 = (Elf32_Sym *)(file1->map_start + symtab1->sh_offset);
    Elf32_Sym *symbols2 = (Elf32_Sym *)(file2->map_start + symtab2->sh_offset);
    size_t numSymbols1 = symtab1->sh_size / sizeof(Elf32_Sym);
    size_t numSymbols2 = symtab2->sh_size / sizeof(Elf32_Sym);

    // Create a list or set to store names in the second symbol table for quick lookups
    for (size_t i = 1; i < numSymbols1; i++) { // Start from 1 to skip the dummy null symbol
        Elf32_Sym *sym1 = (Elf32_Sym*)(symbols1 + i);
        char *symbolName1 = (char*)(symStringTab1 + sym1->st_name);
        if(strcmp(symbolName1, "") != 0){
            // Search for the symbol in the second symbol table

            int foundInFile2 = 0;
            Elf32_Sym *sym2;
            char *symbolName2;
            for (size_t j = 1; j < numSymbols2; j++) { // Start from 1 to skip the dummy null symbol
                sym2 = (Elf32_Sym*)(symbols2 + j);
                symbolName2 = (char*)(symStringTab2 + sym2->st_name);

                if (strcmp(symbolName1, symbolName2) == 0) {
                    foundInFile2 = 1; // Symbol found in file2
                    break;
                }
                symbolName2 = SHN_UNDEF;
            }

            // Check the symbol's definition status
            if (sym1->st_shndx == SHN_UNDEF) {
                if (!foundInFile2 || (symbolName2 && foundInFile2 && sym2->st_shndx == SHN_UNDEF)) {
                    fprintf(stderr, "Symbol '%s' undefined.\n", symbolName1);
                }
                
            } else {
                if (symbolName2 && foundInFile2 && sym2->st_shndx != SHN_UNDEF) {
                    fprintf(stderr, "Symbol '%s' multiply defined.\n", symbolName1);
                }
            }

            // If sym is UNDEFINED in SYMTAB1, and either not found in SYMTAB2 or found but UNDEFINED there as well, print an error message: "Symbol sym undefined".
            // If sym is defined in SYMTAB1, i.e. has a valid section number, and also defined in SYMTAB2, print an error message: "Symbol sym multiply defined".
        }
    }
}

int assertOneSymbolTable(Elf32_Ehdr *header1, Elf32_Ehdr *header2, Elf32_Shdr *sectionHeaders1, Elf32_Shdr *sectionHeaders2){
    int symtabCount1 = 0, symtabCount2 = 0;

    // Count SYMTAB sections
    for (size_t i = 0; i < header1->e_shnum; i++) {
        if (sectionHeaders1[i].sh_type == SHT_SYMTAB) {
            symtabCount1++;
        }
    }
    
    for (size_t i = 0; i < header2->e_shnum; i++) {
        if (sectionHeaders2[i].sh_type == SHT_SYMTAB) {
            symtabCount2++;
        }
    }

    // Validate that both files contain exactly one symbol table
    if (symtabCount1 != 1 || symtabCount2 != 1) {
        fprintf(stderr, "Feature not supported: Each file must contain exactly one symbol table.\n");
        return -1;
    }
    else {
        return 0;
    }
}

Elf32_Shdr* getSymbolTable(Elf32_Ehdr *header, Elf32_Shdr *sectionHeaders) {
    for (size_t i = 0; i < header->e_shnum; i++) {
        if (sectionHeaders[i].sh_type == SHT_SYMTAB) {
            return &sectionHeaders[i];
        }
    }
    return NULL;
}

void mergeELFfiles(){
    ELF_File *file1 = &elf_files[0];
    ELF_File *file2 = &elf_files[1];

    // Extract the ELF headers
    Elf32_Ehdr *header1 = (Elf32_Ehdr *)file1->map_start;
    Elf32_Ehdr *header2 = (Elf32_Ehdr *)file2->map_start;

    // Extract the ELF section headers
    Elf32_Shdr *sectionHeaders1 = (Elf32_Shdr *)(file1->map_start + header1->e_shoff);
    Elf32_Shdr *sectionHeaders2 = (Elf32_Shdr *)(file2->map_start + header2->e_shoff);

    // Create "out.ro" and copy an initial version of the ELF header as its header.
    FILE *outputFile = fopen("out.ro", "wb");
    if (!outputFile) {
        perror("Failed to create output file");
        return;
    }

    // Write ELF header for the output file
    Elf32_Ehdr newHeader = *header1; // Copy the header of the first file
    newHeader.e_shoff = sizeof(newHeader); // Will update this later

    fwrite(&newHeader, sizeof(newHeader), 1, outputFile);

    // Temporary storage for new section headers
    Elf32_Shdr newSectionHeaders[header1->e_shnum];
    size_t offset = sizeof(newHeader); // Start writing sections after the header

    // Loop over the sections for copying
    for (size_t i = 0; i < header1->e_shnum; i++) {
        Elf32_Shdr *curSection1 = &sectionHeaders1[i];
        // Prepare new section header
        newSectionHeaders[i] = *curSection1; // Copy section header
        char *sectionName1 = (char*)(file1->map_start + sectionHeaders1[header1->e_shstrndx].sh_offset + curSection1->sh_name);
        // Handle relevant sections
        if (strcmp(".text", sectionName1) == 0 || strcmp(".data", sectionName1) == 0 || strcmp(".rodata", sectionName1) == 0) {
            // Copy section contents from the first ELF file
            fwrite((void *)(file1->map_start + curSection1->sh_offset), 1, curSection1->sh_size, outputFile);
            newSectionHeaders[i].sh_offset = offset; // Update section offset
            offset += curSection1->sh_size; // Increment offset for next section

            // Now also get the corresponding section from the second ELF file
            for (size_t j = 0; j < header2->e_shnum; j++) {
                Elf32_Shdr *curSection2 = &sectionHeaders2[j];
                char *sectionName2 = (char*)(file1->map_start + sectionHeaders2[header2->e_shstrndx].sh_offset + curSection2->sh_name);
                // Only copy the section if it matches
                if (strcmp(".text", sectionName2) == 0 || strcmp(".data", sectionName2) == 0 || strcmp(".rodata", sectionName2) == 0) {
                    fwrite((void *)(file2->map_start + curSection2->sh_offset), 1, curSection2->sh_size, outputFile);
                    offset += curSection2->sh_size; // Increment offset for the appended content
                    break;
                }
            }
        } else if (strcmp(".shstrtab", sectionName1) == 0) {
            // Copy the string table as-is
            fwrite((void *)(file1->map_start + curSection1->sh_offset), 1, curSection1->sh_size, outputFile);
            newSectionHeaders[i].sh_offset = offset; // Where this is located
            offset += curSection1->sh_size; // Increment offset
        } else if (strcmp(".symtab", sectionName1) == 0) {
            // Copy the symbol table
            fwrite((void *)(file1->map_start + curSection1->sh_offset), 1, curSection1->sh_size, outputFile);
            newSectionHeaders[i].sh_offset = offset; // Where this is located
            offset += curSection1->sh_size; // Increment offset
        } else {
            // For other sections, simply copy; handle according to later parts if needed
            newSectionHeaders[i].sh_offset = curSection1->sh_offset; 
        }
    }

    // Write new section header table to outputFile
    fseek(outputFile, sizeof(newHeader), SEEK_SET);
    fwrite(newSectionHeaders, sizeof(newSectionHeaders[0]), header1->e_shnum, outputFile);

    // Update the e_shoff field in the ELF header to point to the section header table
    newHeader.e_shoff = sizeof(newHeader) + sizeof(newSectionHeaders[0]) * header1->e_shnum;

    // Now write back updated ELF header
    fseek(outputFile, 0, SEEK_SET);
    fwrite(&newHeader, sizeof(newHeader), 1, outputFile);

    // Close the output file
    fclose(outputFile);
    printf(" ELF files merged successfully into 'out.ro'.\n");
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