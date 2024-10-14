global m_mul, t_vec

section .text

m_mul:
    movaps xmm0,     [rsi]
    movaps xmm1, 0x10[rsi]
    movaps xmm2, 0x20[rsi]
    movaps xmm3, 0x30[rsi]

    mov rcx, 0x30

    .loop:
        movaps xmm4, [rdi + rcx]

        movaps xmm5, xmm4
        shufps xmm5, xmm5, 0x00
        mulps  xmm5, xmm0

        movaps xmm6, xmm4
        shufps xmm6, xmm6, 0x55
        mulps  xmm6, xmm1
        addps  xmm6, xmm5

        movaps xmm5, xmm4
        shufps xmm5, xmm5, 0xAA
        mulps  xmm5, xmm2
        addps  xmm6, xmm5

        shufps xmm4, xmm4, 0xFF
        mulps  xmm4, xmm3
        addps  xmm6, xmm4

        movaps [rdi + rcx], xmm6

    sub rcx, 0x10
    jge .loop
ret

t_vec:
    movups xmm0, [rsi]

    movups xmm1,     [rdi]
    movups xmm2, 0x10[rdi]
    movups xmm3, 0x20[rdi]
    movups xmm4, 0x30[rdi]

    mulps xmm1, xmm0
    mulps xmm2, xmm0
    mulps xmm3, xmm0
    mulps xmm0, xmm4

    haddps xmm1, xmm2
    haddps xmm3, xmm0
    haddps xmm1, xmm3

    movups [rsi], xmm1
ret

