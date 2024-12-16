%define stdin           0
%define stdout          1
%define stderr          2

%define syscall_exit    1
%define syscall_read    3
%define syscall_write   4
%define syscall_open    5
%define syscall_close   6


section .data
    newline db 0xA, 0   ; Newline character
section .text

extern strncmp
extern strlen

global main
global system_call

_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    jmp exit

main:
    push ebp
    mov ebp, esp
    mov esi, [ebp+12]       ; argv
    add esi, 4              ; skip first arg
    mov ecx, 1              ; set counter to 1
args:
    cmp ecx, [ebp+8]        ; check if we finished reading all the args
    jge end_args            ; If so, jump to end_args

    ; Save the current value of ecx (counter)
    push ecx                 ; Save the counter value

    ; Get the current argument (argv[ecx])
    push dword [esi]         ; push the current argument (argv[i])
    call strlen              ; Get the length of the arg (strlen(argv[i]))
    add esp, 4               ; Clean up the stack after calling strlen
    mov edx, eax             ; Save the length of the argument (returned from strlen)

    ; Now perform the write system call
    mov eax, syscall_write   ; syscall number for write
    mov ebx, stdout          ; File descriptor for stdout (1)
    mov ecx, [esi]           ; Address of the argument (argv[i])

    ; Write the argument to stdout
    push edx                 ; Length of argument
    push ecx                 ; Address of argument
    push ebx                 ; File descriptor
    push eax                 ; Syscall number
    call system_call         ; Perform the system call: write(STDOUT, argv[i], strlen(argv[i]))
    add esp, 16              ; Clean up the stack

    ; Write a newline character to stdout
    mov eax, syscall_write   ; syscall number for write
    mov ebx, stdout          ; File descriptor for stdout (1)
    lea ecx, [newline]       ; Address of newline character
    mov edx, 1               ; Length of the newline character (1 byte)

    push edx                 ; Length of newline
    push ecx                 ; Address of newline
    push ebx                 ; File descriptor
    push eax                 ; Syscall number
    call system_call         ; Perform the system call: write(STDOUT, newline, 1)
    add esp, 16              ; Clean up the stack

    ; Restore the value of ecx (the counter)
    pop ecx                  ; Restore the counter value

    ; Increment argument counter and move to the next argument
    inc ecx                  ; Increment the argument counter
    add esi, 4               ; Move to the next argument (argv[ecx])
    jmp args                 ; Continue to the next argument

end_args:
    ; End of argument processing, exit the program
    jmp exit

system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller


exit:
    mov     eax,syscall_exit
    int     0x80
    nop