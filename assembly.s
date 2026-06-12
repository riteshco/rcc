.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $8, %rsp
    movl $4, %r11d
    cmpl $4, %r11d
    movl $0, %eax
    sete %al
    movl %eax, -4(%rbp)
    movl -8(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
