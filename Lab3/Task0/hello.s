section .data
    hello: db "hello world", 0xA
    len_hello: equ $ -hello ;const declar

section .text
global main
extern system_call

main:
    push len_hello
    push hello
    push 1
    push 4
    call system_call
    add esp, 16

    xor eax, eax
    ret