section .data
    format db "%d", 0xA, 0
    format_len equ $ - format

section .text
extern printf
extern puts
global main
main:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]    ; argc
    dec eax            ; delete the first argument
    push eax
    push format
    call printf         ;print argc to stdout using printf
    add esp, 8          ;clean the stack
    mov eax, [ebp+12]   ; argv
    mov ebx, eax
    mov ecx, 1          ; ecx = counter
    loop:               ; inorder to print the args
        mov eax, [ebx+ecx*4]
        push ecx 
        push eax
        call puts       ;print argv[i] to stdout using puts, for all i from 0 to argc-1
        add esp, 4
        pop ecx
        inc ecx
        cmp ecx, [ebp+8]
        jl loop
    pop ebp
    ret
