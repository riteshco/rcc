	.file	"return.c"
	.text
	.globl	_start
	.type	_start, @function
_start:
	movl	$69, %eax
	ret
	.size	_start, .-_start
	.ident	"GCC: (GNU) 15.2.1 20260209"
	.section	.note.GNU-stack,"",@progbits
