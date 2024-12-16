%define stdin           0
%define stdout          1
%define stderr          2

%define syscall_exit    1
%define syscall_read    3
%define syscall_write   4
%define syscall_open    5
%define syscall_close   6

%define O_RDONLY        0x0
%define O_WRONLY        0x1
%define O_CREAT         0x40
%define O_TRUNC         0x200

section .data
    hello: db "hello world", 0xA
    len_hello: equ $ -hello ;const declar

section .text

extern strncmp
extern strlen

global main
extern system_call

main:
    mov ebp, esp        ; Set base pointer to stack pointer
    mov ecx, [ebp+4]    ; ecx = argc
    lea esi, [ebp+8]    ; esi = address of argv

debug:
    cmp ecx, 0          ; Check if argc is 0
    jz exit             ; If yes, exit

    mov ebx, [esi]      ; Load address of argument into ebx
    call print_arg      ; Print curr arg
    add esi, 4          ; move to the next arg
    dec ecx             ; decrement
    jmp debug           ; loop

exit:
    mov eax, syscall_exit   ; sys_exit
    xor ebx, ebx        ; Return code 0
    int 0x80            ; System call to exit program

print_arg:
    pusha              ; Save all registers
    push dword [esi]   ; memory address of esi - push curr arg to stack
    call strlen        ; call the function in util

.print:
    mov eax, syscall_write  ; sys_write syscall number
    mov ebx, stdout         ; File descriptor for stdout
    mov ecx, ebx            ; ecx now points to the argument
    int 0x80                ; Make syscall to write

    ; Print a newline after the string
    mov eax, syscall_write  ; Prepare syscall for newline
    mov ebx, stdout
    lea ecx, [newline]      ; Address of newline character
    mov edx, 1              ; Length of newline
    int 0x80

    popa               ; Restore all registers
    ret
