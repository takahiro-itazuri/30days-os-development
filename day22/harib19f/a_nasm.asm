[BITS 32]

		GLOBAL  api_putchar
		GLOBAL	api_putstr
		GLOBAL  api_end
		GLOBAL	api_openwin

[SECTION .text]

api_putchar:	; void api_putchar(int c);
		MOV		EDX,1
		MOV		AL,[ESP+4]
		INT		0x40
		RET

api_putstr:		; void api_putstr(char *s);
		PUSH	EBX
		MOV		EDX,2
		MOV		EBX,[ESP+8]		; s
		INT		0x40
		POP		EBX
		RET

api_end:		; void api_end(void);
		MOV		EDX,4
		INT		0x40

api_openwin:	; int api_openwin(char *buf, int xsize, int ysize, int col_inv, char *title);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,5
		MOV		EBX,[ESP+16]	; buf
		MOV		ESI,[ESP+20]	; xsize
		MOV		EDI,[ESP+24]	; ysize
		MOV		EAX,[ESP+28]	; col_inv
		MOV		ECX,[ESP+32]	; title
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET
