[BITS 32]

	MOV		AL,'A'
	CALL	2*8:0xc95
fin:
	HLT
	JMP		fin
