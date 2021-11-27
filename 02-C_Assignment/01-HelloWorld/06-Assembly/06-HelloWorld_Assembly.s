
section .rodata
	msg_p1:
	db "HelloWorld !!" , 10
	
	msglen: equ $ - msg_p1

section .text

global _start

_start:

	mov rax, 1;
	mov rdi, 1;
	mov rsi, msg_p1;
	mov rdx, msglen;
	syscall;

	mov rax, 60;
	mov rdi, 0;
	syscall;


