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
    newline db 0xA, 0   ; Newline character
    input db "-i", 0 ;
    output db "-o", 0 ;
    buffer db 0
    file_error_message db "Error file", 0;
    file_error_length equ $ - file_error_message

section .bss
    infile resd 1
    outfile resd 1
    shift_value resd 1      ; Store the shift value

section .text

extern strncmp
extern strlen
extern positive_atoi

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

    ; Set default file descriptors
    mov dword [infile], stdin
    mov dword [outfile], stdout

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

    ; Compare if the first two characters of the argument are "-o"
    pushad
    push 2                      ; Number of bytes to check
    lea eax, [esi]              ; Address of the current argument
    push dword [eax]            ; Push argument string
    push output                 ; Push "-o" flag
    call strncmp                ; Compare with "-o"
    add esp, 12                 ; Clean up the stack
    cmp eax, 0                  ; Compare result of strncmp
    popad
    je define_output            ; Jump if "-o" is found

    ; Compare if the first two characters of the argument are "-i"
    pushad
    push 2                      ; Number of bytes to check
    lea eax, [esi]              ; Address of the current argument
    push dword [eax]            ; Push argument string
    push input                  ; Push "-i" flag
    call strncmp                ; Compare with "-i"
    add esp, 12                 ; Clean up the stack
    cmp eax, 0                  ; Compare result of strncmp
    popad
    je define_input             ; Jump if "-i" is found

    ; Restore the value of ecx (the counter)
    pop ecx                  ; Restore the counter value

    ; Increment argument counter and move to the next argument
    inc ecx                  ; Increment the argument counter
    add esi, 4               ; Move to the next argument (argv[ecx])
    jmp args                 ; Continue to the next argument


define_output:
    mov ebx, [esi]            ; Get the current argument string (e.g., "-ofile.txt")
    add ebx, 2                ; Skip the first 2 characters "-o" to get the file name
    mov ecx, O_WRONLY | O_CREAT | O_TRUNC  ; Open flags: write only, create, truncate
    mov edx, 0700o            ; File permissions (read/write/execute for owner)
    mov eax, syscall_open     ; Syscall number for open

    ; write the argument to stdout
    push edx                 ; Length of string
    push ecx                 ; Address of string
    push ebx                 ; File descriptor
    push eax                 ; Syscall number
    call system_call         ; Perform the system call: write(STDOUT, argv[i], strlen(argv[i]))
    add esp, 16              ; Clean up the stack                  ; Perform the open syscall

    test eax, eax             ; Check for errors
    js file_error             ; Jump to error handling if open fails
    mov [outfile], eax        ; Save the file descriptor for output

    pop ecx                   ; Restore the counter value
    inc ecx                   ; Increment the counter
    add esi, 4                ; Move to the next argument
    jmp args                  ; Continue processing arguments

define_input:
    mov ebx, [esi]            ; Get the current argument string (e.g., "-ifile.txt")
    add ebx, 2                ; Skip the first 2 characters "-i" to get the file name
    xor ecx, ecx              ; Open flags: O_RDONLY
    mov eax, syscall_open     ; Syscall number for open

    ; read the argument to stdout
    push edx                 ; Length of string
    push ecx                 ; Address of string
    push ebx                 ; File descriptor
    push eax                 ; Syscall number
    call system_call         ; Perform the system call: write(STDOUT, argv[i], strlen(argv[i]))

    add esp, 16              ; Clean up the stack
    test eax, eax             ; Check for errors
    js file_error             ; Jump to error handling if open fails
    mov [infile], eax         ; Save the file descriptor for input

    pop ecx                   ; Restore the counter value
    inc ecx                   ; Increment the counter
    add esi, 4                ; Move to the next argument
    jmp args                  ; Continue processing arguments

file_error:
    ; Handle file open error
    mov eax, 4              ; syscall_write
    mov ebx, stderr         ; stderr
    lea ecx, [file_error_message]
    mov edx, file_error_length
    int 0x80
    ; Proceed to exit due to failure
    jmp exit
    

end_args:
    ; end of argument processing, now encode

encode:
    push ebp
    mov ebp, esp

    ; read a character from input file
    pushad
    mov edx, 1
    mov ecx, buffer
    mov ebx, dword [infile]
    mov eax, syscall_read
    int 0x80

    cmp eax, 0
    popad
    jle encode_end

    cmp byte [buffer], 0
    je encode_end

    ; check if the character is in the range 'A' to 'z'
    cmp byte [buffer], 'A'
    jl write_encoded_data
    cmp byte [buffer], 'z'
    jg write_encoded_data
    cmp byte [buffer], 'z'
    je circular_encoding

    ; increment the character by 1
    inc byte [buffer]

write_encoded_data:
    ; write the character to the output file
    pushad
    mov edx, 1
    mov ecx, buffer
    mov ebx, dword [outfile]
    mov eax, syscall_write
    
    ; read the argument to stdout
    push edx                 ; Length of string
    push ecx                 ; Address of string
    push ebx                 ; File descriptor
    push eax                 ; Syscall number
    call system_call         ; Perform the system call: write(STDOUT, argv[i], strlen(argv[i]))
    add esp, 16              ; Clean up the stack


    jmp encode

circular_encoding:
    mov byte [buffer], 'A'
    jmp write_encoded_data

encode_end:
    ; mov esp, ebp
    ; pop ebp
    ; ret
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