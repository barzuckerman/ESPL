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
