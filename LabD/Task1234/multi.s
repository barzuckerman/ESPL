section .data
    ;x_struct: 
        ;dw 5                              ; size (5 bytes in num array)
    ;x_num: 
        ;dw 0xaa, 1, 2, 0x44, 0x4f         ; num array in little-endian
    format db "%02hhx", 0                 ; Format string for printf
    newline db 0x0A, 0
    hex_chars db "0123456789ABCDEF"  ; Hex digits for converting to hex characters
section .bss
    x_struct resb 2                       ; Reserve space for struct multi
    x_num resb 600                          ; Reserve space for num array
    input_buffer resb 600                  ; Reserve space for input buffer
section .text
extern printf
extern fgets
extern stdin                    ; Declare stdin (C runtime variable)
global print_multi
global getmulti          
global main 

; Main function
main:
    ; Prologue
    push ebp                              ; Save base pointer
    mov ebp, esp                          ; Set base pointer

    ; Call print_multi(&x_struct)
    ;lea eax, [x_struct]                   ; Load address of x_struct
    ;push eax                              ; Push pointer to x_struct onto the stack
    call getmulti                      ; Call print_multi
    add esp, 4                            ; Clean up stack (1 argument)

    
    ; Exit main
    mov esp, ebp                          ; Restore stack pointer
    pop ebp                               ; Restore base pointer
    mov eax, 0                            ; Return 0
    ret                                   ; Exit program

;-----------------Task 1A-----------------
; void print_multi(struct multi *p)
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

end_loop:
    push newline                          ; Push newline string onto the stack
    call printf                           ; Call printf("\n")
    add esp, 4

    pop ebx                               ; Restore ebx
    pop edi                               ; Restore edi
    pop esi                               ; Restore esi
    mov esp, ebp                          ; Restore stack pointer
    pop ebp                               ; Restore base pointer
    ret                                   ; Return to caller


;-----------------Task 1B-----------------
; void getmulti(struct multi *p)
getmulti:
    push ebp                              ; Save base pointer
    mov ebp, esp                          ; Set base pointer
    push esi                              ; Preserve esi
    push edi                              ; Preserve edi
    push ebx                              ; Preserve ebx

    ; Load struct multi *p
    lea esi, [x_struct]                   ; esi = &x_struct (pointer to struct)
    lea edi, [input_buffer]                      ; edi = &x_num (address of num array)

    ; Read line from stdin
    push dword [stdin]              ; Push the stdin pointer from C runtime
    push 600                              ; Push size of buffer onto the stack
    push input_buffer                              ; Push buffer pointer onto the stack
    call fgets                            ; Call fgets(buffer, 600, stdin)
    add esp, 12                            ; Clean up stack (2 arguments)

    ; Calculate the length of input string (excluding newline and null terminator)
    xor ecx, ecx                          ; Clear ECX (index for length calculation)
calculate_length:
    mov al, byte [edi + ecx]              ; Load character from buffer
    cmp al, 0                             ; Check for null terminator
    je finalize_length                    ; If null terminator, break
    cmp al, 10                            ; Check for newline character
    je replace_newline                    ; If newline, break

    

    inc ecx                               ; Increment index
    jmp calculate_length                  ; Repeat for next character

replace_newline:
    mov byte [edi + ecx], 0               ; Replace newline with null terminator
finalize_length:
    lea edi, [input_buffer]                      ; edi = &x_num (address of num array)
    shr ecx, 1                            ; Divide length by 2 (hex pairs)
    mov byte [esi], cl                    ; Store length in p->size (1 byte)

    ; Call print_multi(p)
    push esi                              ; Push pointer to p onto the stack
    call print_multi                      ; Call print_multi(p)
    add esp, 4                            ; Clean up stack (1 argument)

    ; Epilogue
    pop ebx                               ; Restore ebx
    pop edi                               ; Restore edi
    pop esi                               ; Restore esi
    mov esp, ebp                          ; Restore stack pointer
    pop ebp                               ; Restore base pointer
    ret


