%define stdin           0
%define stdout          1
%define stderr          2

%define O_RDONLY        0x0
%define O_WRONLY        0x1
%define O_WRONLY        0x1
%define O_APPEND        0x400

%define syscall_exit    1
%define syscall_read    3
%define syscall_write   4
%define syscall_open    5
%define syscall_close   6

section .data
    hello db 0x0A,"Hello, Infected File", 0x0A, 0   ; Newline character
    hello_len equ $ - hello
    file_error_message db "Error file", 0;
    file_error_length equ $ - file_error_message

section .text

extern strncmp

global __start
global infector
global system_call
extern main
__start:
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
    mov     eax,1
    int     0x80
    nop
        
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

code_start:
infection:
    mov edx, hello_len             ; Save the length
    ; Now perform the write system call
    mov eax, syscall_write   ; syscall number for write
    mov ebx, stdout          ; File descriptor for stdout (1)
    mov ecx, [hello]           ; Address of the msg

    ; Write the msg to stdout
    push edx                 ; Length of argument
    push ecx                 ; Address of argument
    push ebx                 ; File descriptor
    push eax                 ; Syscall number
    call system_call         ; Perform the system call: write(STDOUT, argv[i], strlen(argv[i]))
    add esp, 16              ; Clean up the stack

infector:
    push ebp
    mov ebp, esp
    sub esp, 4     

    pushad ; save state
   ; Open the file for appending 
    mov ebx, [ebp+8]      ; File name passed as argument
    mov eax, syscall_open
    mov ecx, O_WRONLY | O_APPEND ; Open with read/write and append mode
    mov edx, 0777             ; Default permissions
    push edx                 ; Length of argument
    push ecx                 ; Address of argument
    push ebx                 ; File descriptor
    push eax                 ; Syscall number
    call system_call         ; Perform the system call: write(STDOUT, argv[i], strlen(argv[i]))
    add esp, 16              ; Clean up the stack

    ; Check if the file is opened successfully
    test eax, eax
    jz error

    ; Write the virus code at the end of the file
    mov eax, syscall_write
    mov ebx, eax           ; File descriptor returned from open
    lea ecx, [hello]  ; Address of the virus code
    mov edx, hello_len ; Length of the virus code
    push edx                 ; Length of argument
    push ecx                 ; Address of argument
    push ebx                 ; File descriptor
    push eax                 ; Syscall number
    call system_call         ; Perform the system call: write(STDOUT, argv[i], strlen(argv[i]))
    add esp, 16              ; Clean up the stack

    ; Close the file
    mov eax, syscall_close
    push edx                 ; Length of argument
    push ecx                 ; Address of argument
    push ebx                 ; File descriptor
    push eax                 ; Syscall number
    call system_call         ; Perform the system call: write(STDOUT, argv[i], strlen(argv[i]))
    add esp, 16              ; Clean up the stack

    popad

    add esp, 4
    pop ebp
    ret

error:
    popad
    add esp, 4
    pop ebp
    mov eax, 1
    mov ebx, 0x55
    int 0x80

code_end: