.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $20, %rsp
    movl $5, %r11d
    cmpl $5, %r11d
    movl $0, %eax
    setge %al
    movl %eax, -4(%rbp)
    cmpl $0, -4(%rbp)
    je .and_false.0
    movl $4, %r11d
    cmpl $4, %r11d
    movl $0, %eax
    sete %al
    movl %eax, -12(%rbp)
    cmpl $0, -12(%rbp)
    je .and_false.0
    movl $1, -16(%rbp)
    jmp .and_end.1
.and_false.0:
    movl $0, -16(%rbp)
.and_end.1:
    movl -16(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
