#include <stdio.h>
#include <stdint.h>
#include <elf.h>

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg);
void print_phdr_info(Elf32_Phdr *phdr, int index);
const char *get_p_type_string(Elf32_Word type);

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
    printf("Type Offset VirtAddr PhysAddr FileSiz MemSiz Flg Align");
    printf("%-8s 0x%06x 0x%08x 0x%08x 0x%06x 0x%06x  ",
        get_p_type_string(phdr->p_type),
        phdr->p_offset,
        phdr->p_vaddr,
        phdr->p_paddr,
        phdr->p_filesz,
        phdr->p_memsz);

    
    // Process flags
    printf("%c%c%c 0x%08x\n",
           (phdr->p_flags & PF_R) ? 'R' : ' ',
           (phdr->p_flags & PF_W) ? 'W' : ' ',
           (phdr->p_flags & PF_X) ? 'E' : ' ',
           phdr->p_align);
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <elf-executable>\n", argv[0]);
        return 1;
    }
    foreach_phdr(map_start, print_phdr_info, 0);
    return 0;

}