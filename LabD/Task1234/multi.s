section .data
    format_string: db "%s", 10, 0
    _struct: db 0                  ; Reserve space for struct multi
    _num: times 600 db 0           ; Reserve space for num array

    _struct2: db 0                  ; Reserve space for struct multi
    _num2: times 600 db 0           ; Reserve space for num array

    format_hex db "%02hhx", 0                 ; Format string for printf
    newline db 0xA, 0
    combine db 0
    length dd 0

    ; First test struct
    x_struct: db 5                   ; Length of the first struct
    x_num: db 0x00aa, 1,2,0x0044,0x004f ; Array elements

    ; Second test struct
    y_struct: db 6                   ; Length of the second struct
    y_num: db 0xaa, 1, 2 , 3, 0x44, 0x4f ; Array elements


    calc_size_x: dd 0
    calc_size_y: dd 0
    new_struct:  db 0

    state: dw 0xACE1
    mask:  dw 0xB400
    FORMAT: db "%x", 0

section .bss
    input_buffer: resb 600
    pointer: resd 1

    ; Result struct
section .text
extern printf
extern fgets
extern malloc
extern stdin                    ; Declare stdin (C runtime variable)
global print_multi
global getmulti
global maxmin  
global add_multi
global rand_num
global PR_multi       
global main 

; Main function
main:
    ; Prologue
    push ebp                              ; Save base pointer
    mov ebp, esp                          ; Set base pointer
;---------------------checking part 1---------------------
    ;call getmulti                      ; Call print_multi
    ;add esp, 4                            ; Clean up stack (1 argument)

    ;lea eax, [_struct]            ; Load address of x_struct
    ;push eax                              ; Push pointer to x_struct onto the stack
    ;call print_multi                      ; Call print_multi
    ;add esp, 4                            ; Clean up stack (1 argument)
;---------------------checking part 2---------------------
    
    ;push dword x_struct
    ;call print_multi
    ;add esp, 4

    ;push dword y_struct
    ;call print_multi
    ;add esp, 4

    ;push dword y_struct        ; Second struct
    ;push dword x_struct        ; First struct
    ;call add_multi             ; Perform addition
    ;add esp, 8                 ; Clean up stack

    ; Print result

    ;push dword new_struct        ; First struct
    ;call print_multi
    ;add esp, 4
;---------------------checking part 3A---------------------
    ; call rand_num           ; Call the random number generator
    ; push eax                ; Push the value to the stack
    ; push dword format_hex   ; Push the format string
    ; call printf             ; Print the random number
    ; add esp, 8              ; Clean up the stack
    
    ; push newline
    ; push dword format_string
    ; call printf
    ; add esp, 8

;---------------------checking part 3B---------------------
    ; Test PRmulti and print the resulting MPI
    ; call PRmulti             ; Generate a pseudo-random MPI
    ; push eax                 ; Pass the pointer to the struct to print_multi
    ; call print_multi         ; Print the MPI
    ; add esp, 4               ; Clean up the stack

    mov ecx, [ebp+8] ; get argc
    mov edx, [ebp+12] ; get argv
    mov eax, [edx+4] ; get argv[1]
    cmp ecx, 2
    je args
;---------------------By default---------------------
    push dword x_struct
    call print_multi
    add esp, 4

    push dword y_struct
    call print_multi
    add esp, 4

    push dword y_struct        ; Second struct
    push dword x_struct        ; First struct
    call add_multi             ; Perform addition
    ;add esp, 8                 ; Clean up stack

    ;Print result

    push dword new_struct
    ;push eax
    call print_multi
    add esp, 4
    
    ; Exit main
    mov esp, ebp                          ; Restore stack pointer
    pop ebp                               ; Restore base pointer
    mov eax, 0                            ; Return 0
    ret                                   ; Exit program


args:     
    cmp word[eax], "-I"
    je I_flag
    cmp word[eax], "-R"
    je R_flag
    ret

