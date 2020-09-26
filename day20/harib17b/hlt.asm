[BITS 32]

    MOV     AL,'A'
    CALL    0xc95
fin:
    HLT
    JMP fin
