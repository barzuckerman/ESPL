section .data
    x_struct: 
        db 5                              ; size (5 bytes in num array)
    x_num: 
        db 0xaa, 1, 2, 0x44, 0x4f         ; num array in little-endian
    format db "%02hhx", 0                 ; Format string for printf

section .text
extern printf                             ; Declare external printf function
global print_multi                        ; Export the print_multi function
global main                               ; Export main

; Main function
main:
    push ebp                              ; Save base pointer
    mov ebp, esp                          ; Set base pointer

    ; Call print_multi(&x_struct)
    lea eax, [x_struct]                   ; Load address of x_struct
    push eax                              ; Push pointer to x_struct onto the stack
    call print_multi                      ; Call print_multi
    add esp, 4                            ; Clean up stack (1 argument)

    ; Exit main
    mov esp, ebp                          ; Restore stack pointer
    pop ebp                               ; Restore base pointer
    mov eax, 0                            ; Return 0
    ret                                   ; Exit program

; void print_multi(struct multi *p)
print_multi:
    push ebp                              ; Save base pointer
    mov ebp, esp                          ; Set base pointer
    push esi                              ; Preserve esi
    push edi                              ; Preserve edi
    push ebx                              ; Preserve ebx

    ; Load struct multi *p
    mov esi, [ebp + 8]                    ; esi = p (pointer to struct)
    movzx ecx, byte [esi]                 ; ecx = p->size (size is unsigned char)
    lea edi, [esi + 1]                    ; edi = p->num (address of num array, skipping size byte)

    ; Loop through the num array
    xor ebx, ebx                          ; ebx = i = 0 (counter)
print_loop:
    cmp ebx, ecx                          ; Compare i (ebx) with size (ecx)
    jge end_loop                          ; Exit loop if i >= size

    ; Print num[i]
    movzx eax, byte [edi + ebx]           ; Load num[i] into eax (zero-extended)
    push eax                              ; Push num[i] onto the stack
    push format                           ; Push format string onto the stack
    call printf                           ; Call printf("%02hhx")
    add esp, 8                            ; Clean up stack (2 arguments)

    inc ebx                               ; i++
    jmp print_loop                        ; Repeat loop

end_loop:
    ; Epilogue
    pop ebx                               ; Restore ebx
    pop edi                               ; Restore edi
    pop esi                               ; Restore esi
    mov esp, ebp                          ; Restore stack pointer
    pop ebp                               ; Restore base pointer
    ret                                   ; Return to caller