;---------------------part 1B---------------------
I_flag:
    call getmulti                      ; Call print_multi
    add esp, 4                            ; Clean up stack (1 argument)
    
    mov esi, _struct          ; Source address
    mov edi, _struct2         ; Destination address
    mov ecx, 600           ; Number of bytes to copy

    cld                       ; Clear direction flag for forward copying
    rep movsb                 ; Copy ECX bytes from [ESI] to [EDI]

    lea eax, [_struct2]            ; Load address of x_struct
    push eax                              ; Push pointer to x_struct onto the stack
    call print_multi                      ; Call print_multi

    ; Clear the _struct by setting all bytes to 0 (or another sentinel value if needed)
    lea edi, [_struct]     ; Load address of _struct into edi
    mov ecx, 600           ; Set the length of the struct (in bytes)
    xor eax, eax           ; Set eax to 0 (value to fill with)
    rep stosb              ; Fill _struct with 0 bytes

    lea edi, [input_buffer]     ; Load address of _struct into edi
    mov ecx, 600           ; Set the length of the struct (in bytes)
    xor eax, eax           ; Set eax to 0 (value to fill with)
    rep stosb              ; Fill _struct with 0 bytes

    call getmulti                      ; Call print_multi
    add esp, 4                            ; Clean up stack (1 argument)

    lea eax, [_struct]            ; Load address of x_struct
    push eax                              ; Push pointer to x_struct onto the stack
    call print_multi                      ; Call print_multi
    add esp, 4                            ; Clean up stack (1 argument)

    push dword _struct2        ; Second struct
    push dword _struct        ; First struct
    call add_multi             ; Perform addition
    add esp, 8                 ; Clean up stack

    ;Print result

    push dword new_struct        
    call print_multi
    add esp, 4

    ; Exit main
    mov esp, ebp                          ; Restore stack pointer
    pop ebp                               ; Restore base pointer
    mov eax, 0                            ; Return 0
    ret
;---------------------part 3---------------------
R_flag:
    
    call PR_multi
    mov esp, ebp
    pop ebp
    ret

;-----------------Task 1A-----------------
; void print_multi(struct multi *p)
print_multi: 
    push ebp
    mov ebp, esp
    push esi                              ; Preserve esi
    push edi                              ; Preserve edi
    push ebx 

    mov esi, [ebp+8] ; pointer
    mov ebx, 0
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
    mov dword [_struct], eax

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
    mov dword [_num+edi], edx

    inc edi
    jmp loop_get

odd_Length:
    mov [combine], al
    mov edx, [combine]
    mov dword [_num+edi], edx

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
    movzx edx, byte [ebx]  
    mov dword [calc_size_y], edx   

    movzx ecx, byte [eax]   
    mov dword [calc_size_x], ecx  

    mov edi, [calc_size_y]
    mov esi, [calc_size_x]

    cmp esi, edi
    jge bigger
    mov ecx, eax
    mov eax, ebx
    mov ebx, ecx
bigger:
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

    ; determine max and min structs
    call maxmin
    mov esi, eax ;pointer to the struct with the larger size
    mov edi, ebx ; pointer to the struct with the smaller size

    movzx ecx, byte [esi] ; size of the larger struct
    add ecx, 1          ; add for the carry

    push ecx
    call malloc
    add esp, 4

    mov [new_struct], eax ; pointer to the new struct
    inc byte [esi] 
    movzx ecx, byte [esi]
    dec byte [esi]
    mov byte [new_struct], cl

    movzx ebx, byte [esi]
    inc ebx

after_malloc:
; Perform the memory modification or other operations first
    mov byte [new_struct+ebx], 0   ; Modify the byte at [new_struct + ebx]
    dec ebx                        ; Decrement ebx
    jnz after_malloc               ; Jump to after_malloc if ebx is not zero
    cmp ebx, 0                     ; Now compare ebx with zero (after operations)
    jz small              ; If ebx is zero, jump to pre_first_loop

small:
    movzx ebx, byte [edi] ; size of the smaller struct

smaller_loop:
    cmp ebx, 0
    jz big

    mov dl, byte [edi+ebx] 
    add byte [new_struct+ebx], dl 

    dec ebx
    jmp smaller_loop

big:
    movzx ebx, byte [esi] ; size of the larger struct
    clc

bigger_loop: ;also the carry
    cmp ebx, 0
    jz end

    mov dl, byte [esi+ebx] 
    add byte [new_struct+ebx], dl
    adc byte [new_struct+ebx+1], 0

    dec ebx
    jmp bigger_loop


;-----------------Task 3-----------------
PR_multi:
    push dword [state]
    push FORMAT
    call printf 
    add esp, 8

    mov esi, 20   

loop_rand_num:
    push esi 
    call rand_num

    push eax
    push FORMAT
    call printf
    add esp, 8

    pop esi
    sub esi, 1

    jz done_loop_rand_num
    jmp loop_rand_num

done_loop_rand_num:
    mov esp, ebp
    pop ebp
    ret 

rand_num:
    push ebp
    mov ebp, esp

    ; Load the current state
    mov ax, [state]
    mov bx, [mask]

    ; Random number logic
    xor bx, ax            ; XOR state with mask
    jp parity             ; Jump if parity is even (Parity Flag = 1)

    stc                   ; Set carry flag
    rcr ax, 1             ; Rotate right through carry
    jmp done_rand_num     ; Jump to the end

parity:
    shr ax, 1             ; Shift right

done_rand_num:
    mov [state], ax       ; Update state with new value
    mov eax, [state]      ; Return the new state in EAX

    mov esp, ebp
    pop ebp
    ret
end_loop:
    push newline
    push dword format_string
    call printf
    add esp, 8
end:
    mov esp, ebp
    pop ebp
    ret
