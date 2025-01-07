section .bss
    buffer resb 100                 ; Reserve 100 bytes for the buffer

section .data
    buffer_size equ 100             ; Define buffer size
    msg db "Enter a string: ", 0    ; Message to prompt user

section .text
    extern fgets                    ; Declare fgets function
    extern puts                     ; Declare puts function
    extern stdin                    ; Declare stdin (C runtime variable)
    global main                     ; Entry point

main:
    ; Print the message "Enter a string: "
    push msg                        ; Push the message address
    call puts                       ; Call puts
    add esp, 4                      ; Clean up stack (1 argument)

    ; Call fgets to read input
    lea eax, [buffer]               ; Load address of buffer into eax
    push dword [stdin]              ; Push the stdin pointer from C runtime
    push buffer_size                ; Push buffer size (100)
    push eax                        ; Push buffer pointer
    call fgets                      ; Call fgets
    add esp, 12                     ; Clean up stack (3 arguments)

    ; Print the read string
    push buffer                     ; Push the buffer containing the input
    call puts                       ; Call puts to print the string
    add esp, 4                      ; Clean up stack (1 argument)

    ; Exit gracefully
    xor eax, eax                    ; Clear eax
    ret
print_multi:
    push ebp                              ; Save base pointer
    mov ebp, esp                          ; Set base pointer
    push esi                              ; Preserve esi
    push edi                              ; Preserve edi
    push ebx                              ; Preserve ebx

    ; Load struct multi *p
    mov esi, [ebp + 8]                    ; esi = p (pointer to struct)
    movzx ecx, word [esi]                 ; ecx = p->size (size is unsigned char)
    lea edi, [esi + 2]                    ; edi = p->num (address of num array, skipping size byte)

    ; Loop through the num array in reverse order
    dec ecx                               ; ecx = p->size - 1 (last index)
loop_print:
    cmp ecx, -1                           ; Compare ecx with -1
    je end_loop                           ; Exit loop if ecx == 0

    ; Print num[ecx]
    movzx eax, word [edi + ecx*2]           ; Load num[ecx] into eax (zero-extended)
    push ecx
    push eax                              ; Push num[ecx] onto the stack
    push format                           ; Push format string onto the stack
    call printf                           ; Call printf("%02hhx")
    add esp, 8                            ; Clean up stack (2 arguments)
    pop ecx                               ; Restore ecx
    dec ecx                               ; ecx--
    jmp loop_print                      ; Repeat loop