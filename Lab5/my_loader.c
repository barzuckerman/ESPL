#include <stdio.h>
#include <stdint.h>
#include <elf.h>
#include <sys/mman.h>
#include <bits/mman-map-flags-generic.h>


int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg);
void print_phdr_info(Elf32_Phdr *phdr, int index);
const char *get_p_type_string(Elf32_Word type);
void print_mmap_prot_flags(Elf32_Phdr *phdr, int arg);
Elf32_Word checkMappingFlags(Elf32_Word flag);
int checkProtectionFlags(Elf32_Word flag);
void load_phdr(Elf32_Phdr *phdr, int fd);
int startup(int argc, char **argv, void (*start)());


int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg){
    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *program_header;
    for (int i = 0; i < elf_header->e_phnum; i++) {
        program_header = (Elf32_Phdr *)(map_start + elf_header->e_phoff + i * sizeof(Elf32_Phdr));
        func(program_header, arg);
    }
    return 0;
}

// Function to convert type to string
const char *get_p_type_string(Elf32_Word type) {
    switch (type) {
        case PT_PHDR:   return "PHDR";
        case PT_INTERP: return "INTERP";
        case PT_LOAD:   return "LOAD";
        case PT_DYNAMIC:return "DYNAMIC";
        case PT_NOTE:   return "NOTE";
        case PT_SHLIB:  return "SHLIB";
        case PT_NULL:   return "NULL";
        default:        return "UNKNOWN";
    }
}

void print_phdr_info(Elf32_Phdr *phdr, int index) {
    
    printf("%-8s 0x%06x 0x%08x 0x%08x 0x%06x 0x%06x ",
        get_p_type_string(phdr->p_type),
        phdr->p_offset,
        phdr->p_vaddr,
        phdr->p_paddr,
        phdr->p_filesz,
        phdr->p_memsz);

    
    // Process flags
    printf("%c%c%c 0x%08x 0x%05x  0x%x\n",
           (phdr->p_flags & PF_R) ? 'R' : ' ',
           (phdr->p_flags & PF_W) ? 'W' : ' ',
           (phdr->p_flags & PF_X) ? 'E' : ' ',
           phdr->p_align, checkProtectionFlags(phdr->p_flags), checkMappingFlags(phdr->p_flags));

    
}


void print_mmap_prot_flags(Elf32_Phdr *phdr, int arg)
{
    int prot = 0;
    if (phdr->p_flags & PF_R)
        prot |= PROT_READ;
    if (phdr->p_flags & PF_W)
        prot |= PROT_WRITE;
    if (phdr->p_flags & PF_X)
        prot |= PROT_EXEC;

    printf("0x%x\n", prot);
}

Elf32_Word checkMappingFlags(Elf32_Word flag){  
    switch (flag)
    {
    case 0x1:       // executable
        return PROT_EXEC;
    case 0x2:       // writable
        return MAP_SHARED;
    case 0x3:       // writable | executable
        return MAP_SHARED;
    case 0x4:       // readable
        return MAP_PRIVATE;
    case 0x5:       // readable | executable
        return MAP_PRIVATE;
    case 0x6:       // readable | writable
        return MAP_SHARED;
    case 0x7:       // readable | writable | executable
        return MAP_SHARED; 
    default:
        return 0;
        break;
    }
}

int checkProtectionFlags(Elf32_Word flag){
    switch (flag)
    {
    case 0x1:       // executable
        return PROT_EXEC;
    case 0x2:       // writable
        return PROT_WRITE;
    case 0x3:       // writable | executable
        return PROT_WRITE | PROT_EXEC;
    case 0x4:       // readable
        return PROT_READ;
    case 0x5:       // readable | executable
        return PROT_READ | PROT_EXEC;
    case 0x6:       // readable | writable
        return PROT_READ | PROT_WRITE;
    case 0x7:       // readable | writable | executable
        return PROT_READ | PROT_WRITE | PROT_EXEC; 
    default:
        return PROT_NONE;
    }
}

void load_phdr(Elf32_Phdr *phdr, int fd){
    if (phdr->p_type != PT_LOAD) {
        return;  // Only process LOAD segments
    }

    void* vaddr = (void*)(phdr->p_vaddr & 0xfffff000);
    int offset = phdr->p_offset & 0xfffff000;
    int padding = phdr->p_vaddr & 0xfff;
    size_t memszTotal = phdr->p_memsz + padding;
    void* elfFileMaped = mmap(vaddr, memszTotal, checkProtectionFlags(phdr->p_flags), MAP_PRIVATE | MAP_FIXED , fd, offset); 
    if(elfFileMaped == MAP_FAILED){
        perror("Error");
        return;
    }
    else{
        print_phdr_info(phdr, 0);
    }
}


int main(int argc, char *argv[]) {

    if (argc < 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        perror("fopen");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);

    void *map_start = mmap(NULL, filesize, PROT_READ | PROT_EXEC | PROT_WRITE, MAP_PRIVATE, fileno(file), 0);
    printf("Type     Offset   VirtAddr   PhysAddr   FileSiz  MemSiz   Flg    Align   ProtFlag mapFlag\n");
    foreach_phdr(map_start, load_phdr, fileno(file));

    // Get the ELF header to find the entry point
    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)map_start;

    startup(argc-1, argv+1, (void *)(elf_header->e_entry));


    fclose(file);
    return 0;

}