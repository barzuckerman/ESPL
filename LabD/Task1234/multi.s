section .data
    format_string: db "%s", 10, 0
    x_struct: db 0                  ; Reserve space for struct multi
    x_num: times 600 db 0           ; Reserve space for num array
    ;x_struct: 
        ;dw 5                              ; size (5 bytes in num array)
    ;x_num: 
        ;dw 0xaa, 1, 2, 0x44, 0x4f         ; num array in little-endian
    format_hex db "%02hhx", 0                 ; Format string for printf
    newline db 0x0A, 0
    combine db 0
    length dd 0
section .bss
    input_buffer: resb 600
section .text
extern printf
extern fgets
extern stdin                    ; Declare stdin (C runtime variable)
extern strlen
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

        ; Call print_multi(&x_struct)
    lea eax, [x_struct]            ; Load address of x_struct
    push eax                              ; Push pointer to x_struct onto the stack
    call print_multi                      ; Call print_multi
    add esp, 4                            ; Clean up stack (1 argument)
    
    ; Exit main
    mov esp, ebp                          ; Restore stack pointer
    pop ebp                               ; Restore base pointer
    mov eax, 0                            ; Return 0
    ret                                   ; Exit program

;-----------------Task 1A-----------------
; void print_multi(struct multi *p)
print_multi: 
    push ebp
    mov ebp, esp
    push esi                              ; Preserve esi
    push edi                              ; Preserve edi
    push ebx 

    mov esi, [ebp+8] ; pointer
    mov bl, byte [esi] ; size
    add esi, ebx ; little endian

loop_print:
    cmp ebx, 0
    jz end_loop

    mov ecx, esi
    mov cl, [ecx]

    push dword ecx
    push dword format_hex
    call printf
    add esp, 8

    dec esi
    dec ebx

    jmp loop_print



;-----------------Task 1B-----------------
; void getmulti(struct multi *p)
getmulti:
    push ebp
    mov ebp, esp

    ;loading into the buffer the input the user entered
    push dword [stdin]
    push 600
    push dword input_buffer
    call fgets
    add esp, 12
    ;calculating the size of the input
    push input_buffer
    call strlen

    mov ebx, eax
    mov ecx, 1

    mov [length], eax
    shr byte [length], 1

    mov eax, [length]
    mov dword [x_struct], eax

    mov esi, input_buffer ; buffer
    mov edi, 0 ; counter

;going throw the input and converting it to hex
loop_get:
    cmp edi, [length]
    je end_loop

    mov al, [esi]
    call letter_low
    inc esi
    inc ecx

    cmp ebx,ecx
    je odd_Length

    mov ah, [esi]
    call letter_high
    inc esi
    inc ecx

    shl al, 4
    or al, ah

    mov [combine], al
    mov edx, [combine]
    mov dword [x_num+edi], edx

    inc edi
    jmp loop_get

odd_Length:
    mov [combine], al
    mov edx, [combine]
    mov dword [x_num+edi], edx

    jmp end_loop

letter_low:
    cmp al, 'a'
    jl digit_low
    sub al, 87 
    ret 

digit_low:
    sub al, 48
    ret

letter_high:
    cmp ah, 'a'
    jl digit_high
    sub ah, 87 
    ret 

digit_high:
    sub ah, 48 
    ret


end_loop:
    push newline
    push dword format_string
    call printf
    add esp, 8

    mov esp, ebp
    pop ebp
    ret

exit:
    mov eax, 1
    mov ebx, 0
    int 0x80