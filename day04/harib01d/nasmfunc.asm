; nasmfunc
; TAB=4

[BITS 32]						; 32ビットモード用の機械語を作らせる

		GLOBAL	io_hlt

[SECTION .text]

io_hlt:			; void io_hlt(void);
		HLT
		RET
