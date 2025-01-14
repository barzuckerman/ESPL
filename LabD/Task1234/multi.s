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
    mask:  dw 0x002D

section .bss
    input_buffer: resb 600

    ; Result struct
section .text
extern printf
extern fgets
extern malloc
extern stdin                    ; Declare stdin (C runtime variable)
extern memcpy
global print_multi
global getmulti
global maxmin  
global add_multi
global rand_num
global PRmulti       
global main 

; Main function
main:
    push ebp                              ; Save base pointer
    mov ebp, esp                          ; Set base pointer

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

    ;Print result

    push eax
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
    push eax
    call print_multi
    add esp, 4

    ; Exit main
    mov esp, ebp                          ; Restore stack pointer
    pop ebp                               ; Restore base pointer
    mov eax, 0                            ; Return 0
    ret
;---------------------part 3---------------------
R_flag:
    
    call PRmulti
    push eax
    call print_multi
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

remove_leading_zeros:
    cmp ebx, 0         ; Check if size is 0
    jz end_loop        ; If size is 0, exit the function
    mov al, byte [esi] ; Load the current byte
    cmp al, 0          ; Check if the byte is 0
    jne loop_print     ; If not 0, start printing
    dec esi            ; Move to the previous byte
    dec ebx            ; Decrease the size
    jmp remove_leading_zeros

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

    ; Load the two input arguments
    mov eax, [ebp + 8]     ; First input pointer
    mov ebx, [ebp + 12]    ; Second input pointer

    ; Determine the maximum size of the two arrays
    call maxmin
    mov edi, eax           ; Pointer to the larger array
    movzx edx, byte [edi]    ; Get the size of the larger array

    ; Allocate memory for the result
    add edx, 2             ; Size of result = max size + 2
    push edx
    call malloc
    mov esi, eax           ; Result pointer

    ; Initialize the result with the larger array
    movzx edx, byte [edi]
    mov [esi], byte dl     ; Set size of the result
    mov [esi + edx], byte 0x0 ; Null-terminate for safety

    ; Adjust pointers for copying
    add esi, 1
    add edi, 1

    ; Copy the larger array into the result buffer
    push edx
    push edi
    push esi
    call memcpy

    ; Prepare for addition
    mov edi, ebx           ; Pointer to the smaller array
    add edi, 1             ; Move past the size byte

    mov ecx, 0             ; Counter = 0
    movzx ebx, byte [edi-1]  ; Length of the smaller array
    movzx edx, byte [esi-1]  ; Length of the result buffer

process_digits:
    cmp ecx, ebx           ; Check if we've processed all digits
    jz finalize_result    ; If yes, finalize the result

    ; Perform digit addition with carry
    clc
    mov eax, 0
    mov byte al, byte [edi + ecx]  ; Load digit from smaller array
    adc byte al, byte [esi + ecx]  ; Add digit from result (with carry)
    mov byte [esi + ecx], byte al  ; Store back the result

    ; Handle carry propagation
    mov eax, ecx           
    jc propagate_carry

continue_addition:        
    add ecx, 1
    jnc process_digits     ; Continue if no overflow

propagate_carry:               
    add eax, 1
    clc
    adc byte [esi + eax], byte 0x1 ; Propagate carry
    jnc continue_addition

    jmp propagate_carry    ; Repeat until carry is resolved

finalize_result:
    sub esi, 1             ; Adjust result pointer back

    ; Update the result size
    movzx edx, byte [esi]    
    add edx, 1             ; Increment size by 1
    mov [esi], byte dl     

    ; Return the result pointer
    mov eax, esi
    mov esp, ebp
    pop ebp
    ret

;-----------------Task 3-----------------
rand_num:
; generates a pseudo-random number based on a given state and mask
    push ebp             
    mov ebp, esp  

    mov ax, [state]     ;load the current state into the ax register
    mov bx, [mask]      ;load the mask into the bx register

    and ax, bx        ;state & mask
    add ax, 1
    xor ax, 0   ;perform a bitwise XOR with 0 (no effect, but sets flags).
    jpo     odd

    ;even parity Handling
    mov bx, 0
    jmp count

odd:
    mov bx, 1   ;odd Parity Handling

count:
    mov ax, [state]
    shr ax, 1 
    shl bx, 15     
    or ax, bx     

    mov word [state] ,ax ;store the new state back into the state variable.
    movzx eax, ax
    mov esp, ebp
    pop ebp
    ret
PRmulti:
    push ebp
    mov ebp, esp
    sub esp, 8                      ; Allocate 8 bytes of local stack space (if needed, unused here)

    ; Generate a random number to determine the size of the memory block
    call rand_num
    and eax, 0x000000FF             ; Mask the random number to ensure it fits within 1 byte (0-255)
    mov ecx, eax                    ; Store the size in ECX
    push ecx                        ; Save the size on the stack for later
    push eax                        ; Push the size as an argument for malloc
    call malloc                     ; Allocate memory of size 'eax'
    add esp, 4                      ; Clean up the stack (remove the malloc argument)
    pop ecx                         ; Restore the size into ECX
    mov edi, eax                    ; Store the pointer to the allocated memory in EDI
    mov edx, 0                      ; Initialize the index/counter (EDX) to 0

    ; Store the size of the array at the start of the allocated memory
    mov [edi + edx], cl             ; Store the size (CL) at the first byte of the allocated memory

loop_PRmulti:
    push ecx                        ; Save the size (ECX) on the stack
    call rand_num                   ; Generate a new random number
    pop ecx                         ; Restore the size (ECX)
    inc edx                         ; Increment the index/counter
    mov [edx + edi], al             ; Store the random byte (AL) at the current position in the array
    cmp edx, ecx                    ; Compare the index with the size
    jl loop_PRmulti                 ; If index < size, repeat the loop

    ; Prepare to return the pointer to the allocated memory
    mov eax, edi                    ; Set the return value (pointer to the allocated memory)

    pop ebx                         ; Restore EBX (if it was saved earlier)
    mov esp, ebp                    ; Restore the stack pointer
    pop ebp                         ; Restore the base pointer
    ret                             ; Return to the caller



;-----------------------------------
end_loop:
    push newline
    push dword format_string
    call printf
    add esp, 8
end:
    mov esp, ebp
    pop ebp
    ret
