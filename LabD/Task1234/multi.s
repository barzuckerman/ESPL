section .data
    counter: dd 0                    ; Counter for loop
    ; First test struct
    a_struct: dw 5                   ; Length of the first struct
    a_num: dw 0xAA, 0x01, 0x02, 0x44, 0x4F ; Array elements

    ; Second test struct
    y_struct: dw 6                   ; Length of the second struct
    y_num: dw 0xAA, 0x01, 0x02, 0x03, 0x44, 0x4F ; Array elements

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
    ; Result struct
    new_struct: resb 1
section .text
extern printf
extern fgets
extern malloc
extern stdin                    ; Declare stdin (C runtime variable)
global print_multi
global getmulti
global maxmin  
global add_multi       
global main 

; Main function
main:
    ; Prologue
    push ebp                              ; Save base pointer
    mov ebp, esp                          ; Set base pointer
;---------------------checking part 1---------------------
    ;call getmulti                      ; Call print_multi
    ;add esp, 4                            ; Clean up stack (1 argument)

    ;lea eax, [x_struct]            ; Load address of x_struct
    ;push eax                              ; Push pointer to x_struct onto the stack
    ;call print_multi                      ; Call print_multi
    ;add esp, 4                            ; Clean up stack (1 argument)
;---------------------checking part 2---------------------
    push dword x_struct
    call print_multi
    add esp, 4

    push dword y_struct
    call print_multi
    add esp, 4

    push dword y_struct        ; Second struct
    push dword x_struct        ; First struct
    call add_multi             ; Perform addition
    add esp, 8                 ; Clean up stack

    ; Print result
    push eax                   ; The result struct pointer is in eax
    call print_multi
    add esp, 4

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
    ; Initialize length to 0
    xor eax, eax        ; eax will hold the length, set it to 0
    mov esi, input_buffer ; esi will point to the current character in the string

calculate_length:
    cmp byte [esi], 0   ; Compare the current character with null terminator
    je length_calculated ; If null terminator is found, jump to length_calculated
    inc eax             ; Increment the length counter
    inc esi             ; Move to the next character
    jmp calculate_length ; Repeat the loop

length_calculated:
    mov ebx, eax
    mov ecx, 1

    mov [length], eax
    shr byte [length], 1 ;dividing by 2 inorder to get pairs

    mov eax, [length]
    mov dword [x_struct], eax

    mov esi, input_buffer ; esi holds the array
    mov edi, 0 ; counter

;going throw the input and converting it to hex
loop_get:
    cmp edi, [length]
    je end_loop

    ;for the letter/digit that is in the least significant part and will be most significant
    mov al, [esi]
    call letter_low
    inc esi
    inc ecx

    cmp ebx,ecx
    je odd_Length

    ;for the letter/digit that is in the most significant part and will be least significant
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
    sub al, 87 ; if it not a letter its a digit and now in al has the digit
    ret 

letter_high:
    cmp ah, 'a'
    jl digit_high
    sub ah, 87 
    ret 

digit_low:
    sub al, 48
    ret

digit_high:
    sub ah, 48 
    ret
;-----------------Task 2A-----------------

; inputs:
;   eax - pointer to the first struct
;   ebx - pointer to the second struct
; outputs:
;   eax - pointer to the struct with the larger size
;   ebx - pointer to the struct with the smaller size
maxmin:
    push ebp
    mov ebp, esp

    movzx ecx, byte [eax]  ; ecx = size field of first struct
    movzx edx, byte [ebx]  ; edx = size field of second struct
    cmp ecx, edx           
    jge done               ; if first >= second no need to swap

    xchg eax, ebx          ; Swap eax and ebx
done:
    mov esp, ebp
    pop ebp
    ret

;-----------------Task 2B-----------------
add_multi:
    push ebp
    mov ebp, esp
    push esi
    push edi
    push ebx

    mov eax, [ebp + 8]      
    mov ebx, [ebp + 12]     

    ; Determine max and min structs
    call maxmin
    add esp, 8              ; Clean up stack

    mov ecx, 0
    mov cl, byte [eax]       ; Load the size of the max struct
    add ecx, 2               ; Increase size for carry
    pushad
    push ecx
    call malloc              ; Allocate memory for the new struct
    add esp, 4
    mov dword [new_struct], eax
    popad

    mov edi, dword [new_struct]
    dec ecx
    mov byte [edi], cl       ; Store the size in the new struct
    mov dword [counter], 0
    mov ecx, 0

ebx_looper:
    shr ecx, 8
    mov edx, 0
    mov esi, dword [counter]
    mov dl, byte [eax + esi + 1]   ; Load a byte from max struct
    add ecx, edx
    mov dl, byte [ebx + esi + 1]   ; Load a byte from min struct
    add ecx, edx
    mov byte [edi + esi + 1], cl   ; Store the result in the new struct
    inc dword [counter]
    mov dl, byte [counter]
    cmp dl, byte [eax]
    jne ebx_looper
    cmp dl, byte [eax]
    je end_add

eax_looper:
    shr ecx, 8
    mov edx, 0
    mov esi, dword [counter]
    mov dl, byte [eax + esi + 1]   ; Load a byte from max struct
    add ecx, edx
    mov byte [edi + esi + 1], cl   ; Store the result in the new struct
    inc dword [counter]
    mov dl, byte [counter]
    mov dl, byte [eax]
    jne eax_looper

end_add:
    shr ecx, 8
    mov byte [edi + esi + 2], cl   ; Handle the carry
    popad
    mov esp, ebp
    pop ebp
    mov eax, dword [new_struct]
    ret    
    
end_loop:
    push newline
    push dword format_string
    call printf
    add esp, 8

    mov esp, ebp
    pop ebp
    ret
