.intel_syntax noprefix
.global blake3_hash_many_sse2
.global _blake3_hash_many_sse2
.global blake3_compress_in_place_sse2
.global _blake3_compress_in_place_sse2
.global blake3_compress_xof_sse2
.global _blake3_compress_xof_sse2
.section .text
        .p2align  6
_blake3_hash_many_sse2:
blake3_hash_many_sse2:
        push    r15
        push    r14
        push    r13
        push    r12
        push    rsi
        push    rdi
        push    rbx
        push    rbp
        mov     rbp, rsp
        sub     rsp, 528
        and     rsp, 0xFFFFFFFFFFFFFFC0
        movdqa  xmmword ptr [rsp+0x170], xmm6
        movdqa  xmmword ptr [rsp+0x180], xmm7
        movdqa  xmmword ptr [rsp+0x190], xmm8
        movdqa  xmmword ptr [rsp+0x1A0], xmm9
        movdqa  xmmword ptr [rsp+0x1B0], xmm10
        movdqa  xmmword ptr [rsp+0x1C0], xmm11
        movdqa  xmmword ptr [rsp+0x1D0], xmm12
        movdqa  xmmword ptr [rsp+0x1E0], xmm13
        movdqa  xmmword ptr [rsp+0x1F0], xmm14
        movdqa  xmmword ptr [rsp+0x200], xmm15
        mov     rdi, rcx
        mov     rsi, rdx
        mov     rdx, r8
        mov     rcx, r9
        mov     r8, qword ptr [rbp+0x68]
        movzx   r9, byte ptr [rbp+0x70]
        neg     r9d
        movd    xmm0, r9d
        pshufd  xmm0, xmm0, 0x00
        movdqa  xmmword ptr [rsp+0x130], xmm0
        movdqa  xmm1, xmm0
        pand    xmm1, xmmword ptr [ADD0_sse+rip]
        pand    xmm0, xmmword ptr [ADD1_sse+rip]
        movdqa  xmmword ptr [rsp+0x150], xmm0
        movd    xmm0, r8d
        pshufd  xmm0, xmm0, 0x00
        paddd   xmm0, xmm1
        movdqa  xmmword ptr [rsp+0x110], xmm0
        pxor    xmm0, xmmword ptr [CMP_MSB_MASK_sse+rip]
        pxor    xmm1, xmmword ptr [CMP_MSB_MASK_sse+rip]
        pcmpgtd xmm1, xmm0
        shr     r8, 32
        movd    xmm2, r8d
        pshufd  xmm2, xmm2, 0x00
        psubd   xmm2, xmm1
        movdqa  xmmword ptr [rsp+0x120], xmm2
        mov     rbx, qword ptr [rbp+0x90]
        mov     r15, rdx
        shl     r15, 6
        movzx   r13d, byte ptr [rbp+0x78]
        movzx   r12d, byte ptr [rbp+0x88]
        cmp     rsi, 4
        jc      3f
2:
        movdqu  xmm3, xmmword ptr [rcx]
        pshufd  xmm0, xmm3, 0x00
        pshufd  xmm1, xmm3, 0x55
        pshufd  xmm2, xmm3, 0xAA
        pshufd  xmm3, xmm3, 0xFF
        movdqu  xmm7, xmmword ptr [rcx+0x10]
        pshufd  xmm4, xmm7, 0x00
        pshufd  xmm5, xmm7, 0x55
        pshufd  xmm6, xmm7, 0xAA
        pshufd  xmm7, xmm7, 0xFF
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        mov     r10, qword ptr [rdi+0x10]
        mov     r11, qword ptr [rdi+0x18]
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
9:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        movdqu  xmm8, xmmword ptr [r8+rdx-0x40]
        movdqu  xmm9, xmmword ptr [r9+rdx-0x40]
        movdqu  xmm10, xmmword ptr [r10+rdx-0x40]
        movdqu  xmm11, xmmword ptr [r11+rdx-0x40]
        movdqa  xmm12, xmm8
        punpckldq xmm8, xmm9
        punpckhdq xmm12, xmm9
        movdqa  xmm14, xmm10
        punpckldq xmm10, xmm11
        punpckhdq xmm14, xmm11
        movdqa  xmm9, xmm8
        punpcklqdq xmm8, xmm10
        punpckhqdq xmm9, xmm10
        movdqa  xmm13, xmm12
        punpcklqdq xmm12, xmm14
        punpckhqdq xmm13, xmm14
        movdqa  xmmword ptr [rsp], xmm8
        movdqa  xmmword ptr [rsp+0x10], xmm9
        movdqa  xmmword ptr [rsp+0x20], xmm12
        movdqa  xmmword ptr [rsp+0x30], xmm13
        movdqu  xmm8, xmmword ptr [r8+rdx-0x30]
        movdqu  xmm9, xmmword ptr [r9+rdx-0x30]
        movdqu  xmm10, xmmword ptr [r10+rdx-0x30]
        movdqu  xmm11, xmmword ptr [r11+rdx-0x30]
        movdqa  xmm12, xmm8
        punpckldq xmm8, xmm9
        punpckhdq xmm12, xmm9
        movdqa  xmm14, xmm10
        punpckldq xmm10, xmm11
        punpckhdq xmm14, xmm11
        movdqa  xmm9, xmm8
        punpcklqdq xmm8, xmm10
        punpckhqdq xmm9, xmm10
        movdqa  xmm13, xmm12
        punpcklqdq xmm12, xmm14
        punpckhqdq xmm13, xmm14
        movdqa  xmmword ptr [rsp+0x40], xmm8
        movdqa  xmmword ptr [rsp+0x50], xmm9
        movdqa  xmmword ptr [rsp+0x60], xmm12
        movdqa  xmmword ptr [rsp+0x70], xmm13
        movdqu  xmm8, xmmword ptr [r8+rdx-0x20]
        movdqu  xmm9, xmmword ptr [r9+rdx-0x20]
        movdqu  xmm10, xmmword ptr [r10+rdx-0x20]
        movdqu  xmm11, xmmword ptr [r11+rdx-0x20]
        movdqa  xmm12, xmm8
        punpckldq xmm8, xmm9
        punpckhdq xmm12, xmm9
        movdqa  xmm14, xmm10
        punpckldq xmm10, xmm11
        punpckhdq xmm14, xmm11
        movdqa  xmm9, xmm8
        punpcklqdq xmm8, xmm10
        punpckhqdq xmm9, xmm10
        movdqa  xmm13, xmm12
        punpcklqdq xmm12, xmm14
        punpckhqdq xmm13, xmm14
        movdqa  xmmword ptr [rsp+0x80], xmm8
        movdqa  xmmword ptr [rsp+0x90], xmm9
        movdqa  xmmword ptr [rsp+0xA0], xmm12
        movdqa  xmmword ptr [rsp+0xB0], xmm13
        movdqu  xmm8, xmmword ptr [r8+rdx-0x10]
        movdqu  xmm9, xmmword ptr [r9+rdx-0x10]
        movdqu  xmm10, xmmword ptr [r10+rdx-0x10]
        movdqu  xmm11, xmmword ptr [r11+rdx-0x10]
        movdqa  xmm12, xmm8
        punpckldq xmm8, xmm9
        punpckhdq xmm12, xmm9
        movdqa  xmm14, xmm10
        punpckldq xmm10, xmm11
        punpckhdq xmm14, xmm11
        movdqa  xmm9, xmm8
        punpcklqdq xmm8, xmm10
        punpckhqdq xmm9, xmm10
        movdqa  xmm13, xmm12
        punpcklqdq xmm12, xmm14
        punpckhqdq xmm13, xmm14
        movdqa  xmmword ptr [rsp+0xC0], xmm8
        movdqa  xmmword ptr [rsp+0xD0], xmm9
        movdqa  xmmword ptr [rsp+0xE0], xmm12
        movdqa  xmmword ptr [rsp+0xF0], xmm13
        movdqa  xmm9, xmmword ptr [BLAKE3_IV_sse2_1+rip]
        movdqa  xmm10, xmmword ptr [BLAKE3_IV_sse2_2+rip]
        movdqa  xmm11, xmmword ptr [BLAKE3_IV_sse2_3+rip]
        movdqa  xmm12, xmmword ptr [rsp+0x110]
        movdqa  xmm13, xmmword ptr [rsp+0x120]
        movdqa  xmm14, xmmword ptr [BLAKE3_BLOCK_LEN_sse+rip]
        movd    xmm15, eax
        pshufd  xmm15, xmm15, 0x00
        prefetcht0 [r8+rdx+0x80]
        prefetcht0 [r9+rdx+0x80]
        prefetcht0 [r10+rdx+0x80]
        prefetcht0 [r11+rdx+0x80]
        paddd   xmm0, xmmword ptr [rsp]
        paddd   xmm1, xmmword ptr [rsp+0x20]
        paddd   xmm2, xmmword ptr [rsp+0x40]
        paddd   xmm3, xmmword ptr [rsp+0x60]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        movdqa  xmm8, xmmword ptr [BLAKE3_IV_sse2_0+rip]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x10]
        paddd   xmm1, xmmword ptr [rsp+0x30]
        paddd   xmm2, xmmword ptr [rsp+0x50]
        paddd   xmm3, xmmword ptr [rsp+0x70]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x80]
        paddd   xmm1, xmmword ptr [rsp+0xA0]
        paddd   xmm2, xmmword ptr [rsp+0xC0]
        paddd   xmm3, xmmword ptr [rsp+0xE0]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x90]
        paddd   xmm1, xmmword ptr [rsp+0xB0]
        paddd   xmm2, xmmword ptr [rsp+0xD0]
        paddd   xmm3, xmmword ptr [rsp+0xF0]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x20]
        paddd   xmm1, xmmword ptr [rsp+0x30]
        paddd   xmm2, xmmword ptr [rsp+0x70]
        paddd   xmm3, xmmword ptr [rsp+0x40]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x60]
        paddd   xmm1, xmmword ptr [rsp+0xA0]
        paddd   xmm2, xmmword ptr [rsp]
        paddd   xmm3, xmmword ptr [rsp+0xD0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x10]
        paddd   xmm1, xmmword ptr [rsp+0xC0]
        paddd   xmm2, xmmword ptr [rsp+0x90]
        paddd   xmm3, xmmword ptr [rsp+0xF0]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xB0]
        paddd   xmm1, xmmword ptr [rsp+0x50]
        paddd   xmm2, xmmword ptr [rsp+0xE0]
        paddd   xmm3, xmmword ptr [rsp+0x80]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x30]
        paddd   xmm1, xmmword ptr [rsp+0xA0]
        paddd   xmm2, xmmword ptr [rsp+0xD0]
        paddd   xmm3, xmmword ptr [rsp+0x70]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x40]
        paddd   xmm1, xmmword ptr [rsp+0xC0]
        paddd   xmm2, xmmword ptr [rsp+0x20]
        paddd   xmm3, xmmword ptr [rsp+0xE0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x60]
        paddd   xmm1, xmmword ptr [rsp+0x90]
        paddd   xmm2, xmmword ptr [rsp+0xB0]
        paddd   xmm3, xmmword ptr [rsp+0x80]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x50]
        paddd   xmm1, xmmword ptr [rsp]
        paddd   xmm2, xmmword ptr [rsp+0xF0]
        paddd   xmm3, xmmword ptr [rsp+0x10]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xA0]
        paddd   xmm1, xmmword ptr [rsp+0xC0]
        paddd   xmm2, xmmword ptr [rsp+0xE0]
        paddd   xmm3, xmmword ptr [rsp+0xD0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x70]
        paddd   xmm1, xmmword ptr [rsp+0x90]
        paddd   xmm2, xmmword ptr [rsp+0x30]
        paddd   xmm3, xmmword ptr [rsp+0xF0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x40]
        paddd   xmm1, xmmword ptr [rsp+0xB0]
        paddd   xmm2, xmmword ptr [rsp+0x50]
        paddd   xmm3, xmmword ptr [rsp+0x10]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp]
        paddd   xmm1, xmmword ptr [rsp+0x20]
        paddd   xmm2, xmmword ptr [rsp+0x80]
        paddd   xmm3, xmmword ptr [rsp+0x60]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xC0]
        paddd   xmm1, xmmword ptr [rsp+0x90]
        paddd   xmm2, xmmword ptr [rsp+0xF0]
        paddd   xmm3, xmmword ptr [rsp+0xE0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xD0]
        paddd   xmm1, xmmword ptr [rsp+0xB0]
        paddd   xmm2, xmmword ptr [rsp+0xA0]
        paddd   xmm3, xmmword ptr [rsp+0x80]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x70]
        paddd   xmm1, xmmword ptr [rsp+0x50]
        paddd   xmm2, xmmword ptr [rsp]
        paddd   xmm3, xmmword ptr [rsp+0x60]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x20]
        paddd   xmm1, xmmword ptr [rsp+0x30]
        paddd   xmm2, xmmword ptr [rsp+0x10]
        paddd   xmm3, xmmword ptr [rsp+0x40]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x90]
        paddd   xmm1, xmmword ptr [rsp+0xB0]
        paddd   xmm2, xmmword ptr [rsp+0x80]
        paddd   xmm3, xmmword ptr [rsp+0xF0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xE0]
        paddd   xmm1, xmmword ptr [rsp+0x50]
        paddd   xmm2, xmmword ptr [rsp+0xC0]
        paddd   xmm3, xmmword ptr [rsp+0x10]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xD0]
        paddd   xmm1, xmmword ptr [rsp]
        paddd   xmm2, xmmword ptr [rsp+0x20]
        paddd   xmm3, xmmword ptr [rsp+0x40]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x30]
        paddd   xmm1, xmmword ptr [rsp+0xA0]
        paddd   xmm2, xmmword ptr [rsp+0x60]
        paddd   xmm3, xmmword ptr [rsp+0x70]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xB0]
        paddd   xmm1, xmmword ptr [rsp+0x50]
        paddd   xmm2, xmmword ptr [rsp+0x10]
        paddd   xmm3, xmmword ptr [rsp+0x80]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xF0]
        paddd   xmm1, xmmword ptr [rsp]
        paddd   xmm2, xmmword ptr [rsp+0x90]
        paddd   xmm3, xmmword ptr [rsp+0x60]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xE0]
        paddd   xmm1, xmmword ptr [rsp+0x20]
        paddd   xmm2, xmmword ptr [rsp+0x30]
        paddd   xmm3, xmmword ptr [rsp+0x70]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        pshuflw xmm15, xmm15, 0xB1
        pshufhw xmm15, xmm15, 0xB1
        pshuflw xmm12, xmm12, 0xB1
        pshufhw xmm12, xmm12, 0xB1
        pshuflw xmm13, xmm13, 0xB1
        pshufhw xmm13, xmm13, 0xB1
        pshuflw xmm14, xmm14, 0xB1
        pshufhw xmm14, xmm14, 0xB1
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xA0]
        paddd   xmm1, xmmword ptr [rsp+0xC0]
        paddd   xmm2, xmmword ptr [rsp+0x40]
        paddd   xmm3, xmmword ptr [rsp+0xD0]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmm15
        psrld   xmm15, 8
        pslld   xmm8, 24
        pxor    xmm15, xmm8
        movdqa  xmm8, xmm12
        psrld   xmm12, 8
        pslld   xmm8, 24
        pxor    xmm12, xmm8
        movdqa  xmm8, xmm13
        psrld   xmm13, 8
        pslld   xmm8, 24
        pxor    xmm13, xmm8
        movdqa  xmm8, xmm14
        psrld   xmm14, 8
        pslld   xmm8, 24
        pxor    xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        pxor    xmm0, xmm8
        pxor    xmm1, xmm9
        pxor    xmm2, xmm10
        pxor    xmm3, xmm11
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        pxor    xmm4, xmm12
        pxor    xmm5, xmm13
        pxor    xmm6, xmm14
        pxor    xmm7, xmm15
        mov     eax, r13d
        jne     9b
        movdqa  xmm9, xmm0
        punpckldq xmm0, xmm1
        punpckhdq xmm9, xmm1
        movdqa  xmm11, xmm2
        punpckldq xmm2, xmm3
        punpckhdq xmm11, xmm3
        movdqa  xmm1, xmm0
        punpcklqdq xmm0, xmm2
        punpckhqdq xmm1, xmm2
        movdqa  xmm3, xmm9
        punpcklqdq xmm9, xmm11
        punpckhqdq xmm3, xmm11
        movdqu  xmmword ptr [rbx], xmm0
        movdqu  xmmword ptr [rbx+0x20], xmm1
        movdqu  xmmword ptr [rbx+0x40], xmm9
        movdqu  xmmword ptr [rbx+0x60], xmm3
        movdqa  xmm9, xmm4
        punpckldq xmm4, xmm5
        punpckhdq xmm9, xmm5
        movdqa  xmm11, xmm6
        punpckldq xmm6, xmm7
        punpckhdq xmm11, xmm7
        movdqa  xmm5, xmm4
        punpcklqdq xmm4, xmm6
        punpckhqdq xmm5, xmm6
        movdqa  xmm7, xmm9
        punpcklqdq xmm9, xmm11
        punpckhqdq xmm7, xmm11
        movdqu  xmmword ptr [rbx+0x10], xmm4
        movdqu  xmmword ptr [rbx+0x30], xmm5
        movdqu  xmmword ptr [rbx+0x50], xmm9
        movdqu  xmmword ptr [rbx+0x70], xmm7
        movdqa  xmm1, xmmword ptr [rsp+0x110]
        movdqa  xmm0, xmm1
        paddd   xmm1, xmmword ptr [rsp+0x150]
        movdqa  xmmword ptr [rsp+0x110], xmm1
        pxor    xmm0, xmmword ptr [CMP_MSB_MASK_sse+rip]
        pxor    xmm1, xmmword ptr [CMP_MSB_MASK_sse+rip]
        pcmpgtd xmm0, xmm1
        movdqa  xmm1, xmmword ptr [rsp+0x120]
        psubd   xmm1, xmm0
        movdqa  xmmword ptr [rsp+0x120], xmm1
        add     rbx, 128
        add     rdi, 32
        sub     rsi, 4
        cmp     rsi, 4
        jnc     2b
        test    rsi, rsi
        jne     3f
4:
        movdqa  xmm6, xmmword ptr [rsp+0x170]
        movdqa  xmm7, xmmword ptr [rsp+0x180]
        movdqa  xmm8, xmmword ptr [rsp+0x190]
        movdqa  xmm9, xmmword ptr [rsp+0x1A0]
        movdqa  xmm10, xmmword ptr [rsp+0x1B0]
        movdqa  xmm11, xmmword ptr [rsp+0x1C0]
        movdqa  xmm12, xmmword ptr [rsp+0x1D0]
        movdqa  xmm13, xmmword ptr [rsp+0x1E0]
        movdqa  xmm14, xmmword ptr [rsp+0x1F0]
        movdqa  xmm15, xmmword ptr [rsp+0x200]
        mov     rsp, rbp
        pop     rbp
        pop     rbx
        pop     rdi
        pop     rsi
        pop     r12
        pop     r13
        pop     r14
        pop     r15
        ret
.p2align 5
3:
        test    esi, 0x2
        je      3f
        movups  xmm0, xmmword ptr [rcx]
        movups  xmm1, xmmword ptr [rcx+0x10]
        movaps  xmm8, xmm0
        movaps  xmm9, xmm1
        movd    xmm13, dword ptr [rsp+0x110]
        movd    xmm14, dword ptr [rsp+0x120]
        punpckldq xmm13, xmm14
        movaps  xmmword ptr [rsp], xmm13
        movd    xmm14, dword ptr [rsp+0x114]
        movd    xmm13, dword ptr [rsp+0x124]
        punpckldq xmm14, xmm13
        movaps  xmmword ptr [rsp+0x10], xmm14
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
2:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        movaps  xmm2, xmmword ptr [BLAKE3_IV_sse2+rip]
        movaps  xmm10, xmm2
        movups  xmm4, xmmword ptr [r8+rdx-0x40]
        movups  xmm5, xmmword ptr [r8+rdx-0x30]
        movaps  xmm3, xmm4
        shufps  xmm4, xmm5, 136
        shufps  xmm3, xmm5, 221
        movaps  xmm5, xmm3
        movups  xmm6, xmmword ptr [r8+rdx-0x20]
        movups  xmm7, xmmword ptr [r8+rdx-0x10]
        movaps  xmm3, xmm6
        shufps  xmm6, xmm7, 136
        pshufd  xmm6, xmm6, 0x93
        shufps  xmm3, xmm7, 221
        pshufd  xmm7, xmm3, 0x93
        movups  xmm12, xmmword ptr [r9+rdx-0x40]
        movups  xmm13, xmmword ptr [r9+rdx-0x30]
        movaps  xmm11, xmm12
        shufps  xmm12, xmm13, 136
        shufps  xmm11, xmm13, 221
        movaps  xmm13, xmm11
        movups  xmm14, xmmword ptr [r9+rdx-0x20]
        movups  xmm15, xmmword ptr [r9+rdx-0x10]
        movaps  xmm11, xmm14
        shufps  xmm14, xmm15, 136
        pshufd  xmm14, xmm14, 0x93
        shufps  xmm11, xmm15, 221
        pshufd  xmm15, xmm11, 0x93
        shl     rax, 0x20
        or      rax, 0x40
        movq    xmm3, rax
        movdqa  xmmword ptr [rsp+0x20], xmm3
        movaps  xmm3, xmmword ptr [rsp]
        movaps  xmm11, xmmword ptr [rsp+0x10]
        punpcklqdq xmm3, xmmword ptr [rsp+0x20]
        punpcklqdq xmm11, xmmword ptr [rsp+0x20]
        mov     al, 7
9:
        paddd   xmm0, xmm4
        paddd   xmm8, xmm12
        movaps  xmmword ptr [rsp+0x20], xmm4
        movaps  xmmword ptr [rsp+0x30], xmm12
        paddd   xmm0, xmm1
        paddd   xmm8, xmm9
        pxor    xmm3, xmm0
        pxor    xmm11, xmm8
        pshuflw xmm3, xmm3, 0xB1
        pshufhw xmm3, xmm3, 0xB1
        pshuflw xmm11, xmm11, 0xB1
        pshufhw xmm11, xmm11, 0xB1
        paddd   xmm2, xmm3
        paddd   xmm10, xmm11
        pxor    xmm1, xmm2
        pxor    xmm9, xmm10
        movdqa  xmm4, xmm1
        pslld   xmm1, 20
        psrld   xmm4, 12
        por     xmm1, xmm4
        movdqa  xmm4, xmm9
        pslld   xmm9, 20
        psrld   xmm4, 12
        por     xmm9, xmm4
        paddd   xmm0, xmm5
        paddd   xmm8, xmm13
        movaps  xmmword ptr [rsp+0x40], xmm5
        movaps  xmmword ptr [rsp+0x50], xmm13
        paddd   xmm0, xmm1
        paddd   xmm8, xmm9
        pxor    xmm3, xmm0
        pxor    xmm11, xmm8
        movdqa  xmm13, xmm3
        psrld   xmm3, 8
        pslld   xmm13, 24
        pxor    xmm3, xmm13
        movdqa  xmm13, xmm11
        psrld   xmm11, 8
        pslld   xmm13, 24
        pxor    xmm11, xmm13
        paddd   xmm2, xmm3
        paddd   xmm10, xmm11
        pxor    xmm1, xmm2
        pxor    xmm9, xmm10
        movdqa  xmm4, xmm1
        pslld   xmm1, 25
        psrld   xmm4, 7
        por     xmm1, xmm4
        movdqa  xmm4, xmm9
        pslld   xmm9, 25
        psrld   xmm4, 7
        por     xmm9, xmm4
        pshufd  xmm0, xmm0, 0x93
        pshufd  xmm8, xmm8, 0x93
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm11, xmm11, 0x4E
        pshufd  xmm2, xmm2, 0x39
        pshufd  xmm10, xmm10, 0x39
        paddd   xmm0, xmm6
        paddd   xmm8, xmm14
        paddd   xmm0, xmm1
        paddd   xmm8, xmm9
        pxor    xmm3, xmm0
        pxor    xmm11, xmm8
        pshuflw xmm3, xmm3, 0xB1
        pshufhw xmm3, xmm3, 0xB1
        pshuflw xmm11, xmm11, 0xB1
        pshufhw xmm11, xmm11, 0xB1
        paddd   xmm2, xmm3
        paddd   xmm10, xmm11
        pxor    xmm1, xmm2
        pxor    xmm9, xmm10
        movdqa  xmm4, xmm1
        pslld   xmm1, 20
        psrld   xmm4, 12
        por     xmm1, xmm4
        movdqa  xmm4, xmm9
        pslld   xmm9, 20
        psrld   xmm4, 12
        por     xmm9, xmm4
        paddd   xmm0, xmm7
        paddd   xmm8, xmm15
        paddd   xmm0, xmm1
        paddd   xmm8, xmm9
        pxor    xmm3, xmm0
        pxor    xmm11, xmm8
        movdqa  xmm13, xmm3
        psrld   xmm3, 8
        pslld   xmm13, 24
        pxor    xmm3, xmm13
        movdqa  xmm13, xmm11
        psrld   xmm11, 8
        pslld   xmm13, 24
        pxor    xmm11, xmm13
        paddd   xmm2, xmm3
        paddd   xmm10, xmm11
        pxor    xmm1, xmm2
        pxor    xmm9, xmm10
        movdqa  xmm4, xmm1
        pslld   xmm1, 25
        psrld   xmm4, 7
        por     xmm1, xmm4
        movdqa  xmm4, xmm9
        pslld   xmm9, 25
        psrld   xmm4, 7
        por     xmm9, xmm4
        pshufd  xmm0, xmm0, 0x39
        pshufd  xmm8, xmm8, 0x39
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm11, xmm11, 0x4E
        pshufd  xmm2, xmm2, 0x93
        pshufd  xmm10, xmm10, 0x93
        dec     al
        je      9f
        movdqa  xmm12, xmmword ptr [rsp+0x20]
        movdqa  xmm5, xmmword ptr [rsp+0x40]
        pshufd  xmm13, xmm12, 0x0F
        shufps  xmm12, xmm5, 214
        pshufd  xmm4, xmm12, 0x39
        movdqa  xmm12, xmm6
        shufps  xmm12, xmm7, 250
        pand    xmm13, xmmword ptr [PBLENDW_0x33_MASK+rip]
        pand    xmm12, xmmword ptr [PBLENDW_0xCC_MASK+rip]
        por     xmm13, xmm12
        movdqa  xmmword ptr [rsp+0x20], xmm13
        movdqa  xmm12, xmm7
        punpcklqdq xmm12, xmm5
        movdqa  xmm13, xmm6
        pand    xmm12, xmmword ptr [PBLENDW_0x3F_MASK+rip]
        pand    xmm13, xmmword ptr [PBLENDW_0xC0_MASK+rip]
        por     xmm12, xmm13
        pshufd  xmm12, xmm12, 0x78
        punpckhdq xmm5, xmm7
        punpckldq xmm6, xmm5
        pshufd  xmm7, xmm6, 0x1E
        movdqa  xmmword ptr [rsp+0x40], xmm12
        movdqa  xmm5, xmmword ptr [rsp+0x30]
        movdqa  xmm13, xmmword ptr [rsp+0x50]
        pshufd  xmm6, xmm5, 0x0F
        shufps  xmm5, xmm13, 214
        pshufd  xmm12, xmm5, 0x39
        movdqa  xmm5, xmm14
        shufps  xmm5, xmm15, 250
        pand    xmm6, xmmword ptr [PBLENDW_0x33_MASK+rip]
        pand    xmm5, xmmword ptr [PBLENDW_0xCC_MASK+rip]
        por     xmm6, xmm5
        movdqa  xmm5, xmm15
        punpcklqdq xmm5, xmm13
        movdqa  xmmword ptr [rsp+0x30], xmm2
        movdqa  xmm2, xmm14
        pand    xmm5, xmmword ptr [PBLENDW_0x3F_MASK+rip]
        pand    xmm2, xmmword ptr [PBLENDW_0xC0_MASK+rip]
        por     xmm5, xmm2
        movdqa  xmm2, xmmword ptr [rsp+0x30]
        pshufd  xmm5, xmm5, 0x78
        punpckhdq xmm13, xmm15
        punpckldq xmm14, xmm13
        pshufd  xmm15, xmm14, 0x1E
        movdqa  xmm13, xmm6
        movdqa  xmm14, xmm5
        movdqa  xmm5, xmmword ptr [rsp+0x20]
        movdqa  xmm6, xmmword ptr [rsp+0x40]
        jmp     9b
9:
        pxor    xmm0, xmm2
        pxor    xmm1, xmm3
        pxor    xmm8, xmm10
        pxor    xmm9, xmm11
        mov     eax, r13d
        cmp     rdx, r15
        jne     2b
        movups  xmmword ptr [rbx], xmm0
        movups  xmmword ptr [rbx+0x10], xmm1
        movups  xmmword ptr [rbx+0x20], xmm8
        movups  xmmword ptr [rbx+0x30], xmm9
        mov     eax, dword ptr [rsp+0x130]
        neg     eax
        mov    r10d, dword ptr [rsp+0x110+8*rax]
        mov    r11d, dword ptr [rsp+0x120+8*rax]
        mov dword ptr [rsp+0x110], r10d
        mov dword ptr [rsp+0x120], r11d
        add     rdi, 16
        add     rbx, 64
        sub     rsi, 2
3:
        test    esi, 0x1
        je      4b
        movups  xmm0, xmmword ptr [rcx]
        movups  xmm1, xmmword ptr [rcx+0x10]
        movd    xmm13, dword ptr [rsp+0x110]
        movd    xmm14, dword ptr [rsp+0x120]
        punpckldq xmm13, xmm14
        mov     r8, qword ptr [rdi]
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
2:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        movaps  xmm2, xmmword ptr [BLAKE3_IV_sse2+rip]
        shl     rax, 32
        or      rax, 64
        movq    xmm12, rax
        movdqa  xmm3, xmm13
        punpcklqdq xmm3, xmm12
        movups  xmm4, xmmword ptr [r8+rdx-0x40]
        movups  xmm5, xmmword ptr [r8+rdx-0x30]
        movaps  xmm8, xmm4
        shufps  xmm4, xmm5, 136
        shufps  xmm8, xmm5, 221
        movaps  xmm5, xmm8
        movups  xmm6, xmmword ptr [r8+rdx-0x20]
        movups  xmm7, xmmword ptr [r8+rdx-0x10]
        movaps  xmm8, xmm6
        shufps  xmm6, xmm7, 136
        pshufd  xmm6, xmm6, 0x93
        shufps  xmm8, xmm7, 221
        pshufd  xmm7, xmm8, 0x93
        mov     al, 7
9:
        paddd   xmm0, xmm4
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshuflw xmm3, xmm3, 0xB1
        pshufhw xmm3, xmm3, 0xB1
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm5
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        movdqa  xmm14, xmm3
        psrld   xmm3, 8
        pslld   xmm14, 24
        pxor    xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x93
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x39
        paddd   xmm0, xmm6
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshuflw xmm3, xmm3, 0xB1
        pshufhw xmm3, xmm3, 0xB1
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm7
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        movdqa  xmm14, xmm3
        psrld   xmm3, 8
        pslld   xmm14, 24
        pxor    xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x39
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x93
        dec     al
        jz      9f
        movdqa  xmm8, xmm4
        shufps  xmm8, xmm5, 214
        pshufd  xmm9, xmm4, 0x0F
        pshufd  xmm4, xmm8, 0x39
        movdqa  xmm8, xmm6
        shufps  xmm8, xmm7, 250
        pand    xmm9, xmmword ptr [PBLENDW_0x33_MASK+rip]
        pand    xmm8, xmmword ptr [PBLENDW_0xCC_MASK+rip]
        por     xmm9, xmm8
        movdqa  xmm8, xmm7
        punpcklqdq xmm8, xmm5
        movdqa  xmm10, xmm6
        pand    xmm8, xmmword ptr [PBLENDW_0x3F_MASK+rip]
        pand    xmm10, xmmword ptr [PBLENDW_0xC0_MASK+rip]
        por     xmm8, xmm10
        pshufd  xmm8, xmm8, 0x78
        punpckhdq xmm5, xmm7
        punpckldq xmm6, xmm5
        pshufd  xmm7, xmm6, 0x1E
        movdqa  xmm5, xmm9
        movdqa  xmm6, xmm8
        jmp     9b
9:
        pxor    xmm0, xmm2
        pxor    xmm1, xmm3
        mov     eax, r13d
        cmp     rdx, r15
        jne     2b
        movups  xmmword ptr [rbx], xmm0
        movups  xmmword ptr [rbx+0x10], xmm1
        jmp     4b

.p2align 6
blake3_compress_in_place_sse2:
_blake3_compress_in_place_sse2:
        sub     rsp, 120
        movdqa  xmmword ptr [rsp], xmm6
        movdqa  xmmword ptr [rsp+0x10], xmm7
        movdqa  xmmword ptr [rsp+0x20], xmm8
        movdqa  xmmword ptr [rsp+0x30], xmm9
        movdqa  xmmword ptr [rsp+0x40], xmm11
        movdqa  xmmword ptr [rsp+0x50], xmm14
        movdqa  xmmword ptr [rsp+0x60], xmm15
        movups  xmm0, xmmword ptr [rcx]
        movups  xmm1, xmmword ptr [rcx+0x10]
        movaps  xmm2, xmmword ptr [BLAKE3_IV_sse2+rip]
        movzx   eax, byte ptr [rsp+0xA0]
        movzx   r8d, r8b
        shl     rax, 32
        add     r8, rax
        movq    xmm3, r9
        movq    xmm4, r8
        punpcklqdq xmm3, xmm4
        movups  xmm4, xmmword ptr [rdx]
        movups  xmm5, xmmword ptr [rdx+0x10]
        movaps  xmm8, xmm4
        shufps  xmm4, xmm5, 136
        shufps  xmm8, xmm5, 221
        movaps  xmm5, xmm8
        movups  xmm6, xmmword ptr [rdx+0x20]
        movups  xmm7, xmmword ptr [rdx+0x30]
        movaps  xmm8, xmm6
        shufps  xmm6, xmm7, 136
        pshufd  xmm6, xmm6, 0x93
        shufps  xmm8, xmm7, 221
        pshufd  xmm7, xmm8, 0x93
        mov     al, 7
9:
        paddd   xmm0, xmm4
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshuflw xmm3, xmm3, 0xB1
        pshufhw xmm3, xmm3, 0xB1
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm5
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        movdqa  xmm14, xmm3
        psrld   xmm3, 8
        pslld   xmm14, 24
        pxor    xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x93
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x39
        paddd   xmm0, xmm6
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshuflw xmm3, xmm3, 0xB1
        pshufhw xmm3, xmm3, 0xB1
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm7
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        movdqa  xmm14, xmm3
        psrld   xmm3, 8
        pslld   xmm14, 24
        pxor    xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x39
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x93
        dec     al
        jz      9f
        movdqa  xmm8, xmm4
        shufps  xmm8, xmm5, 214
        pshufd  xmm9, xmm4, 0x0F
        pshufd  xmm4, xmm8, 0x39
        movdqa  xmm8, xmm6
        shufps  xmm8, xmm7, 250
        pand    xmm9, xmmword ptr [PBLENDW_0x33_MASK+rip]
        pand    xmm8, xmmword ptr [PBLENDW_0xCC_MASK+rip]
        por     xmm9, xmm8
        movdqa  xmm8, xmm7
        punpcklqdq xmm8, xmm5
        movdqa  xmm10, xmm6
        pand    xmm8, xmmword ptr [PBLENDW_0x3F_MASK+rip]
        pand    xmm10, xmmword ptr [PBLENDW_0xC0_MASK+rip]
        por     xmm8, xmm10
        pshufd  xmm8, xmm8, 0x78
        punpckhdq xmm5, xmm7
        punpckldq xmm6, xmm5
        pshufd  xmm7, xmm6, 0x1E
        movdqa  xmm5, xmm9
        movdqa  xmm6, xmm8
        jmp     9b
9:
        pxor    xmm0, xmm2
        pxor    xmm1, xmm3
        movups  xmmword ptr [rcx], xmm0
        movups  xmmword ptr [rcx+0x10], xmm1
        movdqa  xmm6, xmmword ptr [rsp]
        movdqa  xmm7, xmmword ptr [rsp+0x10]
        movdqa  xmm8, xmmword ptr [rsp+0x20]
        movdqa  xmm9, xmmword ptr [rsp+0x30]
        movdqa  xmm11, xmmword ptr [rsp+0x40]
        movdqa  xmm14, xmmword ptr [rsp+0x50]
        movdqa  xmm15, xmmword ptr [rsp+0x60]
        add     rsp, 120
        ret


.p2align 6
_blake3_compress_xof_sse2:
blake3_compress_xof_sse2:
        sub     rsp, 120
        movdqa  xmmword ptr [rsp], xmm6
        movdqa  xmmword ptr [rsp+0x10], xmm7
        movdqa  xmmword ptr [rsp+0x20], xmm8
        movdqa  xmmword ptr [rsp+0x30], xmm9
        movdqa  xmmword ptr [rsp+0x40], xmm11
        movdqa  xmmword ptr [rsp+0x50], xmm14
        movdqa  xmmword ptr [rsp+0x60], xmm15
        movups  xmm0, xmmword ptr [rcx]
        movups  xmm1, xmmword ptr [rcx+0x10]
        movaps  xmm2, xmmword ptr [BLAKE3_IV_sse2+rip]
        movzx   eax, byte ptr [rsp+0xA0]
        movzx   r8d, r8b
        mov     r10, qword ptr [rsp+0xA8]
        shl     rax, 32
        add     r8, rax
        movq    xmm3, r9
        movq    xmm4, r8
        punpcklqdq xmm3, xmm4
        movups  xmm4, xmmword ptr [rdx]
        movups  xmm5, xmmword ptr [rdx+0x10]
        movaps  xmm8, xmm4
        shufps  xmm4, xmm5, 136
        shufps  xmm8, xmm5, 221
        movaps  xmm5, xmm8
        movups  xmm6, xmmword ptr [rdx+0x20]
        movups  xmm7, xmmword ptr [rdx+0x30]
        movaps  xmm8, xmm6
        shufps  xmm6, xmm7, 136
        pshufd  xmm6, xmm6, 0x93
        shufps  xmm8, xmm7, 221
        pshufd  xmm7, xmm8, 0x93
        mov     al, 7
9:
        paddd   xmm0, xmm4
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshuflw xmm3, xmm3, 0xB1
        pshufhw xmm3, xmm3, 0xB1
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm5
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        movdqa  xmm14, xmm3
        psrld   xmm3, 8
        pslld   xmm14, 24
        pxor    xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x93
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x39
        paddd   xmm0, xmm6
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshuflw xmm3, xmm3, 0xB1
        pshufhw xmm3, xmm3, 0xB1
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm7
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        movdqa  xmm14, xmm3
        psrld   xmm3, 8
        pslld   xmm14, 24
        pxor    xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x39
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x93
        dec     al
        jz      9f
        movdqa  xmm8, xmm4
        shufps  xmm8, xmm5, 214
        pshufd  xmm9, xmm4, 0x0F
        pshufd  xmm4, xmm8, 0x39
        movdqa  xmm8, xmm6
        shufps  xmm8, xmm7, 250
        pand    xmm9, xmmword ptr [PBLENDW_0x33_MASK+rip]
        pand    xmm8, xmmword ptr [PBLENDW_0xCC_MASK+rip]
        por     xmm9, xmm8
        movdqa  xmm8, xmm7
        punpcklqdq xmm8, xmm5
        movdqa  xmm10, xmm6
        pand    xmm8, xmmword ptr [PBLENDW_0x3F_MASK+rip]
        pand    xmm10, xmmword ptr [PBLENDW_0xC0_MASK+rip]
        por     xmm8, xmm10
        pshufd  xmm8, xmm8, 0x78
        punpckhdq xmm5, xmm7
        punpckldq xmm6, xmm5
        pshufd  xmm7, xmm6, 0x1E
        movdqa  xmm5, xmm9
        movdqa  xmm6, xmm8
        jmp     9b
9:
        movdqu  xmm4, xmmword ptr [rcx]
        movdqu  xmm5, xmmword ptr [rcx+0x10]
        pxor    xmm0, xmm2
        pxor    xmm1, xmm3
        pxor    xmm2, xmm4
        pxor    xmm3, xmm5
        movups  xmmword ptr [r10], xmm0
        movups  xmmword ptr [r10+0x10], xmm1
        movups  xmmword ptr [r10+0x20], xmm2
        movups  xmmword ptr [r10+0x30], xmm3
        movdqa  xmm6, xmmword ptr [rsp]
        movdqa  xmm7, xmmword ptr [rsp+0x10]
        movdqa  xmm8, xmmword ptr [rsp+0x20]
        movdqa  xmm9, xmmword ptr [rsp+0x30]
        movdqa  xmm11, xmmword ptr [rsp+0x40]
        movdqa  xmm14, xmmword ptr [rsp+0x50]
        movdqa  xmm15, xmmword ptr [rsp+0x60]
        add     rsp, 120
        ret


.section .rodata
.p2align  6
BLAKE3_IV_sse2:
        .long  0x6A09E667, 0xBB67AE85
        .long  0x3C6EF372, 0xA54FF53A
ADD0_sse:   
        .long  0, 1, 2, 3
ADD1_sse:
        .long  4, 4, 4, 4
BLAKE3_IV_sse2_0:
        .long  0x6A09E667, 0x6A09E667, 0x6A09E667, 0x6A09E667
BLAKE3_IV_sse2_1:
        .long  0xBB67AE85, 0xBB67AE85, 0xBB67AE85, 0xBB67AE85
BLAKE3_IV_sse2_2:
        .long  0x3C6EF372, 0x3C6EF372, 0x3C6EF372, 0x3C6EF372
BLAKE3_IV_sse2_3:
        .long  0xA54FF53A, 0xA54FF53A, 0xA54FF53A, 0xA54FF53A
BLAKE3_BLOCK_LEN_sse:
        .long  64, 64, 64, 64
CMP_MSB_MASK_sse:
        .long  0x80000000, 0x80000000, 0x80000000, 0x80000000
PBLENDW_0x33_MASK:
        .long  0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000
PBLENDW_0xCC_MASK:
        .long  0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF
PBLENDW_0x3F_MASK:
        .long  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000
PBLENDW_0xC0_MASK:
        .long  0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF
















.intel_syntax noprefix
.global blake3_hash_many_sse41
.global _blake3_hash_many_sse41
.global blake3_compress_in_place_sse41
.global _blake3_compress_in_place_sse41
.global blake3_compress_xof_sse41
.global _blake3_compress_xof_sse41
.section .text
        .p2align  6
_blake3_hash_many_sse41:
blake3_hash_many_sse41:
        push    r15
        push    r14
        push    r13
        push    r12
        push    rsi
        push    rdi
        push    rbx
        push    rbp
        mov     rbp, rsp
        sub     rsp, 528
        and     rsp, 0xFFFFFFFFFFFFFFC0
        movdqa  xmmword ptr [rsp+0x170], xmm6
        movdqa  xmmword ptr [rsp+0x180], xmm7
        movdqa  xmmword ptr [rsp+0x190], xmm8
        movdqa  xmmword ptr [rsp+0x1A0], xmm9
        movdqa  xmmword ptr [rsp+0x1B0], xmm10
        movdqa  xmmword ptr [rsp+0x1C0], xmm11
        movdqa  xmmword ptr [rsp+0x1D0], xmm12
        movdqa  xmmword ptr [rsp+0x1E0], xmm13
        movdqa  xmmword ptr [rsp+0x1F0], xmm14
        movdqa  xmmword ptr [rsp+0x200], xmm15
        mov     rdi, rcx
        mov     rsi, rdx
        mov     rdx, r8
        mov     rcx, r9
        mov     r8, qword ptr [rbp+0x68]
        movzx   r9, byte ptr [rbp+0x70]
        neg     r9d
        movd    xmm0, r9d
        pshufd  xmm0, xmm0, 0x00
        movdqa  xmmword ptr [rsp+0x130], xmm0
        movdqa  xmm1, xmm0
        pand    xmm1, xmmword ptr [ADD0_sse41+rip]
        pand    xmm0, xmmword ptr [ADD1_sse41+rip]
        movdqa  xmmword ptr [rsp+0x150], xmm0
        movd    xmm0, r8d
        pshufd  xmm0, xmm0, 0x00
        paddd   xmm0, xmm1
        movdqa  xmmword ptr [rsp+0x110], xmm0
        pxor    xmm0, xmmword ptr [CMP_MSB_MASK_sse41+rip]
        pxor    xmm1, xmmword ptr [CMP_MSB_MASK_sse41+rip]
        pcmpgtd xmm1, xmm0
        shr     r8, 32
        movd    xmm2, r8d
        pshufd  xmm2, xmm2, 0x00
        psubd   xmm2, xmm1
        movdqa  xmmword ptr [rsp+0x120], xmm2
        mov     rbx, qword ptr [rbp+0x90]
        mov     r15, rdx
        shl     r15, 6
        movzx   r13d, byte ptr [rbp+0x78]
        movzx   r12d, byte ptr [rbp+0x88]
        cmp     rsi, 4
        jc      3f
2:
        movdqu  xmm3, xmmword ptr [rcx]
        pshufd  xmm0, xmm3, 0x00
        pshufd  xmm1, xmm3, 0x55
        pshufd  xmm2, xmm3, 0xAA
        pshufd  xmm3, xmm3, 0xFF
        movdqu  xmm7, xmmword ptr [rcx+0x10]
        pshufd  xmm4, xmm7, 0x00
        pshufd  xmm5, xmm7, 0x55
        pshufd  xmm6, xmm7, 0xAA
        pshufd  xmm7, xmm7, 0xFF
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        mov     r10, qword ptr [rdi+0x10]
        mov     r11, qword ptr [rdi+0x18]
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
9:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        movdqu  xmm8, xmmword ptr [r8+rdx-0x40]
        movdqu  xmm9, xmmword ptr [r9+rdx-0x40]
        movdqu  xmm10, xmmword ptr [r10+rdx-0x40]
        movdqu  xmm11, xmmword ptr [r11+rdx-0x40]
        movdqa  xmm12, xmm8
        punpckldq xmm8, xmm9
        punpckhdq xmm12, xmm9
        movdqa  xmm14, xmm10
        punpckldq xmm10, xmm11
        punpckhdq xmm14, xmm11
        movdqa  xmm9, xmm8
        punpcklqdq xmm8, xmm10
        punpckhqdq xmm9, xmm10
        movdqa  xmm13, xmm12
        punpcklqdq xmm12, xmm14
        punpckhqdq xmm13, xmm14
        movdqa  xmmword ptr [rsp], xmm8
        movdqa  xmmword ptr [rsp+0x10], xmm9
        movdqa  xmmword ptr [rsp+0x20], xmm12
        movdqa  xmmword ptr [rsp+0x30], xmm13
        movdqu  xmm8, xmmword ptr [r8+rdx-0x30]
        movdqu  xmm9, xmmword ptr [r9+rdx-0x30]
        movdqu  xmm10, xmmword ptr [r10+rdx-0x30]
        movdqu  xmm11, xmmword ptr [r11+rdx-0x30]
        movdqa  xmm12, xmm8
        punpckldq xmm8, xmm9
        punpckhdq xmm12, xmm9
        movdqa  xmm14, xmm10
        punpckldq xmm10, xmm11
        punpckhdq xmm14, xmm11
        movdqa  xmm9, xmm8
        punpcklqdq xmm8, xmm10
        punpckhqdq xmm9, xmm10
        movdqa  xmm13, xmm12
        punpcklqdq xmm12, xmm14
        punpckhqdq xmm13, xmm14
        movdqa  xmmword ptr [rsp+0x40], xmm8
        movdqa  xmmword ptr [rsp+0x50], xmm9
        movdqa  xmmword ptr [rsp+0x60], xmm12
        movdqa  xmmword ptr [rsp+0x70], xmm13
        movdqu  xmm8, xmmword ptr [r8+rdx-0x20]
        movdqu  xmm9, xmmword ptr [r9+rdx-0x20]
        movdqu  xmm10, xmmword ptr [r10+rdx-0x20]
        movdqu  xmm11, xmmword ptr [r11+rdx-0x20]
        movdqa  xmm12, xmm8
        punpckldq xmm8, xmm9
        punpckhdq xmm12, xmm9
        movdqa  xmm14, xmm10
        punpckldq xmm10, xmm11
        punpckhdq xmm14, xmm11
        movdqa  xmm9, xmm8
        punpcklqdq xmm8, xmm10
        punpckhqdq xmm9, xmm10
        movdqa  xmm13, xmm12
        punpcklqdq xmm12, xmm14
        punpckhqdq xmm13, xmm14
        movdqa  xmmword ptr [rsp+0x80], xmm8
        movdqa  xmmword ptr [rsp+0x90], xmm9
        movdqa  xmmword ptr [rsp+0xA0], xmm12
        movdqa  xmmword ptr [rsp+0xB0], xmm13
        movdqu  xmm8, xmmword ptr [r8+rdx-0x10]
        movdqu  xmm9, xmmword ptr [r9+rdx-0x10]
        movdqu  xmm10, xmmword ptr [r10+rdx-0x10]
        movdqu  xmm11, xmmword ptr [r11+rdx-0x10]
        movdqa  xmm12, xmm8
        punpckldq xmm8, xmm9
        punpckhdq xmm12, xmm9
        movdqa  xmm14, xmm10
        punpckldq xmm10, xmm11
        punpckhdq xmm14, xmm11
        movdqa  xmm9, xmm8
        punpcklqdq xmm8, xmm10
        punpckhqdq xmm9, xmm10
        movdqa  xmm13, xmm12
        punpcklqdq xmm12, xmm14
        punpckhqdq xmm13, xmm14
        movdqa  xmmword ptr [rsp+0xC0], xmm8
        movdqa  xmmword ptr [rsp+0xD0], xmm9
        movdqa  xmmword ptr [rsp+0xE0], xmm12
        movdqa  xmmword ptr [rsp+0xF0], xmm13
        movdqa  xmm9, xmmword ptr [ BLAKE3_IV_1_sse41+rip]
        movdqa  xmm10, xmmword ptr [ BLAKE3_IV_2_sse41+rip]
        movdqa  xmm11, xmmword ptr [ BLAKE3_IV_3_sse41+rip]
        movdqa  xmm12, xmmword ptr [rsp+0x110]
        movdqa  xmm13, xmmword ptr [rsp+0x120]
        movdqa  xmm14, xmmword ptr [BLAKE3_BLOCK_LEN_sse41+rip]
        movd    xmm15, eax
        pshufd  xmm15, xmm15, 0x00
        prefetcht0 [r8+rdx+0x80]
        prefetcht0 [r9+rdx+0x80]
        prefetcht0 [r10+rdx+0x80]
        prefetcht0 [r11+rdx+0x80]
        paddd   xmm0, xmmword ptr [rsp]
        paddd   xmm1, xmmword ptr [rsp+0x20]
        paddd   xmm2, xmmword ptr [rsp+0x40]
        paddd   xmm3, xmmword ptr [rsp+0x60]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [ BLAKE3_IV_0_sse41+rip]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x10]
        paddd   xmm1, xmmword ptr [rsp+0x30]
        paddd   xmm2, xmmword ptr [rsp+0x50]
        paddd   xmm3, xmmword ptr [rsp+0x70]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x80]
        paddd   xmm1, xmmword ptr [rsp+0xA0]
        paddd   xmm2, xmmword ptr [rsp+0xC0]
        paddd   xmm3, xmmword ptr [rsp+0xE0]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x90]
        paddd   xmm1, xmmword ptr [rsp+0xB0]
        paddd   xmm2, xmmword ptr [rsp+0xD0]
        paddd   xmm3, xmmword ptr [rsp+0xF0]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x20]
        paddd   xmm1, xmmword ptr [rsp+0x30]
        paddd   xmm2, xmmword ptr [rsp+0x70]
        paddd   xmm3, xmmword ptr [rsp+0x40]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x60]
        paddd   xmm1, xmmword ptr [rsp+0xA0]
        paddd   xmm2, xmmword ptr [rsp]
        paddd   xmm3, xmmword ptr [rsp+0xD0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x10]
        paddd   xmm1, xmmword ptr [rsp+0xC0]
        paddd   xmm2, xmmword ptr [rsp+0x90]
        paddd   xmm3, xmmword ptr [rsp+0xF0]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xB0]
        paddd   xmm1, xmmword ptr [rsp+0x50]
        paddd   xmm2, xmmword ptr [rsp+0xE0]
        paddd   xmm3, xmmword ptr [rsp+0x80]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x30]
        paddd   xmm1, xmmword ptr [rsp+0xA0]
        paddd   xmm2, xmmword ptr [rsp+0xD0]
        paddd   xmm3, xmmword ptr [rsp+0x70]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x40]
        paddd   xmm1, xmmword ptr [rsp+0xC0]
        paddd   xmm2, xmmword ptr [rsp+0x20]
        paddd   xmm3, xmmword ptr [rsp+0xE0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x60]
        paddd   xmm1, xmmword ptr [rsp+0x90]
        paddd   xmm2, xmmword ptr [rsp+0xB0]
        paddd   xmm3, xmmword ptr [rsp+0x80]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x50]
        paddd   xmm1, xmmword ptr [rsp]
        paddd   xmm2, xmmword ptr [rsp+0xF0]
        paddd   xmm3, xmmword ptr [rsp+0x10]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xA0]
        paddd   xmm1, xmmword ptr [rsp+0xC0]
        paddd   xmm2, xmmword ptr [rsp+0xE0]
        paddd   xmm3, xmmword ptr [rsp+0xD0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x70]
        paddd   xmm1, xmmword ptr [rsp+0x90]
        paddd   xmm2, xmmword ptr [rsp+0x30]
        paddd   xmm3, xmmword ptr [rsp+0xF0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x40]
        paddd   xmm1, xmmword ptr [rsp+0xB0]
        paddd   xmm2, xmmword ptr [rsp+0x50]
        paddd   xmm3, xmmword ptr [rsp+0x10]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp]
        paddd   xmm1, xmmword ptr [rsp+0x20]
        paddd   xmm2, xmmword ptr [rsp+0x80]
        paddd   xmm3, xmmword ptr [rsp+0x60]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xC0]
        paddd   xmm1, xmmword ptr [rsp+0x90]
        paddd   xmm2, xmmword ptr [rsp+0xF0]
        paddd   xmm3, xmmword ptr [rsp+0xE0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xD0]
        paddd   xmm1, xmmword ptr [rsp+0xB0]
        paddd   xmm2, xmmword ptr [rsp+0xA0]
        paddd   xmm3, xmmword ptr [rsp+0x80]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x70]
        paddd   xmm1, xmmword ptr [rsp+0x50]
        paddd   xmm2, xmmword ptr [rsp]
        paddd   xmm3, xmmword ptr [rsp+0x60]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x20]
        paddd   xmm1, xmmword ptr [rsp+0x30]
        paddd   xmm2, xmmword ptr [rsp+0x10]
        paddd   xmm3, xmmword ptr [rsp+0x40]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x90]
        paddd   xmm1, xmmword ptr [rsp+0xB0]
        paddd   xmm2, xmmword ptr [rsp+0x80]
        paddd   xmm3, xmmword ptr [rsp+0xF0]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xE0]
        paddd   xmm1, xmmword ptr [rsp+0x50]
        paddd   xmm2, xmmword ptr [rsp+0xC0]
        paddd   xmm3, xmmword ptr [rsp+0x10]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xD0]
        paddd   xmm1, xmmword ptr [rsp]
        paddd   xmm2, xmmword ptr [rsp+0x20]
        paddd   xmm3, xmmword ptr [rsp+0x40]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0x30]
        paddd   xmm1, xmmword ptr [rsp+0xA0]
        paddd   xmm2, xmmword ptr [rsp+0x60]
        paddd   xmm3, xmmword ptr [rsp+0x70]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xB0]
        paddd   xmm1, xmmword ptr [rsp+0x50]
        paddd   xmm2, xmmword ptr [rsp+0x10]
        paddd   xmm3, xmmword ptr [rsp+0x80]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xF0]
        paddd   xmm1, xmmword ptr [rsp]
        paddd   xmm2, xmmword ptr [rsp+0x90]
        paddd   xmm3, xmmword ptr [rsp+0x60]
        paddd   xmm0, xmm4
        paddd   xmm1, xmm5
        paddd   xmm2, xmm6
        paddd   xmm3, xmm7
        pxor    xmm12, xmm0
        pxor    xmm13, xmm1
        pxor    xmm14, xmm2
        pxor    xmm15, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        pshufb  xmm15, xmm8
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm12
        paddd   xmm9, xmm13
        paddd   xmm10, xmm14
        paddd   xmm11, xmm15
        pxor    xmm4, xmm8
        pxor    xmm5, xmm9
        pxor    xmm6, xmm10
        pxor    xmm7, xmm11
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xE0]
        paddd   xmm1, xmmword ptr [rsp+0x20]
        paddd   xmm2, xmmword ptr [rsp+0x30]
        paddd   xmm3, xmmword ptr [rsp+0x70]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        movdqa  xmmword ptr [rsp+0x100], xmm8
        movdqa  xmm8, xmm5
        psrld   xmm8, 12
        pslld   xmm5, 20
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 12
        pslld   xmm6, 20
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 12
        pslld   xmm7, 20
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 12
        pslld   xmm4, 20
        por     xmm4, xmm8
        paddd   xmm0, xmmword ptr [rsp+0xA0]
        paddd   xmm1, xmmword ptr [rsp+0xC0]
        paddd   xmm2, xmmword ptr [rsp+0x40]
        paddd   xmm3, xmmword ptr [rsp+0xD0]
        paddd   xmm0, xmm5
        paddd   xmm1, xmm6
        paddd   xmm2, xmm7
        paddd   xmm3, xmm4
        pxor    xmm15, xmm0
        pxor    xmm12, xmm1
        pxor    xmm13, xmm2
        pxor    xmm14, xmm3
        movdqa  xmm8, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm15, xmm8
        pshufb  xmm12, xmm8
        pshufb  xmm13, xmm8
        pshufb  xmm14, xmm8
        paddd   xmm10, xmm15
        paddd   xmm11, xmm12
        movdqa  xmm8, xmmword ptr [rsp+0x100]
        paddd   xmm8, xmm13
        paddd   xmm9, xmm14
        pxor    xmm5, xmm10
        pxor    xmm6, xmm11
        pxor    xmm7, xmm8
        pxor    xmm4, xmm9
        pxor    xmm0, xmm8
        pxor    xmm1, xmm9
        pxor    xmm2, xmm10
        pxor    xmm3, xmm11
        movdqa  xmm8, xmm5
        psrld   xmm8, 7
        pslld   xmm5, 25
        por     xmm5, xmm8
        movdqa  xmm8, xmm6
        psrld   xmm8, 7
        pslld   xmm6, 25
        por     xmm6, xmm8
        movdqa  xmm8, xmm7
        psrld   xmm8, 7
        pslld   xmm7, 25
        por     xmm7, xmm8
        movdqa  xmm8, xmm4
        psrld   xmm8, 7
        pslld   xmm4, 25
        por     xmm4, xmm8
        pxor    xmm4, xmm12
        pxor    xmm5, xmm13
        pxor    xmm6, xmm14
        pxor    xmm7, xmm15
        mov     eax, r13d
        jne     9b
        movdqa  xmm9, xmm0
        punpckldq xmm0, xmm1
        punpckhdq xmm9, xmm1
        movdqa  xmm11, xmm2
        punpckldq xmm2, xmm3
        punpckhdq xmm11, xmm3
        movdqa  xmm1, xmm0
        punpcklqdq xmm0, xmm2
        punpckhqdq xmm1, xmm2
        movdqa  xmm3, xmm9
        punpcklqdq xmm9, xmm11
        punpckhqdq xmm3, xmm11
        movdqu  xmmword ptr [rbx], xmm0
        movdqu  xmmword ptr [rbx+0x20], xmm1
        movdqu  xmmword ptr [rbx+0x40], xmm9
        movdqu  xmmword ptr [rbx+0x60], xmm3
        movdqa  xmm9, xmm4
        punpckldq xmm4, xmm5
        punpckhdq xmm9, xmm5
        movdqa  xmm11, xmm6
        punpckldq xmm6, xmm7
        punpckhdq xmm11, xmm7
        movdqa  xmm5, xmm4
        punpcklqdq xmm4, xmm6
        punpckhqdq xmm5, xmm6
        movdqa  xmm7, xmm9
        punpcklqdq xmm9, xmm11
        punpckhqdq xmm7, xmm11
        movdqu  xmmword ptr [rbx+0x10], xmm4
        movdqu  xmmword ptr [rbx+0x30], xmm5
        movdqu  xmmword ptr [rbx+0x50], xmm9
        movdqu  xmmword ptr [rbx+0x70], xmm7
        movdqa  xmm1, xmmword ptr [rsp+0x110]
        movdqa  xmm0, xmm1
        paddd   xmm1, xmmword ptr [rsp+0x150]
        movdqa  xmmword ptr [rsp+0x110], xmm1
        pxor    xmm0, xmmword ptr [CMP_MSB_MASK_sse41+rip]
        pxor    xmm1, xmmword ptr [CMP_MSB_MASK_sse41+rip]
        pcmpgtd xmm0, xmm1
        movdqa  xmm1, xmmword ptr [rsp+0x120]
        psubd   xmm1, xmm0
        movdqa  xmmword ptr [rsp+0x120], xmm1
        add     rbx, 128
        add     rdi, 32
        sub     rsi, 4
        cmp     rsi, 4
        jnc     2b
        test    rsi, rsi
        jne     3f
4:
        movdqa  xmm6, xmmword ptr [rsp+0x170]
        movdqa  xmm7, xmmword ptr [rsp+0x180]
        movdqa  xmm8, xmmword ptr [rsp+0x190]
        movdqa  xmm9, xmmword ptr [rsp+0x1A0]
        movdqa  xmm10, xmmword ptr [rsp+0x1B0]
        movdqa  xmm11, xmmword ptr [rsp+0x1C0]
        movdqa  xmm12, xmmword ptr [rsp+0x1D0]
        movdqa  xmm13, xmmword ptr [rsp+0x1E0]
        movdqa  xmm14, xmmword ptr [rsp+0x1F0]
        movdqa  xmm15, xmmword ptr [rsp+0x200]
        mov     rsp, rbp
        pop     rbp
        pop     rbx
        pop     rdi
        pop     rsi
        pop     r12
        pop     r13
        pop     r14
        pop     r15
        ret
.p2align 5
3:
        test    esi, 0x2
        je      3f
        movups  xmm0, xmmword ptr [rcx]
        movups  xmm1, xmmword ptr [rcx+0x10]
        movaps  xmm8, xmm0
        movaps  xmm9, xmm1
        movd    xmm13, dword ptr [rsp+0x110]
        pinsrd  xmm13, dword ptr [rsp+0x120], 1
        pinsrd  xmm13, dword ptr [BLAKE3_BLOCK_LEN_sse41+rip], 2
        movaps  xmmword ptr [rsp], xmm13
        movd    xmm14, dword ptr [rsp+0x114]
        pinsrd  xmm14, dword ptr [rsp+0x124], 1
        pinsrd  xmm14, dword ptr [BLAKE3_BLOCK_LEN_sse41+rip], 2
        movaps  xmmword ptr [rsp+0x10], xmm14
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
2:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        movaps  xmm2, xmmword ptr [BLAKE3_IV_sse41+rip]
        movaps  xmm10, xmm2
        movups  xmm4, xmmword ptr [r8+rdx-0x40]
        movups  xmm5, xmmword ptr [r8+rdx-0x30]
        movaps  xmm3, xmm4
        shufps  xmm4, xmm5, 136
        shufps  xmm3, xmm5, 221
        movaps  xmm5, xmm3
        movups  xmm6, xmmword ptr [r8+rdx-0x20]
        movups  xmm7, xmmword ptr [r8+rdx-0x10]
        movaps  xmm3, xmm6
        shufps  xmm6, xmm7, 136
        pshufd  xmm6, xmm6, 0x93
        shufps  xmm3, xmm7, 221
        pshufd  xmm7, xmm3, 0x93
        movups  xmm12, xmmword ptr [r9+rdx-0x40]
        movups  xmm13, xmmword ptr [r9+rdx-0x30]
        movaps  xmm11, xmm12
        shufps  xmm12, xmm13, 136
        shufps  xmm11, xmm13, 221
        movaps  xmm13, xmm11
        movups  xmm14, xmmword ptr [r9+rdx-0x20]
        movups  xmm15, xmmword ptr [r9+rdx-0x10]
        movaps  xmm11, xmm14
        shufps  xmm14, xmm15, 136
        pshufd  xmm14, xmm14, 0x93
        shufps  xmm11, xmm15, 221
        pshufd  xmm15, xmm11, 0x93
        movaps  xmm3, xmmword ptr [rsp]
        movaps  xmm11, xmmword ptr [rsp+0x10]
        pinsrd  xmm3, eax, 3
        pinsrd  xmm11, eax, 3
        mov     al, 7
9:
        paddd   xmm0, xmm4
        paddd   xmm8, xmm12
        movaps  xmmword ptr [rsp+0x20], xmm4
        movaps  xmmword ptr [rsp+0x30], xmm12
        paddd   xmm0, xmm1
        paddd   xmm8, xmm9
        pxor    xmm3, xmm0
        pxor    xmm11, xmm8
        movaps  xmm12, xmmword ptr [ROT16_sse41+rip]
        pshufb  xmm3, xmm12
        pshufb  xmm11, xmm12
        paddd   xmm2, xmm3
        paddd   xmm10, xmm11
        pxor    xmm1, xmm2
        pxor    xmm9, xmm10
        movdqa  xmm4, xmm1
        pslld   xmm1, 20
        psrld   xmm4, 12
        por     xmm1, xmm4
        movdqa  xmm4, xmm9
        pslld   xmm9, 20
        psrld   xmm4, 12
        por     xmm9, xmm4
        paddd   xmm0, xmm5
        paddd   xmm8, xmm13
        movaps  xmmword ptr [rsp+0x40], xmm5
        movaps  xmmword ptr [rsp+0x50], xmm13
        paddd   xmm0, xmm1
        paddd   xmm8, xmm9
        pxor    xmm3, xmm0
        pxor    xmm11, xmm8
        movaps  xmm13, xmmword ptr [ROT8_sse41+rip]
        pshufb  xmm3, xmm13
        pshufb  xmm11, xmm13
        paddd   xmm2, xmm3
        paddd   xmm10, xmm11
        pxor    xmm1, xmm2
        pxor    xmm9, xmm10
        movdqa  xmm4, xmm1
        pslld   xmm1, 25
        psrld   xmm4, 7
        por     xmm1, xmm4
        movdqa  xmm4, xmm9
        pslld   xmm9, 25
        psrld   xmm4, 7
        por     xmm9, xmm4
        pshufd  xmm0, xmm0, 0x93
        pshufd  xmm8, xmm8, 0x93
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm11, xmm11, 0x4E
        pshufd  xmm2, xmm2, 0x39
        pshufd  xmm10, xmm10, 0x39
        paddd   xmm0, xmm6
        paddd   xmm8, xmm14
        paddd   xmm0, xmm1
        paddd   xmm8, xmm9
        pxor    xmm3, xmm0
        pxor    xmm11, xmm8
        pshufb  xmm3, xmm12
        pshufb  xmm11, xmm12
        paddd   xmm2, xmm3
        paddd   xmm10, xmm11
        pxor    xmm1, xmm2
        pxor    xmm9, xmm10
        movdqa  xmm4, xmm1
        pslld   xmm1, 20
        psrld   xmm4, 12
        por     xmm1, xmm4
        movdqa  xmm4, xmm9
        pslld   xmm9, 20
        psrld   xmm4, 12
        por     xmm9, xmm4
        paddd   xmm0, xmm7
        paddd   xmm8, xmm15
        paddd   xmm0, xmm1
        paddd   xmm8, xmm9
        pxor    xmm3, xmm0
        pxor    xmm11, xmm8
        pshufb  xmm3, xmm13
        pshufb  xmm11, xmm13
        paddd   xmm2, xmm3
        paddd   xmm10, xmm11
        pxor    xmm1, xmm2
        pxor    xmm9, xmm10
        movdqa  xmm4, xmm1
        pslld   xmm1, 25
        psrld   xmm4, 7
        por     xmm1, xmm4
        movdqa  xmm4, xmm9
        pslld   xmm9, 25
        psrld   xmm4, 7
        por     xmm9, xmm4
        pshufd  xmm0, xmm0, 0x39
        pshufd  xmm8, xmm8, 0x39
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm11, xmm11, 0x4E
        pshufd  xmm2, xmm2, 0x93
        pshufd  xmm10, xmm10, 0x93
        dec     al
        je      9f
        movdqa  xmm12, xmmword ptr [rsp+0x20]
        movdqa  xmm5, xmmword ptr [rsp+0x40]
        pshufd  xmm13, xmm12, 0x0F
        shufps  xmm12, xmm5, 214
        pshufd  xmm4, xmm12, 0x39
        movdqa  xmm12, xmm6
        shufps  xmm12, xmm7, 250
        pblendw xmm13, xmm12, 0xCC
        movdqa  xmm12, xmm7
        punpcklqdq xmm12, xmm5
        pblendw xmm12, xmm6, 0xC0
        pshufd  xmm12, xmm12, 0x78
        punpckhdq xmm5, xmm7
        punpckldq xmm6, xmm5
        pshufd  xmm7, xmm6, 0x1E
        movdqa  xmmword ptr [rsp+0x20], xmm13
        movdqa  xmmword ptr [rsp+0x40], xmm12
        movdqa  xmm5, xmmword ptr [rsp+0x30]
        movdqa  xmm13, xmmword ptr [rsp+0x50]
        pshufd  xmm6, xmm5, 0x0F
        shufps  xmm5, xmm13, 214
        pshufd  xmm12, xmm5, 0x39
        movdqa  xmm5, xmm14
        shufps  xmm5, xmm15, 250
        pblendw xmm6, xmm5, 0xCC
        movdqa  xmm5, xmm15
        punpcklqdq xmm5, xmm13
        pblendw xmm5, xmm14, 0xC0
        pshufd  xmm5, xmm5, 0x78
        punpckhdq xmm13, xmm15
        punpckldq xmm14, xmm13
        pshufd  xmm15, xmm14, 0x1E
        movdqa  xmm13, xmm6
        movdqa  xmm14, xmm5
        movdqa  xmm5, xmmword ptr [rsp+0x20]
        movdqa  xmm6, xmmword ptr [rsp+0x40]
        jmp     9b
9:
        pxor    xmm0, xmm2
        pxor    xmm1, xmm3
        pxor    xmm8, xmm10
        pxor    xmm9, xmm11
        mov     eax, r13d
        cmp     rdx, r15
        jne     2b
        movups  xmmword ptr [rbx], xmm0
        movups  xmmword ptr [rbx+0x10], xmm1
        movups  xmmword ptr [rbx+0x20], xmm8
        movups  xmmword ptr [rbx+0x30], xmm9
        movdqa  xmm0, xmmword ptr [rsp+0x130]
        movdqa  xmm1, xmmword ptr [rsp+0x110]
        movdqa  xmm2, xmmword ptr [rsp+0x120]
        movdqu  xmm3, xmmword ptr [rsp+0x118]
        movdqu  xmm4, xmmword ptr [rsp+0x128]
        blendvps xmm1, xmm3, xmm0
        blendvps xmm2, xmm4, xmm0
        movdqa  xmmword ptr [rsp+0x110], xmm1
        movdqa  xmmword ptr [rsp+0x120], xmm2
        add     rdi, 16
        add     rbx, 64
        sub     rsi, 2
3:
        test    esi, 0x1
        je      4b
        movups  xmm0, xmmword ptr [rcx]
        movups  xmm1, xmmword ptr [rcx+0x10]
        movd    xmm13, dword ptr [rsp+0x110]
        pinsrd  xmm13, dword ptr [rsp+0x120], 1
        pinsrd  xmm13, dword ptr [BLAKE3_BLOCK_LEN_sse41+rip], 2
        movaps  xmm14, xmmword ptr [ROT8_sse41+rip]
        movaps  xmm15, xmmword ptr [ROT16_sse41+rip]
        mov     r8, qword ptr [rdi]
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
2:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        movaps  xmm2, xmmword ptr [BLAKE3_IV_sse41+rip]
        movaps  xmm3, xmm13
        pinsrd  xmm3, eax, 3
        movups  xmm4, xmmword ptr [r8+rdx-0x40]
        movups  xmm5, xmmword ptr [r8+rdx-0x30]
        movaps  xmm8, xmm4
        shufps  xmm4, xmm5, 136
        shufps  xmm8, xmm5, 221
        movaps  xmm5, xmm8
        movups  xmm6, xmmword ptr [r8+rdx-0x20]
        movups  xmm7, xmmword ptr [r8+rdx-0x10]
        movaps  xmm8, xmm6
        shufps  xmm6, xmm7, 136
        pshufd  xmm6, xmm6, 0x93
        shufps  xmm8, xmm7, 221
        pshufd  xmm7, xmm8, 0x93
        mov     al, 7
9:
        paddd   xmm0, xmm4
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm15
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm5
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x93
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x39
        paddd   xmm0, xmm6
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm15
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm7
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x39
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x93
        dec     al
        jz      9f
        movdqa  xmm8, xmm4
        shufps  xmm8, xmm5, 214
        pshufd  xmm9, xmm4, 0x0F
        pshufd  xmm4, xmm8, 0x39
        movdqa  xmm8, xmm6
        shufps  xmm8, xmm7, 250
        pblendw xmm9, xmm8, 0xCC
        movdqa  xmm8, xmm7
        punpcklqdq xmm8, xmm5
        pblendw xmm8, xmm6, 0xC0
        pshufd  xmm8, xmm8, 0x78
        punpckhdq xmm5, xmm7
        punpckldq xmm6, xmm5
        pshufd  xmm7, xmm6, 0x1E
        movdqa  xmm5, xmm9
        movdqa  xmm6, xmm8
        jmp     9b
9:
        pxor    xmm0, xmm2
        pxor    xmm1, xmm3
        mov     eax, r13d
        cmp     rdx, r15
        jne     2b
        movups  xmmword ptr [rbx], xmm0
        movups  xmmword ptr [rbx+0x10], xmm1
        jmp     4b

.p2align 6
blake3_compress_in_place_sse41:
_blake3_compress_in_place_sse41:
        sub     rsp, 120
        movdqa  xmmword ptr [rsp], xmm6
        movdqa  xmmword ptr [rsp+0x10], xmm7
        movdqa  xmmword ptr [rsp+0x20], xmm8
        movdqa  xmmword ptr [rsp+0x30], xmm9
        movdqa  xmmword ptr [rsp+0x40], xmm11
        movdqa  xmmword ptr [rsp+0x50], xmm14
        movdqa  xmmword ptr [rsp+0x60], xmm15
        movups  xmm0, xmmword ptr [rcx]
        movups  xmm1, xmmword ptr [rcx+0x10]
        movaps  xmm2, xmmword ptr [BLAKE3_IV_sse41+rip]
        movzx   eax, byte ptr [rsp+0xA0]
        movzx   r8d, r8b
        shl     rax, 32
        add     r8, rax
        movq    xmm3, r9
        movq    xmm4, r8
        punpcklqdq xmm3, xmm4
        movups  xmm4, xmmword ptr [rdx]
        movups  xmm5, xmmword ptr [rdx+0x10]
        movaps  xmm8, xmm4
        shufps  xmm4, xmm5, 136
        shufps  xmm8, xmm5, 221
        movaps  xmm5, xmm8
        movups  xmm6, xmmword ptr [rdx+0x20]
        movups  xmm7, xmmword ptr [rdx+0x30]
        movaps  xmm8, xmm6
        shufps  xmm6, xmm7, 136
        pshufd  xmm6, xmm6, 0x93
        shufps  xmm8, xmm7, 221
        pshufd  xmm7, xmm8, 0x93
        movaps  xmm14, xmmword ptr [ROT8_sse41+rip]
        movaps  xmm15, xmmword ptr [ROT16_sse41+rip]
        mov     al, 7
9:
        paddd   xmm0, xmm4
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm15
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm5
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x93
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x39
        paddd   xmm0, xmm6
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm15
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm7
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x39
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x93
        dec     al
        jz      9f
        movdqa  xmm8, xmm4
        shufps  xmm8, xmm5, 214
        pshufd  xmm9, xmm4, 0x0F
        pshufd  xmm4, xmm8, 0x39
        movdqa  xmm8, xmm6
        shufps  xmm8, xmm7, 250
        pblendw xmm9, xmm8, 0xCC
        movdqa  xmm8, xmm7
        punpcklqdq xmm8, xmm5
        pblendw xmm8, xmm6, 0xC0
        pshufd  xmm8, xmm8, 0x78
        punpckhdq xmm5, xmm7
        punpckldq xmm6, xmm5
        pshufd  xmm7, xmm6, 0x1E
        movdqa  xmm5, xmm9
        movdqa  xmm6, xmm8
        jmp     9b
9:
        pxor    xmm0, xmm2
        pxor    xmm1, xmm3
        movups  xmmword ptr [rcx], xmm0
        movups  xmmword ptr [rcx+0x10], xmm1
        movdqa  xmm6, xmmword ptr [rsp]
        movdqa  xmm7, xmmword ptr [rsp+0x10]
        movdqa  xmm8, xmmword ptr [rsp+0x20]
        movdqa  xmm9, xmmword ptr [rsp+0x30]
        movdqa  xmm11, xmmword ptr [rsp+0x40]
        movdqa  xmm14, xmmword ptr [rsp+0x50]
        movdqa  xmm15, xmmword ptr [rsp+0x60]
        add     rsp, 120
        ret


.p2align 6
_blake3_compress_xof_sse41:
blake3_compress_xof_sse41:
        sub     rsp, 120
        movdqa  xmmword ptr [rsp], xmm6
        movdqa  xmmword ptr [rsp+0x10], xmm7
        movdqa  xmmword ptr [rsp+0x20], xmm8
        movdqa  xmmword ptr [rsp+0x30], xmm9
        movdqa  xmmword ptr [rsp+0x40], xmm11
        movdqa  xmmword ptr [rsp+0x50], xmm14
        movdqa  xmmword ptr [rsp+0x60], xmm15
        movups  xmm0, xmmword ptr [rcx]
        movups  xmm1, xmmword ptr [rcx+0x10]
        movaps  xmm2, xmmword ptr [BLAKE3_IV_sse41+rip]
        movzx   eax, byte ptr [rsp+0xA0]
        movzx   r8d, r8b
        mov     r10, qword ptr [rsp+0xA8]
        shl     rax, 32
        add     r8, rax
        movq    xmm3, r9
        movq    xmm4, r8
        punpcklqdq xmm3, xmm4
        movups  xmm4, xmmword ptr [rdx]
        movups  xmm5, xmmword ptr [rdx+0x10]
        movaps  xmm8, xmm4
        shufps  xmm4, xmm5, 136
        shufps  xmm8, xmm5, 221
        movaps  xmm5, xmm8
        movups  xmm6, xmmword ptr [rdx+0x20]
        movups  xmm7, xmmword ptr [rdx+0x30]
        movaps  xmm8, xmm6
        shufps  xmm6, xmm7, 136
        pshufd  xmm6, xmm6, 0x93
        shufps  xmm8, xmm7, 221
        pshufd  xmm7, xmm8, 0x93
        movaps  xmm14, xmmword ptr [ROT8_sse41+rip]
        movaps  xmm15, xmmword ptr [ROT16_sse41+rip]
        mov     al, 7
9:
        paddd   xmm0, xmm4
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm15
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm5
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x93
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x39
        paddd   xmm0, xmm6
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm15
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 20
        psrld   xmm11, 12
        por     xmm1, xmm11
        paddd   xmm0, xmm7
        paddd   xmm0, xmm1
        pxor    xmm3, xmm0
        pshufb  xmm3, xmm14
        paddd   xmm2, xmm3
        pxor    xmm1, xmm2
        movdqa  xmm11, xmm1
        pslld   xmm1, 25
        psrld   xmm11, 7
        por     xmm1, xmm11
        pshufd  xmm0, xmm0, 0x39
        pshufd  xmm3, xmm3, 0x4E
        pshufd  xmm2, xmm2, 0x93
        dec     al
        jz      9f
        movdqa  xmm8, xmm4
        shufps  xmm8, xmm5, 214
        pshufd  xmm9, xmm4, 0x0F
        pshufd  xmm4, xmm8, 0x39
        movdqa  xmm8, xmm6
        shufps  xmm8, xmm7, 250
        pblendw xmm9, xmm8, 0xCC
        movdqa  xmm8, xmm7
        punpcklqdq xmm8, xmm5
        pblendw xmm8, xmm6, 0xC0
        pshufd  xmm8, xmm8, 0x78
        punpckhdq xmm5, xmm7
        punpckldq xmm6, xmm5
        pshufd  xmm7, xmm6, 0x1E
        movdqa  xmm5, xmm9
        movdqa  xmm6, xmm8
        jmp     9b
9:
        movdqu  xmm4, xmmword ptr [rcx]
        movdqu  xmm5, xmmword ptr [rcx+0x10]
        pxor    xmm0, xmm2
        pxor    xmm1, xmm3
        pxor    xmm2, xmm4
        pxor    xmm3, xmm5
        movups  xmmword ptr [r10], xmm0
        movups  xmmword ptr [r10+0x10], xmm1
        movups  xmmword ptr [r10+0x20], xmm2
        movups  xmmword ptr [r10+0x30], xmm3
        movdqa  xmm6, xmmword ptr [rsp]
        movdqa  xmm7, xmmword ptr [rsp+0x10]
        movdqa  xmm8, xmmword ptr [rsp+0x20]
        movdqa  xmm9, xmmword ptr [rsp+0x30]
        movdqa  xmm11, xmmword ptr [rsp+0x40]
        movdqa  xmm14, xmmword ptr [rsp+0x50]
        movdqa  xmm15, xmmword ptr [rsp+0x60]
        add     rsp, 120
        ret


.section .rodata
.p2align  6
BLAKE3_IV_sse41:
        .long  0x6A09E667, 0xBB67AE85
        .long  0x3C6EF372, 0xA54FF53A
ROT16_sse41:
        .byte  2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13
ROT8_sse41:
        .byte  1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 8, 13, 14, 15, 12
ADD0_sse41:   
        .long  0, 1, 2, 3
ADD1_sse41:
        .long  4, 4, 4, 4
 BLAKE3_IV_0_sse41:
        .long  0x6A09E667, 0x6A09E667, 0x6A09E667, 0x6A09E667
 BLAKE3_IV_1_sse41:
        .long  0xBB67AE85, 0xBB67AE85, 0xBB67AE85, 0xBB67AE85
 BLAKE3_IV_2_sse41:
        .long  0x3C6EF372, 0x3C6EF372, 0x3C6EF372, 0x3C6EF372
 BLAKE3_IV_3_sse41:
        .long  0xA54FF53A, 0xA54FF53A, 0xA54FF53A, 0xA54FF53A
BLAKE3_BLOCK_LEN_sse41:
        .long  64, 64, 64, 64
CMP_MSB_MASK_sse41:
        .long  0x80000000, 0x80000000, 0x80000000, 0x80000000





















.intel_syntax noprefix
.global _blake3_hash_many_avx2
.global blake3_hash_many_avx2
.section .text
        .p2align  6
_blake3_hash_many_avx2:
blake3_hash_many_avx2:
        push    r15
        push    r14
        push    r13
        push    r12
        push    rsi
        push    rdi
        push    rbx
        push    rbp
        mov     rbp, rsp
        sub     rsp, 880
        and     rsp, 0xFFFFFFFFFFFFFFC0
        vmovdqa xmmword ptr [rsp+0x2D0], xmm6
        vmovdqa xmmword ptr [rsp+0x2E0], xmm7
        vmovdqa xmmword ptr [rsp+0x2F0], xmm8
        vmovdqa xmmword ptr [rsp+0x300], xmm9
        vmovdqa xmmword ptr [rsp+0x310], xmm10
        vmovdqa xmmword ptr [rsp+0x320], xmm11
        vmovdqa xmmword ptr [rsp+0x330], xmm12
        vmovdqa xmmword ptr [rsp+0x340], xmm13
        vmovdqa xmmword ptr [rsp+0x350], xmm14
        vmovdqa xmmword ptr [rsp+0x360], xmm15
        mov     rdi, rcx
        mov     rsi, rdx
        mov     rdx, r8
        mov     rcx, r9
        mov     r8, qword ptr [rbp+0x68]
        movzx   r9, byte ptr [rbp+0x70]
        neg     r9d
        vmovd   xmm0, r9d
        vpbroadcastd ymm0, xmm0
        vmovdqa ymmword ptr [rsp+0x260], ymm0
        vpand   ymm1, ymm0, ymmword ptr [ADD0+rip]
        vpand   ymm2, ymm0, ymmword ptr [ADD1+rip]
        vmovdqa ymmword ptr [rsp+0x2A0], ymm2
        vmovd   xmm2, r8d
        vpbroadcastd ymm2, xmm2
        vpaddd  ymm2, ymm2, ymm1
        vmovdqa ymmword ptr [rsp+0x220], ymm2
        vpxor   ymm1, ymm1, ymmword ptr [CMP_MSB_MASK+rip]
        vpxor   ymm2, ymm2, ymmword ptr [CMP_MSB_MASK+rip]
        vpcmpgtd ymm2, ymm1, ymm2
        shr     r8, 32
        vmovd   xmm3, r8d
        vpbroadcastd ymm3, xmm3
        vpsubd  ymm3, ymm3, ymm2
        vmovdqa ymmword ptr [rsp+0x240], ymm3
        shl     rdx, 6
        mov     qword ptr [rsp+0x2C0], rdx
        cmp     rsi, 8
        jc      3f
2:
        vpbroadcastd ymm0, dword ptr [rcx]
        vpbroadcastd ymm1, dword ptr [rcx+0x4]
        vpbroadcastd ymm2, dword ptr [rcx+0x8]
        vpbroadcastd ymm3, dword ptr [rcx+0xC]
        vpbroadcastd ymm4, dword ptr [rcx+0x10]
        vpbroadcastd ymm5, dword ptr [rcx+0x14]
        vpbroadcastd ymm6, dword ptr [rcx+0x18]
        vpbroadcastd ymm7, dword ptr [rcx+0x1C]
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        mov     r10, qword ptr [rdi+0x10]
        mov     r11, qword ptr [rdi+0x18]
        mov     r12, qword ptr [rdi+0x20]
        mov     r13, qword ptr [rdi+0x28]
        mov     r14, qword ptr [rdi+0x30]
        mov     r15, qword ptr [rdi+0x38]
        movzx   eax, byte ptr [rbp+0x78]
        movzx   ebx, byte ptr [rbp+0x80]
        or      eax, ebx
        xor     edx, edx
.p2align 5
9:
        movzx   ebx, byte ptr [rbp+0x88]
        or      ebx, eax
        add     rdx, 64
        cmp     rdx, qword ptr [rsp+0x2C0]
        cmove   eax, ebx
        mov     dword ptr [rsp+0x200], eax
        vmovups xmm8, xmmword ptr [r8+rdx-0x40]
        vinsertf128 ymm8, ymm8, xmmword ptr [r12+rdx-0x40], 0x01
        vmovups xmm9, xmmword ptr [r9+rdx-0x40]
        vinsertf128 ymm9, ymm9, xmmword ptr [r13+rdx-0x40], 0x01
        vunpcklpd ymm12, ymm8, ymm9
        vunpckhpd ymm13, ymm8, ymm9
        vmovups xmm10, xmmword ptr [r10+rdx-0x40]
        vinsertf128 ymm10, ymm10, xmmword ptr [r14+rdx-0x40], 0x01
        vmovups xmm11, xmmword ptr [r11+rdx-0x40]
        vinsertf128 ymm11, ymm11, xmmword ptr [r15+rdx-0x40], 0x01
        vunpcklpd ymm14, ymm10, ymm11
        vunpckhpd ymm15, ymm10, ymm11
        vshufps ymm8, ymm12, ymm14, 136
        vmovaps ymmword ptr [rsp], ymm8
        vshufps ymm9, ymm12, ymm14, 221
        vmovaps ymmword ptr [rsp+0x20], ymm9
        vshufps ymm10, ymm13, ymm15, 136
        vmovaps ymmword ptr [rsp+0x40], ymm10
        vshufps ymm11, ymm13, ymm15, 221
        vmovaps ymmword ptr [rsp+0x60], ymm11
        vmovups xmm8, xmmword ptr [r8+rdx-0x30]
        vinsertf128 ymm8, ymm8, xmmword ptr [r12+rdx-0x30], 0x01
        vmovups xmm9, xmmword ptr [r9+rdx-0x30]
        vinsertf128 ymm9, ymm9, xmmword ptr [r13+rdx-0x30], 0x01
        vunpcklpd ymm12, ymm8, ymm9
        vunpckhpd ymm13, ymm8, ymm9
        vmovups xmm10, xmmword ptr [r10+rdx-0x30]
        vinsertf128 ymm10, ymm10, xmmword ptr [r14+rdx-0x30], 0x01
        vmovups xmm11, xmmword ptr [r11+rdx-0x30]
        vinsertf128 ymm11, ymm11, xmmword ptr [r15+rdx-0x30], 0x01
        vunpcklpd ymm14, ymm10, ymm11
        vunpckhpd ymm15, ymm10, ymm11
        vshufps ymm8, ymm12, ymm14, 136
        vmovaps ymmword ptr [rsp+0x80], ymm8
        vshufps ymm9, ymm12, ymm14, 221
        vmovaps ymmword ptr [rsp+0xA0], ymm9
        vshufps ymm10, ymm13, ymm15, 136
        vmovaps ymmword ptr [rsp+0xC0], ymm10
        vshufps ymm11, ymm13, ymm15, 221
        vmovaps ymmword ptr [rsp+0xE0], ymm11
        vmovups xmm8, xmmword ptr [r8+rdx-0x20]
        vinsertf128 ymm8, ymm8, xmmword ptr [r12+rdx-0x20], 0x01
        vmovups xmm9, xmmword ptr [r9+rdx-0x20]
        vinsertf128 ymm9, ymm9, xmmword ptr [r13+rdx-0x20], 0x01
        vunpcklpd ymm12, ymm8, ymm9
        vunpckhpd ymm13, ymm8, ymm9
        vmovups xmm10, xmmword ptr [r10+rdx-0x20]
        vinsertf128 ymm10, ymm10, xmmword ptr [r14+rdx-0x20], 0x01
        vmovups xmm11, xmmword ptr [r11+rdx-0x20]
        vinsertf128 ymm11, ymm11, xmmword ptr [r15+rdx-0x20], 0x01
        vunpcklpd ymm14, ymm10, ymm11
        vunpckhpd ymm15, ymm10, ymm11
        vshufps ymm8, ymm12, ymm14, 136
        vmovaps ymmword ptr [rsp+0x100], ymm8
        vshufps ymm9, ymm12, ymm14, 221
        vmovaps ymmword ptr [rsp+0x120], ymm9
        vshufps ymm10, ymm13, ymm15, 136
        vmovaps ymmword ptr [rsp+0x140], ymm10
        vshufps ymm11, ymm13, ymm15, 221
        vmovaps ymmword ptr [rsp+0x160], ymm11
        vmovups xmm8, xmmword ptr [r8+rdx-0x10]
        vinsertf128 ymm8, ymm8, xmmword ptr [r12+rdx-0x10], 0x01
        vmovups xmm9, xmmword ptr [r9+rdx-0x10]
        vinsertf128 ymm9, ymm9, xmmword ptr [r13+rdx-0x10], 0x01
        vunpcklpd ymm12, ymm8, ymm9
        vunpckhpd ymm13, ymm8, ymm9
        vmovups xmm10, xmmword ptr [r10+rdx-0x10]
        vinsertf128 ymm10, ymm10, xmmword ptr [r14+rdx-0x10], 0x01
        vmovups xmm11, xmmword ptr [r11+rdx-0x10]
        vinsertf128 ymm11, ymm11, xmmword ptr [r15+rdx-0x10], 0x01
        vunpcklpd ymm14, ymm10, ymm11
        vunpckhpd ymm15, ymm10, ymm11
        vshufps ymm8, ymm12, ymm14, 136
        vmovaps ymmword ptr [rsp+0x180], ymm8
        vshufps ymm9, ymm12, ymm14, 221
        vmovaps ymmword ptr [rsp+0x1A0], ymm9
        vshufps ymm10, ymm13, ymm15, 136
        vmovaps ymmword ptr [rsp+0x1C0], ymm10
        vshufps ymm11, ymm13, ymm15, 221
        vmovaps ymmword ptr [rsp+0x1E0], ymm11
        vpbroadcastd ymm15, dword ptr [rsp+0x200]
        prefetcht0 [r8+rdx+0x80]
        prefetcht0 [r12+rdx+0x80]
        prefetcht0 [r9+rdx+0x80]
        prefetcht0 [r13+rdx+0x80]
        prefetcht0 [r10+rdx+0x80]
        prefetcht0 [r14+rdx+0x80]
        prefetcht0 [r11+rdx+0x80]
        prefetcht0 [r15+rdx+0x80]
        vpaddd  ymm0, ymm0, ymmword ptr [rsp]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x40]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x80]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0xC0]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm0, ymmword ptr [rsp+0x220]
        vpxor   ymm13, ymm1, ymmword ptr [rsp+0x240]
        vpxor   ymm14, ymm2, ymmword ptr [BLAKE3_BLOCK_LEN+rip]
        vpxor   ymm15, ymm3, ymm15
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [BLAKE3_IV_0+rip]
        vpaddd  ymm9, ymm13, ymmword ptr [BLAKE3_IV_1+rip]
        vpaddd  ymm10, ymm14, ymmword ptr [BLAKE3_IV_2+rip]
        vpaddd  ymm11, ymm15, ymmword ptr [BLAKE3_IV_3+rip]
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x20]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x60]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0xA0]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0xE0]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x100]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x140]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x180]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x1C0]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x120]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x160]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x1A0]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x1E0]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x40]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x60]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0xE0]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x80]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0xC0]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x140]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x1A0]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x20]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x180]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x120]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x1E0]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x160]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0xA0]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x1C0]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x100]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x60]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x140]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x1A0]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0xE0]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x80]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x180]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x40]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x1C0]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0xC0]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x120]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x160]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x100]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0xA0]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x1E0]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x20]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x140]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x180]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x1C0]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x1A0]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0xE0]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x120]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x60]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x1E0]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x80]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x160]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0xA0]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x20]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x40]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x100]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0xC0]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x180]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x120]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x1E0]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x1C0]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x1A0]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x160]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x140]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x100]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0xE0]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0xA0]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0xC0]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x40]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x60]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x20]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x80]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x120]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x160]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x100]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x1E0]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x1C0]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0xA0]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x180]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x20]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x1A0]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x40]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x80]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x60]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x140]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0xC0]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0xE0]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x160]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0xA0]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x20]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x100]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x1E0]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x120]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0xC0]
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxor   ymm12, ymm12, ymm0
        vpxor   ymm13, ymm13, ymm1
        vpxor   ymm14, ymm14, ymm2
        vpxor   ymm15, ymm15, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpshufb ymm15, ymm15, ymm8
        vpaddd  ymm8, ymm12, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxor   ymm4, ymm4, ymm8
        vpxor   ymm5, ymm5, ymm9
        vpxor   ymm6, ymm6, ymm10
        vpxor   ymm7, ymm7, ymm11
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x1C0]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x40]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x60]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0xE0]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT16+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vmovdqa ymmword ptr [rsp+0x200], ymm8
        vpsrld  ymm8, ymm5, 12
        vpslld  ymm5, ymm5, 20
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 12
        vpslld  ymm6, ymm6, 20
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 12
        vpslld  ymm7, ymm7, 20
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 12
        vpslld  ymm4, ymm4, 20
        vpor    ymm4, ymm4, ymm8
        vpaddd  ymm0, ymm0, ymmword ptr [rsp+0x140]
        vpaddd  ymm1, ymm1, ymmword ptr [rsp+0x180]
        vpaddd  ymm2, ymm2, ymmword ptr [rsp+0x80]
        vpaddd  ymm3, ymm3, ymmword ptr [rsp+0x1A0]
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxor   ymm15, ymm15, ymm0
        vpxor   ymm12, ymm12, ymm1
        vpxor   ymm13, ymm13, ymm2
        vpxor   ymm14, ymm14, ymm3
        vbroadcasti128 ymm8, xmmword ptr [ROT8+rip]
        vpshufb ymm15, ymm15, ymm8
        vpshufb ymm12, ymm12, ymm8
        vpshufb ymm13, ymm13, ymm8
        vpshufb ymm14, ymm14, ymm8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm13, ymmword ptr [rsp+0x200]
        vpaddd  ymm9, ymm9, ymm14
        vpxor   ymm5, ymm5, ymm10
        vpxor   ymm6, ymm6, ymm11
        vpxor   ymm7, ymm7, ymm8
        vpxor   ymm4, ymm4, ymm9
        vpxor   ymm0, ymm0, ymm8
        vpxor   ymm1, ymm1, ymm9
        vpxor   ymm2, ymm2, ymm10
        vpxor   ymm3, ymm3, ymm11
        vpsrld  ymm8, ymm5, 7
        vpslld  ymm5, ymm5, 25
        vpor    ymm5, ymm5, ymm8
        vpsrld  ymm8, ymm6, 7
        vpslld  ymm6, ymm6, 25
        vpor    ymm6, ymm6, ymm8
        vpsrld  ymm8, ymm7, 7
        vpslld  ymm7, ymm7, 25
        vpor    ymm7, ymm7, ymm8
        vpsrld  ymm8, ymm4, 7
        vpslld  ymm4, ymm4, 25
        vpor    ymm4, ymm4, ymm8
        vpxor   ymm4, ymm4, ymm12
        vpxor   ymm5, ymm5, ymm13
        vpxor   ymm6, ymm6, ymm14
        vpxor   ymm7, ymm7, ymm15
        movzx   eax, byte ptr [rbp+0x78]
        jne     9b
        mov     rbx, qword ptr [rbp+0x90]
        vunpcklps ymm8, ymm0, ymm1
        vunpcklps ymm9, ymm2, ymm3
        vunpckhps ymm10, ymm0, ymm1
        vunpcklps ymm11, ymm4, ymm5
        vunpcklps ymm0, ymm6, ymm7
        vshufps ymm12, ymm8, ymm9, 78
        vblendps ymm1, ymm8, ymm12, 0xCC
        vshufps ymm8, ymm11, ymm0, 78
        vunpckhps ymm13, ymm2, ymm3
        vblendps ymm2, ymm11, ymm8, 0xCC
        vblendps ymm3, ymm12, ymm9, 0xCC
        vperm2f128 ymm12, ymm1, ymm2, 0x20
        vmovups ymmword ptr [rbx], ymm12
        vunpckhps ymm14, ymm4, ymm5
        vblendps ymm4, ymm8, ymm0, 0xCC
        vunpckhps ymm15, ymm6, ymm7
        vperm2f128 ymm7, ymm3, ymm4, 0x20
        vmovups ymmword ptr [rbx+0x20], ymm7
        vshufps ymm5, ymm10, ymm13, 78
        vblendps ymm6, ymm5, ymm13, 0xCC
        vshufps ymm13, ymm14, ymm15, 78
        vblendps ymm10, ymm10, ymm5, 0xCC
        vblendps ymm14, ymm14, ymm13, 0xCC
        vperm2f128 ymm8, ymm10, ymm14, 0x20
        vmovups ymmword ptr [rbx+0x40], ymm8
        vblendps ymm15, ymm13, ymm15, 0xCC
        vperm2f128 ymm13, ymm6, ymm15, 0x20
        vmovups ymmword ptr [rbx+0x60], ymm13
        vperm2f128 ymm9, ymm1, ymm2, 0x31
        vperm2f128 ymm11, ymm3, ymm4, 0x31
        vmovups ymmword ptr [rbx+0x80], ymm9
        vperm2f128 ymm14, ymm10, ymm14, 0x31
        vperm2f128 ymm15, ymm6, ymm15, 0x31
        vmovups ymmword ptr [rbx+0xA0], ymm11
        vmovups ymmword ptr [rbx+0xC0], ymm14
        vmovups ymmword ptr [rbx+0xE0], ymm15
        vmovdqa ymm0, ymmword ptr [rsp+0x2A0]
        vpaddd  ymm1, ymm0, ymmword ptr [rsp+0x220]
        vmovdqa ymmword ptr [rsp+0x220], ymm1
        vpxor   ymm0, ymm0, ymmword ptr [CMP_MSB_MASK+rip]
        vpxor   ymm2, ymm1, ymmword ptr [CMP_MSB_MASK+rip]
        vpcmpgtd ymm2, ymm0, ymm2
        vmovdqa ymm0, ymmword ptr [rsp+0x240]
        vpsubd  ymm2, ymm0, ymm2
        vmovdqa ymmword ptr [rsp+0x240], ymm2
        add     rdi, 64
        add     rbx, 256
        mov     qword ptr [rbp+0x90], rbx
        sub     rsi, 8
        cmp     rsi, 8
        jnc     2b
        test    rsi, rsi
        jnz     3f
4:
        vzeroupper
        vmovdqa xmm6, xmmword ptr [rsp+0x2D0]
        vmovdqa xmm7, xmmword ptr [rsp+0x2E0]
        vmovdqa xmm8, xmmword ptr [rsp+0x2F0]
        vmovdqa xmm9, xmmword ptr [rsp+0x300]
        vmovdqa xmm10, xmmword ptr [rsp+0x310]
        vmovdqa xmm11, xmmword ptr [rsp+0x320]
        vmovdqa xmm12, xmmword ptr [rsp+0x330]
        vmovdqa xmm13, xmmword ptr [rsp+0x340]
        vmovdqa xmm14, xmmword ptr [rsp+0x350]
        vmovdqa xmm15, xmmword ptr [rsp+0x360]
        mov     rsp, rbp
        pop     rbp
        pop     rbx
        pop     rdi
        pop     rsi
        pop     r12
        pop     r13
        pop     r14
        pop     r15
        ret
.p2align 5
3:
        mov     rbx, qword ptr [rbp+0x90]
        mov     r15, qword ptr [rsp+0x2C0]
        movzx   r13d, byte ptr [rbp+0x78]
        movzx   r12d, byte ptr [rbp+0x88]
        test    rsi, 0x4
        je      3f
        vbroadcasti128 ymm0, xmmword ptr [rcx]
        vbroadcasti128 ymm1, xmmword ptr [rcx+0x10]
        vmovdqa ymm8, ymm0
        vmovdqa ymm9, ymm1
        vbroadcasti128 ymm12, xmmword ptr [rsp+0x220]
        vbroadcasti128 ymm13, xmmword ptr [rsp+0x240]
        vpunpckldq ymm14, ymm12, ymm13
        vpunpckhdq ymm15, ymm12, ymm13
        vpermq  ymm14, ymm14, 0x50
        vpermq  ymm15, ymm15, 0x50
        vbroadcasti128 ymm12, xmmword ptr [BLAKE3_BLOCK_LEN+rip]
        vpblendd ymm14, ymm14, ymm12, 0x44
        vpblendd ymm15, ymm15, ymm12, 0x44
        vmovdqa ymmword ptr [rsp], ymm14
        vmovdqa ymmword ptr [rsp+0x20], ymm15
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        mov     r10, qword ptr [rdi+0x10]
        mov     r11, qword ptr [rdi+0x18]
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
.p2align 5
2:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        mov     dword ptr [rsp+0x200], eax
        vmovups ymm2, ymmword ptr [r8+rdx-0x40]
        vinsertf128 ymm2, ymm2, xmmword ptr [r9+rdx-0x40], 0x01
        vmovups ymm3, ymmword ptr [r8+rdx-0x30]
        vinsertf128 ymm3, ymm3, xmmword ptr [r9+rdx-0x30], 0x01
        vshufps ymm4, ymm2, ymm3, 136
        vshufps ymm5, ymm2, ymm3, 221
        vmovups ymm2, ymmword ptr [r8+rdx-0x20]
        vinsertf128 ymm2, ymm2, xmmword ptr [r9+rdx-0x20], 0x01
        vmovups ymm3, ymmword ptr [r8+rdx-0x10]
        vinsertf128 ymm3, ymm3, xmmword ptr [r9+rdx-0x10], 0x01
        vshufps ymm6, ymm2, ymm3, 136
        vshufps ymm7, ymm2, ymm3, 221
        vpshufd ymm6, ymm6, 0x93
        vpshufd ymm7, ymm7, 0x93
        vmovups ymm10, ymmword ptr [r10+rdx-0x40]
        vinsertf128 ymm10, ymm10, xmmword ptr [r11+rdx-0x40], 0x01
        vmovups ymm11, ymmword ptr [r10+rdx-0x30]
        vinsertf128 ymm11, ymm11, xmmword ptr [r11+rdx-0x30], 0x01
        vshufps ymm12, ymm10, ymm11, 136
        vshufps ymm13, ymm10, ymm11, 221
        vmovups ymm10, ymmword ptr [r10+rdx-0x20]
        vinsertf128 ymm10, ymm10, xmmword ptr [r11+rdx-0x20], 0x01
        vmovups ymm11, ymmword ptr [r10+rdx-0x10]
        vinsertf128 ymm11, ymm11, xmmword ptr [r11+rdx-0x10], 0x01
        vshufps ymm14, ymm10, ymm11, 136
        vshufps ymm15, ymm10, ymm11, 221
        vpshufd ymm14, ymm14, 0x93
        vpshufd ymm15, ymm15, 0x93
        vpbroadcastd ymm2, dword ptr [rsp+0x200]
        vmovdqa ymm3, ymmword ptr [rsp]
        vmovdqa ymm11, ymmword ptr [rsp+0x20]
        vpblendd ymm3, ymm3, ymm2, 0x88
        vpblendd ymm11, ymm11, ymm2, 0x88
        vbroadcasti128 ymm2, xmmword ptr [BLAKE3_IV+rip]
        vmovdqa ymm10, ymm2
        mov     al, 7
9:
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm8, ymm8, ymm12
        vmovdqa ymmword ptr [rsp+0x40], ymm4
        nop
        vmovdqa ymmword ptr [rsp+0x60], ymm12
        nop
        vpaddd  ymm0, ymm0, ymm1
        vpaddd  ymm8, ymm8, ymm9
        vpxor   ymm3, ymm3, ymm0
        vpxor   ymm11, ymm11, ymm8
        vbroadcasti128 ymm4, xmmword ptr [ROT16+rip]
        vpshufb ymm3, ymm3, ymm4
        vpshufb ymm11, ymm11, ymm4
        vpaddd  ymm2, ymm2, ymm3
        vpaddd  ymm10, ymm10, ymm11
        vpxor   ymm1, ymm1, ymm2
        vpxor   ymm9, ymm9, ymm10
        vpsrld  ymm4, ymm1, 12
        vpslld  ymm1, ymm1, 20
        vpor    ymm1, ymm1, ymm4
        vpsrld  ymm4, ymm9, 12
        vpslld  ymm9, ymm9, 20
        vpor    ymm9, ymm9, ymm4
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm0, ymm0, ymm1
        vpaddd  ymm8, ymm8, ymm9
        vmovdqa ymmword ptr [rsp+0x80], ymm5
        vmovdqa ymmword ptr [rsp+0xA0], ymm13
        vpxor   ymm3, ymm3, ymm0
        vpxor   ymm11, ymm11, ymm8
        vbroadcasti128 ymm4, xmmword ptr [ROT8+rip]
        vpshufb ymm3, ymm3, ymm4
        vpshufb ymm11, ymm11, ymm4
        vpaddd  ymm2, ymm2, ymm3
        vpaddd  ymm10, ymm10, ymm11
        vpxor   ymm1, ymm1, ymm2
        vpxor   ymm9, ymm9, ymm10
        vpsrld  ymm4, ymm1, 7
        vpslld  ymm1, ymm1, 25
        vpor    ymm1, ymm1, ymm4
        vpsrld  ymm4, ymm9, 7
        vpslld  ymm9, ymm9, 25
        vpor    ymm9, ymm9, ymm4
        vpshufd ymm0, ymm0, 0x93
        vpshufd ymm8, ymm8, 0x93
        vpshufd ymm3, ymm3, 0x4E
        vpshufd ymm11, ymm11, 0x4E
        vpshufd ymm2, ymm2, 0x39
        vpshufd ymm10, ymm10, 0x39
        vpaddd  ymm0, ymm0, ymm6
        vpaddd  ymm8, ymm8, ymm14
        vpaddd  ymm0, ymm0, ymm1
        vpaddd  ymm8, ymm8, ymm9
        vpxor   ymm3, ymm3, ymm0
        vpxor   ymm11, ymm11, ymm8
        vbroadcasti128 ymm4, xmmword ptr [ROT16+rip]
        vpshufb ymm3, ymm3, ymm4
        vpshufb ymm11, ymm11, ymm4
        vpaddd  ymm2, ymm2, ymm3
        vpaddd  ymm10, ymm10, ymm11
        vpxor   ymm1, ymm1, ymm2
        vpxor   ymm9, ymm9, ymm10
        vpsrld  ymm4, ymm1, 12
        vpslld  ymm1, ymm1, 20
        vpor    ymm1, ymm1, ymm4
        vpsrld  ymm4, ymm9, 12
        vpslld  ymm9, ymm9, 20
        vpor    ymm9, ymm9, ymm4
        vpaddd  ymm0, ymm0, ymm7
        vpaddd  ymm8, ymm8, ymm15
        vpaddd  ymm0, ymm0, ymm1
        vpaddd  ymm8, ymm8, ymm9
        vpxor   ymm3, ymm3, ymm0
        vpxor   ymm11, ymm11, ymm8
        vbroadcasti128 ymm4, xmmword ptr [ROT8+rip]
        vpshufb ymm3, ymm3, ymm4
        vpshufb ymm11, ymm11, ymm4
        vpaddd  ymm2, ymm2, ymm3
        vpaddd  ymm10, ymm10, ymm11
        vpxor   ymm1, ymm1, ymm2
        vpxor   ymm9, ymm9, ymm10
        vpsrld  ymm4, ymm1, 7
        vpslld  ymm1, ymm1, 25
        vpor    ymm1, ymm1, ymm4
        vpsrld  ymm4, ymm9, 7
        vpslld  ymm9, ymm9, 25
        vpor    ymm9, ymm9, ymm4
        vpshufd ymm0, ymm0, 0x39
        vpshufd ymm8, ymm8, 0x39
        vpshufd ymm3, ymm3, 0x4E
        vpshufd ymm11, ymm11, 0x4E
        vpshufd ymm2, ymm2, 0x93
        vpshufd ymm10, ymm10, 0x93
        dec     al
        je      9f
        vmovdqa ymm4, ymmword ptr [rsp+0x40]
        vmovdqa ymm5, ymmword ptr [rsp+0x80]
        vshufps ymm12, ymm4, ymm5, 214
        vpshufd ymm13, ymm4, 0x0F
        vpshufd ymm4, ymm12, 0x39
        vshufps ymm12, ymm6, ymm7, 250
        vpblendd ymm13, ymm13, ymm12, 0xAA
        vpunpcklqdq ymm12, ymm7, ymm5
        vpblendd ymm12, ymm12, ymm6, 0x88
        vpshufd ymm12, ymm12, 0x78
        vpunpckhdq ymm5, ymm5, ymm7
        vpunpckldq ymm6, ymm6, ymm5
        vpshufd ymm7, ymm6, 0x1E
        vmovdqa ymmword ptr [rsp+0x40], ymm13
        vmovdqa ymmword ptr [rsp+0x80], ymm12
        vmovdqa ymm12, ymmword ptr [rsp+0x60]
        vmovdqa ymm13, ymmword ptr [rsp+0xA0]
        vshufps ymm5, ymm12, ymm13, 214
        vpshufd ymm6, ymm12, 0x0F
        vpshufd ymm12, ymm5, 0x39
        vshufps ymm5, ymm14, ymm15, 250
        vpblendd ymm6, ymm6, ymm5, 0xAA
        vpunpcklqdq ymm5, ymm15, ymm13
        vpblendd ymm5, ymm5, ymm14, 0x88
        vpshufd ymm5, ymm5, 0x78
        vpunpckhdq ymm13, ymm13, ymm15
        vpunpckldq ymm14, ymm14, ymm13
        vpshufd ymm15, ymm14, 0x1E
        vmovdqa ymm13, ymm6
        vmovdqa ymm14, ymm5
        vmovdqa ymm5, ymmword ptr [rsp+0x40]
        vmovdqa ymm6, ymmword ptr [rsp+0x80]
        jmp     9b
9:
        vpxor   ymm0, ymm0, ymm2
        vpxor   ymm1, ymm1, ymm3
        vpxor   ymm8, ymm8, ymm10
        vpxor   ymm9, ymm9, ymm11
        mov     eax, r13d
        cmp     rdx, r15
        jne     2b
        vmovdqu xmmword ptr [rbx], xmm0
        vmovdqu xmmword ptr [rbx+0x10], xmm1
        vextracti128 xmmword ptr [rbx+0x20], ymm0, 0x01
        vextracti128 xmmword ptr [rbx+0x30], ymm1, 0x01
        vmovdqu xmmword ptr [rbx+0x40], xmm8
        vmovdqu xmmword ptr [rbx+0x50], xmm9
        vextracti128 xmmword ptr [rbx+0x60], ymm8, 0x01
        vextracti128 xmmword ptr [rbx+0x70], ymm9, 0x01
        vmovaps xmm8, xmmword ptr [rsp+0x260]
        vmovaps xmm0, xmmword ptr [rsp+0x220]
        vmovaps xmm1, xmmword ptr [rsp+0x230]
        vmovaps xmm2, xmmword ptr [rsp+0x240]
        vmovaps xmm3, xmmword ptr [rsp+0x250]
        vblendvps xmm0, xmm0, xmm1, xmm8
        vblendvps xmm2, xmm2, xmm3, xmm8
        vmovaps xmmword ptr [rsp+0x220], xmm0
        vmovaps xmmword ptr [rsp+0x240], xmm2
        add     rbx, 128
        add     rdi, 32
        sub     rsi, 4
3:
        test    rsi, 0x2
        je      3f
        vbroadcasti128 ymm0, xmmword ptr [rcx]
        vbroadcasti128 ymm1, xmmword ptr [rcx+0x10]
        vmovd   xmm13, dword ptr [rsp+0x220]
        vpinsrd xmm13, xmm13, dword ptr [rsp+0x240], 1
        vpinsrd xmm13, xmm13, dword ptr [BLAKE3_BLOCK_LEN+rip], 2
        vmovd   xmm14, dword ptr [rsp+0x224]
        vpinsrd xmm14, xmm14, dword ptr [rsp+0x244], 1
        vpinsrd xmm14, xmm14, dword ptr [BLAKE3_BLOCK_LEN+rip], 2
        vinserti128 ymm13, ymm13, xmm14, 0x01
        vbroadcasti128 ymm14, xmmword ptr [ROT16+rip]
        vbroadcasti128 ymm15, xmmword ptr [ROT8+rip]
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
.p2align 5
2:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        mov     dword ptr [rsp+0x200], eax
        vbroadcasti128 ymm2, xmmword ptr [BLAKE3_IV+rip]
        vpbroadcastd ymm8, dword ptr [rsp+0x200]
        vpblendd ymm3, ymm13, ymm8, 0x88
        vmovups ymm8, ymmword ptr [r8+rdx-0x40]
        vinsertf128 ymm8, ymm8, xmmword ptr [r9+rdx-0x40], 0x01
        vmovups ymm9, ymmword ptr [r8+rdx-0x30]
        vinsertf128 ymm9, ymm9, xmmword ptr [r9+rdx-0x30], 0x01
        vshufps ymm4, ymm8, ymm9, 136
        vshufps ymm5, ymm8, ymm9, 221
        vmovups ymm8, ymmword ptr [r8+rdx-0x20]
        vinsertf128 ymm8, ymm8, xmmword ptr [r9+rdx-0x20], 0x01
        vmovups ymm9, ymmword ptr [r8+rdx-0x10]
        vinsertf128 ymm9, ymm9, xmmword ptr [r9+rdx-0x10], 0x01
        vshufps ymm6, ymm8, ymm9, 136
        vshufps ymm7, ymm8, ymm9, 221
        vpshufd ymm6, ymm6, 0x93
        vpshufd ymm7, ymm7, 0x93
        mov     al, 7
9:
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm0, ymm0, ymm1
        vpxor   ymm3, ymm3, ymm0
        vpshufb ymm3, ymm3, ymm14
        vpaddd  ymm2, ymm2, ymm3
        vpxor   ymm1, ymm1, ymm2
        vpsrld  ymm8, ymm1, 12
        vpslld  ymm1, ymm1, 20
        vpor    ymm1, ymm1, ymm8
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm0, ymm0, ymm1
        vpxor   ymm3, ymm3, ymm0
        vpshufb ymm3, ymm3, ymm15
        vpaddd  ymm2, ymm2, ymm3
        vpxor   ymm1, ymm1, ymm2
        vpsrld  ymm8, ymm1, 7
        vpslld  ymm1, ymm1, 25
        vpor    ymm1, ymm1, ymm8
        vpshufd ymm0, ymm0, 0x93
        vpshufd ymm3, ymm3, 0x4E
        vpshufd ymm2, ymm2, 0x39
        vpaddd  ymm0, ymm0, ymm6
        vpaddd  ymm0, ymm0, ymm1
        vpxor   ymm3, ymm3, ymm0
        vpshufb ymm3, ymm3, ymm14
        vpaddd  ymm2, ymm2, ymm3
        vpxor   ymm1, ymm1, ymm2
        vpsrld  ymm8, ymm1, 12
        vpslld  ymm1, ymm1, 20
        vpor    ymm1, ymm1, ymm8
        vpaddd  ymm0, ymm0, ymm7
        vpaddd  ymm0, ymm0, ymm1
        vpxor   ymm3, ymm3, ymm0
        vpshufb ymm3, ymm3, ymm15
        vpaddd  ymm2, ymm2, ymm3
        vpxor   ymm1, ymm1, ymm2
        vpsrld  ymm8, ymm1, 7
        vpslld  ymm1, ymm1, 25
        vpor    ymm1, ymm1, ymm8
        vpshufd ymm0, ymm0, 0x39
        vpshufd ymm3, ymm3, 0x4E
        vpshufd ymm2, ymm2, 0x93
        dec     al
        jz      9f
        vshufps ymm8, ymm4, ymm5, 214
        vpshufd ymm9, ymm4, 0x0F
        vpshufd ymm4, ymm8, 0x39
        vshufps ymm8, ymm6, ymm7, 250
        vpblendd ymm9, ymm9, ymm8, 0xAA
        vpunpcklqdq ymm8, ymm7, ymm5
        vpblendd ymm8, ymm8, ymm6, 0x88
        vpshufd ymm8, ymm8, 0x78
        vpunpckhdq ymm5, ymm5, ymm7
        vpunpckldq ymm6, ymm6, ymm5
        vpshufd ymm7, ymm6, 0x1E
        vmovdqa ymm5, ymm9
        vmovdqa ymm6, ymm8
        jmp     9b
9:
        vpxor   ymm0, ymm0, ymm2
        vpxor   ymm1, ymm1, ymm3
        mov     eax, r13d
        cmp     rdx, r15
        jne     2b
        vmovdqu xmmword ptr [rbx], xmm0
        vmovdqu xmmword ptr [rbx+0x10], xmm1
        vextracti128 xmmword ptr [rbx+0x20], ymm0, 0x01
        vextracti128 xmmword ptr [rbx+0x30], ymm1, 0x01
        vmovaps ymm8, ymmword ptr [rsp+0x260]
        vmovaps ymm0, ymmword ptr [rsp+0x220]
        vmovups ymm1, ymmword ptr [rsp+0x228]
        vmovaps ymm2, ymmword ptr [rsp+0x240]
        vmovups ymm3, ymmword ptr [rsp+0x248]
        vblendvps ymm0, ymm0, ymm1, ymm8
        vblendvps ymm2, ymm2, ymm3, ymm8
        vmovaps ymmword ptr [rsp+0x220], ymm0
        vmovaps ymmword ptr [rsp+0x240], ymm2
        add     rbx, 64
        add     rdi, 16
        sub     rsi, 2
3:
        test    rsi, 0x1
        je      4b
        vmovdqu xmm0, xmmword ptr [rcx]
        vmovdqu xmm1, xmmword ptr [rcx+0x10]
        vmovd   xmm3, dword ptr [rsp+0x220]
        vpinsrd xmm3, xmm3, dword ptr [rsp+0x240], 1
        vpinsrd xmm13, xmm3, dword ptr [BLAKE3_BLOCK_LEN+rip], 2
        vmovdqa xmm14, xmmword ptr [ROT16+rip]
        vmovdqa xmm15, xmmword ptr [ROT8+rip]
        mov     r8, qword ptr [rdi]
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
.p2align 5
2:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        vmovdqa xmm2, xmmword ptr [BLAKE3_IV+rip]
        vmovdqa xmm3, xmm13
        vpinsrd xmm3, xmm3, eax, 3
        vmovups xmm8, xmmword ptr [r8+rdx-0x40]
        vmovups xmm9, xmmword ptr [r8+rdx-0x30]
        vshufps xmm4, xmm8, xmm9, 136
        vshufps xmm5, xmm8, xmm9, 221
        vmovups xmm8, xmmword ptr [r8+rdx-0x20]
        vmovups xmm9, xmmword ptr [r8+rdx-0x10]
        vshufps xmm6, xmm8, xmm9, 136
        vshufps xmm7, xmm8, xmm9, 221
        vpshufd xmm6, xmm6, 0x93
        vpshufd xmm7, xmm7, 0x93
        mov     al, 7
9:
        vpaddd  xmm0, xmm0, xmm4
        vpaddd  xmm0, xmm0, xmm1
        vpxor   xmm3, xmm3, xmm0
        vpshufb xmm3, xmm3, xmm14
        vpaddd  xmm2, xmm2, xmm3
        vpxor   xmm1, xmm1, xmm2
        vpsrld  xmm8, xmm1, 12
        vpslld  xmm1, xmm1, 20
        vpor    xmm1, xmm1, xmm8
        vpaddd  xmm0, xmm0, xmm5
        vpaddd  xmm0, xmm0, xmm1
        vpxor   xmm3, xmm3, xmm0
        vpshufb xmm3, xmm3, xmm15
        vpaddd  xmm2, xmm2, xmm3
        vpxor   xmm1, xmm1, xmm2
        vpsrld  xmm8, xmm1, 7
        vpslld  xmm1, xmm1, 25
        vpor    xmm1, xmm1, xmm8
        vpshufd xmm0, xmm0, 0x93
        vpshufd xmm3, xmm3, 0x4E
        vpshufd xmm2, xmm2, 0x39
        vpaddd  xmm0, xmm0, xmm6
        vpaddd  xmm0, xmm0, xmm1
        vpxor   xmm3, xmm3, xmm0
        vpshufb xmm3, xmm3, xmm14
        vpaddd  xmm2, xmm2, xmm3
        vpxor   xmm1, xmm1, xmm2
        vpsrld  xmm8, xmm1, 12
        vpslld  xmm1, xmm1, 20
        vpor    xmm1, xmm1, xmm8
        vpaddd  xmm0, xmm0, xmm7
        vpaddd  xmm0, xmm0, xmm1
        vpxor   xmm3, xmm3, xmm0
        vpshufb xmm3, xmm3, xmm15
        vpaddd  xmm2, xmm2, xmm3
        vpxor   xmm1, xmm1, xmm2
        vpsrld  xmm8, xmm1, 7
        vpslld  xmm1, xmm1, 25
        vpor    xmm1, xmm1, xmm8
        vpshufd xmm0, xmm0, 0x39
        vpshufd xmm3, xmm3, 0x4E
        vpshufd xmm2, xmm2, 0x93
        dec     al
        jz      9f
        vshufps xmm8, xmm4, xmm5, 214
        vpshufd xmm9, xmm4, 0x0F
        vpshufd xmm4, xmm8, 0x39
        vshufps xmm8, xmm6, xmm7, 250
        vpblendd xmm9, xmm9, xmm8, 0xAA
        vpunpcklqdq xmm8, xmm7, xmm5
        vpblendd xmm8, xmm8, xmm6, 0x88
        vpshufd xmm8, xmm8, 0x78
        vpunpckhdq xmm5, xmm5, xmm7
        vpunpckldq xmm6, xmm6, xmm5
        vpshufd xmm7, xmm6, 0x1E
        vmovdqa xmm5, xmm9
        vmovdqa xmm6, xmm8
        jmp     9b
9:
        vpxor   xmm0, xmm0, xmm2
        vpxor   xmm1, xmm1, xmm3
        mov     eax, r13d
        cmp     rdx, r15
        jne     2b
        vmovdqu xmmword ptr [rbx], xmm0
        vmovdqu xmmword ptr [rbx+0x10], xmm1
        jmp     4b

.section .rodata
.p2align  6
ADD0:
        .long  0, 1, 2, 3, 4, 5, 6, 7
ADD1:
        .long  8, 8, 8, 8, 8, 8, 8, 8
BLAKE3_IV_0:
        .long  0x6A09E667, 0x6A09E667, 0x6A09E667, 0x6A09E667
        .long  0x6A09E667, 0x6A09E667, 0x6A09E667, 0x6A09E667
BLAKE3_IV_1:
        .long  0xBB67AE85, 0xBB67AE85, 0xBB67AE85, 0xBB67AE85
        .long  0xBB67AE85, 0xBB67AE85, 0xBB67AE85, 0xBB67AE85
BLAKE3_IV_2:
        .long  0x3C6EF372, 0x3C6EF372, 0x3C6EF372, 0x3C6EF372
        .long  0x3C6EF372, 0x3C6EF372, 0x3C6EF372, 0x3C6EF372
BLAKE3_IV_3:
        .long  0xA54FF53A, 0xA54FF53A, 0xA54FF53A, 0xA54FF53A
        .long  0xA54FF53A, 0xA54FF53A, 0xA54FF53A, 0xA54FF53A
BLAKE3_BLOCK_LEN:
        .long  0x00000040, 0x00000040, 0x00000040, 0x00000040
        .long  0x00000040, 0x00000040, 0x00000040, 0x00000040
ROT16:
        .byte  2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13
ROT8:
        .byte  1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 8, 13, 14, 15, 12
CMP_MSB_MASK:
        .long  0x80000000, 0x80000000, 0x80000000, 0x80000000
        .long  0x80000000, 0x80000000, 0x80000000, 0x80000000
BLAKE3_IV:
        .long  0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A


















.intel_syntax noprefix

.global _blake3_hash_many_avx512
.global blake3_hash_many_avx512
.global blake3_compress_in_place_avx512
.global _blake3_compress_in_place_avx512
.global blake3_compress_xof_avx512
.global _blake3_compress_xof_avx512

.section .text
.p2align  6
_blake3_hash_many_avx512:
blake3_hash_many_avx512:
        push    r15
        push    r14
        push    r13
        push    r12
        push    rdi
        push    rsi
        push    rbx
        push    rbp
        mov     rbp, rsp
        sub     rsp, 304
        and     rsp, 0xFFFFFFFFFFFFFFC0
        vmovdqa xmmword ptr [rsp+0x90], xmm6
        vmovdqa xmmword ptr [rsp+0xA0], xmm7
        vmovdqa xmmword ptr [rsp+0xB0], xmm8
        vmovdqa xmmword ptr [rsp+0xC0], xmm9
        vmovdqa xmmword ptr [rsp+0xD0], xmm10
        vmovdqa xmmword ptr [rsp+0xE0], xmm11
        vmovdqa xmmword ptr [rsp+0xF0], xmm12
        vmovdqa xmmword ptr [rsp+0x100], xmm13
        vmovdqa xmmword ptr [rsp+0x110], xmm14
        vmovdqa xmmword ptr [rsp+0x120], xmm15
        mov     rdi, rcx
        mov     rsi, rdx
        mov     rdx, r8
        mov     rcx, r9
        mov     r8, qword ptr [rbp+0x68]
        movzx   r9, byte ptr [rbp+0x70]
        neg     r9
        kmovw   k1, r9d
        vmovd   xmm0, r8d
        vpbroadcastd ymm0, xmm0
        shr     r8, 32
        vmovd   xmm1, r8d
        vpbroadcastd ymm1, xmm1
        vmovdqa ymm4, ymm1
        vmovdqa ymm5, ymm1
        vpaddd  ymm2, ymm0, ymmword ptr [ADD0_avx512+rip]
        vpaddd  ymm3, ymm0, ymmword ptr [ADD0_avx512+32+rip]
        vpcmpltud k2, ymm2, ymm0
        vpcmpltud k3, ymm3, ymm0
        vpaddd  ymm4 {k2}, ymm4, dword ptr [ADD1_avx512+rip] {1to8}
        vpaddd  ymm5 {k3}, ymm5, dword ptr [ADD1_avx512+rip] {1to8}
        knotw   k2, k1
        vmovdqa32 ymm2 {k2}, ymm0
        vmovdqa32 ymm3 {k2}, ymm0
        vmovdqa32 ymm4 {k2}, ymm1
        vmovdqa32 ymm5 {k2}, ymm1
        vmovdqa ymmword ptr [rsp], ymm2
        vmovdqa ymmword ptr [rsp+0x20], ymm3
        vmovdqa ymmword ptr [rsp+0x40], ymm4
        vmovdqa ymmword ptr [rsp+0x60], ymm5
        shl     rdx, 6
        mov     qword ptr [rsp+0x80], rdx
        cmp     rsi, 16
        jc      3f
2:
        vpbroadcastd zmm0, dword ptr [rcx]
        vpbroadcastd zmm1, dword ptr [rcx+0x1*0x4]
        vpbroadcastd zmm2, dword ptr [rcx+0x2*0x4]
        vpbroadcastd zmm3, dword ptr [rcx+0x3*0x4]
        vpbroadcastd zmm4, dword ptr [rcx+0x4*0x4]
        vpbroadcastd zmm5, dword ptr [rcx+0x5*0x4]
        vpbroadcastd zmm6, dword ptr [rcx+0x6*0x4]
        vpbroadcastd zmm7, dword ptr [rcx+0x7*0x4]
        movzx   eax, byte ptr [rbp+0x78]
        movzx   ebx, byte ptr [rbp+0x80]
        or      eax, ebx
        xor     edx, edx
.p2align 5
9:
        movzx   ebx, byte ptr [rbp+0x88]
        or      ebx, eax
        add     rdx, 64
        cmp     rdx, qword ptr [rsp+0x80]
        cmove   eax, ebx
        mov     dword ptr [rsp+0x88], eax
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        mov     r10, qword ptr [rdi+0x10]
        mov     r11, qword ptr [rdi+0x18]
        mov     r12, qword ptr [rdi+0x40]
        mov     r13, qword ptr [rdi+0x48]
        mov     r14, qword ptr [rdi+0x50]
        mov     r15, qword ptr [rdi+0x58]
        vmovdqu32 ymm16, ymmword ptr [rdx+r8-0x2*0x20]
        vinserti64x4 zmm16, zmm16, ymmword ptr [rdx+r12-0x2*0x20], 0x01
        vmovdqu32 ymm17, ymmword ptr [rdx+r9-0x2*0x20]
        vinserti64x4 zmm17, zmm17, ymmword ptr [rdx+r13-0x2*0x20], 0x01
        vpunpcklqdq zmm8, zmm16, zmm17
        vpunpckhqdq zmm9, zmm16, zmm17
        vmovdqu32 ymm18, ymmword ptr [rdx+r10-0x2*0x20]
        vinserti64x4 zmm18, zmm18, ymmword ptr [rdx+r14-0x2*0x20], 0x01
        vmovdqu32 ymm19, ymmword ptr [rdx+r11-0x2*0x20]
        vinserti64x4 zmm19, zmm19, ymmword ptr [rdx+r15-0x2*0x20], 0x01
        vpunpcklqdq zmm10, zmm18, zmm19
        vpunpckhqdq zmm11, zmm18, zmm19
        mov     r8, qword ptr [rdi+0x20]
        mov     r9, qword ptr [rdi+0x28]
        mov     r10, qword ptr [rdi+0x30]
        mov     r11, qword ptr [rdi+0x38]
        mov     r12, qword ptr [rdi+0x60]
        mov     r13, qword ptr [rdi+0x68]
        mov     r14, qword ptr [rdi+0x70]
        mov     r15, qword ptr [rdi+0x78]
        vmovdqu32 ymm16, ymmword ptr [rdx+r8-0x2*0x20]
        vinserti64x4 zmm16, zmm16, ymmword ptr [rdx+r12-0x2*0x20], 0x01
        vmovdqu32 ymm17, ymmword ptr [rdx+r9-0x2*0x20]
        vinserti64x4 zmm17, zmm17, ymmword ptr [rdx+r13-0x2*0x20], 0x01
        vpunpcklqdq zmm12, zmm16, zmm17
        vpunpckhqdq zmm13, zmm16, zmm17
        vmovdqu32 ymm18, ymmword ptr [rdx+r10-0x2*0x20]
        vinserti64x4 zmm18, zmm18, ymmword ptr [rdx+r14-0x2*0x20], 0x01
        vmovdqu32 ymm19, ymmword ptr [rdx+r11-0x2*0x20]
        vinserti64x4 zmm19, zmm19, ymmword ptr [rdx+r15-0x2*0x20], 0x01
        vpunpcklqdq zmm14, zmm18, zmm19
        vpunpckhqdq zmm15, zmm18, zmm19
        vmovdqa32 zmm27, zmmword ptr [INDEX0+rip]
        vmovdqa32 zmm31, zmmword ptr [INDEX1+rip]
        vshufps zmm16, zmm8, zmm10, 136
        vshufps zmm17, zmm12, zmm14, 136
        vmovdqa32 zmm20, zmm16
        vpermt2d zmm16, zmm27, zmm17
        vpermt2d zmm20, zmm31, zmm17
        vshufps zmm17, zmm8, zmm10, 221
        vshufps zmm30, zmm12, zmm14, 221
        vmovdqa32 zmm21, zmm17
        vpermt2d zmm17, zmm27, zmm30
        vpermt2d zmm21, zmm31, zmm30
        vshufps zmm18, zmm9, zmm11, 136
        vshufps zmm8, zmm13, zmm15, 136
        vmovdqa32 zmm22, zmm18
        vpermt2d zmm18, zmm27, zmm8
        vpermt2d zmm22, zmm31, zmm8
        vshufps zmm19, zmm9, zmm11, 221
        vshufps zmm8, zmm13, zmm15, 221
        vmovdqa32 zmm23, zmm19
        vpermt2d zmm19, zmm27, zmm8
        vpermt2d zmm23, zmm31, zmm8
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        mov     r10, qword ptr [rdi+0x10]
        mov     r11, qword ptr [rdi+0x18]
        mov     r12, qword ptr [rdi+0x40]
        mov     r13, qword ptr [rdi+0x48]
        mov     r14, qword ptr [rdi+0x50]
        mov     r15, qword ptr [rdi+0x58]
        vmovdqu32 ymm24, ymmword ptr [r8+rdx-0x1*0x20]
        vinserti64x4 zmm24, zmm24, ymmword ptr [r12+rdx-0x1*0x20], 0x01
        vmovdqu32 ymm25, ymmword ptr [r9+rdx-0x1*0x20]
        vinserti64x4 zmm25, zmm25, ymmword ptr [r13+rdx-0x1*0x20], 0x01
        vpunpcklqdq zmm8, zmm24, zmm25
        vpunpckhqdq zmm9, zmm24, zmm25
        vmovdqu32 ymm24, ymmword ptr [r10+rdx-0x1*0x20]
        vinserti64x4 zmm24, zmm24, ymmword ptr [r14+rdx-0x1*0x20], 0x01
        vmovdqu32 ymm25, ymmword ptr [r11+rdx-0x1*0x20]
        vinserti64x4 zmm25, zmm25, ymmword ptr [r15+rdx-0x1*0x20], 0x01
        vpunpcklqdq zmm10, zmm24, zmm25
        vpunpckhqdq zmm11, zmm24, zmm25
        prefetcht0 [r8+rdx+0x80]
        prefetcht0 [r12+rdx+0x80]
        prefetcht0 [r9+rdx+0x80]
        prefetcht0 [r13+rdx+0x80]
        prefetcht0 [r10+rdx+0x80]
        prefetcht0 [r14+rdx+0x80]
        prefetcht0 [r11+rdx+0x80]
        prefetcht0 [r15+rdx+0x80]
        mov     r8, qword ptr [rdi+0x20]
        mov     r9, qword ptr [rdi+0x28]
        mov     r10, qword ptr [rdi+0x30]
        mov     r11, qword ptr [rdi+0x38]
        mov     r12, qword ptr [rdi+0x60]
        mov     r13, qword ptr [rdi+0x68]
        mov     r14, qword ptr [rdi+0x70]
        mov     r15, qword ptr [rdi+0x78]
        vmovdqu32 ymm24, ymmword ptr [r8+rdx-0x1*0x20]
        vinserti64x4 zmm24, zmm24, ymmword ptr [r12+rdx-0x1*0x20], 0x01
        vmovdqu32 ymm25, ymmword ptr [r9+rdx-0x1*0x20]
        vinserti64x4 zmm25, zmm25, ymmword ptr [r13+rdx-0x1*0x20], 0x01
        vpunpcklqdq zmm12, zmm24, zmm25
        vpunpckhqdq zmm13, zmm24, zmm25
        vmovdqu32 ymm24, ymmword ptr [r10+rdx-0x1*0x20]
        vinserti64x4 zmm24, zmm24, ymmword ptr [r14+rdx-0x1*0x20], 0x01
        vmovdqu32 ymm25, ymmword ptr [r11+rdx-0x1*0x20]
        vinserti64x4 zmm25, zmm25, ymmword ptr [r15+rdx-0x1*0x20], 0x01
        vpunpcklqdq zmm14, zmm24, zmm25
        vpunpckhqdq zmm15, zmm24, zmm25
        prefetcht0 [r8+rdx+0x80]
        prefetcht0 [r12+rdx+0x80]
        prefetcht0 [r9+rdx+0x80]
        prefetcht0 [r13+rdx+0x80]
        prefetcht0 [r10+rdx+0x80]
        prefetcht0 [r14+rdx+0x80]
        prefetcht0 [r11+rdx+0x80]
        prefetcht0 [r15+rdx+0x80]
        vshufps zmm24, zmm8, zmm10, 136
        vshufps zmm30, zmm12, zmm14, 136
        vmovdqa32 zmm28, zmm24
        vpermt2d zmm24, zmm27, zmm30
        vpermt2d zmm28, zmm31, zmm30
        vshufps zmm25, zmm8, zmm10, 221
        vshufps zmm30, zmm12, zmm14, 221
        vmovdqa32 zmm29, zmm25
        vpermt2d zmm25, zmm27, zmm30
        vpermt2d zmm29, zmm31, zmm30
        vshufps zmm26, zmm9, zmm11, 136
        vshufps zmm8, zmm13, zmm15, 136
        vmovdqa32 zmm30, zmm26
        vpermt2d zmm26, zmm27, zmm8
        vpermt2d zmm30, zmm31, zmm8
        vshufps zmm8, zmm9, zmm11, 221
        vshufps zmm10, zmm13, zmm15, 221
        vpermi2d zmm27, zmm8, zmm10
        vpermi2d zmm31, zmm8, zmm10
        vpbroadcastd zmm8, dword ptr [BLAKE3_IV_avx512_0+rip]
        vpbroadcastd zmm9, dword ptr [BLAKE3_IV_avx512_1+rip]
        vpbroadcastd zmm10, dword ptr [BLAKE3_IV_avx512_2+rip]
        vpbroadcastd zmm11, dword ptr [BLAKE3_IV_avx512_3+rip]
        vmovdqa32 zmm12, zmmword ptr [rsp]
        vmovdqa32 zmm13, zmmword ptr [rsp+0x1*0x40]
        vpbroadcastd zmm14, dword ptr [BLAKE3_BLOCK_LEN_avx512+rip]
        vpbroadcastd zmm15, dword ptr [rsp+0x22*0x4]
        vpaddd  zmm0, zmm0, zmm16
        vpaddd  zmm1, zmm1, zmm18
        vpaddd  zmm2, zmm2, zmm20
        vpaddd  zmm3, zmm3, zmm22
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vprord  zmm15, zmm15, 16
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 12
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vpaddd  zmm0, zmm0, zmm17
        vpaddd  zmm1, zmm1, zmm19
        vpaddd  zmm2, zmm2, zmm21
        vpaddd  zmm3, zmm3, zmm23
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vprord  zmm15, zmm15, 8
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 7
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vpaddd  zmm0, zmm0, zmm24
        vpaddd  zmm1, zmm1, zmm26
        vpaddd  zmm2, zmm2, zmm28
        vpaddd  zmm3, zmm3, zmm30
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 16
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vprord  zmm4, zmm4, 12
        vpaddd  zmm0, zmm0, zmm25
        vpaddd  zmm1, zmm1, zmm27
        vpaddd  zmm2, zmm2, zmm29
        vpaddd  zmm3, zmm3, zmm31
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 8
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vprord  zmm4, zmm4, 7
        vpaddd  zmm0, zmm0, zmm18
        vpaddd  zmm1, zmm1, zmm19
        vpaddd  zmm2, zmm2, zmm23
        vpaddd  zmm3, zmm3, zmm20
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vprord  zmm15, zmm15, 16
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 12
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vpaddd  zmm0, zmm0, zmm22
        vpaddd  zmm1, zmm1, zmm26
        vpaddd  zmm2, zmm2, zmm16
        vpaddd  zmm3, zmm3, zmm29
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vprord  zmm15, zmm15, 8
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 7
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vpaddd  zmm0, zmm0, zmm17
        vpaddd  zmm1, zmm1, zmm28
        vpaddd  zmm2, zmm2, zmm25
        vpaddd  zmm3, zmm3, zmm31
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 16
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vprord  zmm4, zmm4, 12
        vpaddd  zmm0, zmm0, zmm27
        vpaddd  zmm1, zmm1, zmm21
        vpaddd  zmm2, zmm2, zmm30
        vpaddd  zmm3, zmm3, zmm24
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 8
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vprord  zmm4, zmm4, 7
        vpaddd  zmm0, zmm0, zmm19
        vpaddd  zmm1, zmm1, zmm26
        vpaddd  zmm2, zmm2, zmm29
        vpaddd  zmm3, zmm3, zmm23
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vprord  zmm15, zmm15, 16
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 12
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vpaddd  zmm0, zmm0, zmm20
        vpaddd  zmm1, zmm1, zmm28
        vpaddd  zmm2, zmm2, zmm18
        vpaddd  zmm3, zmm3, zmm30
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vprord  zmm15, zmm15, 8
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 7
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vpaddd  zmm0, zmm0, zmm22
        vpaddd  zmm1, zmm1, zmm25
        vpaddd  zmm2, zmm2, zmm27
        vpaddd  zmm3, zmm3, zmm24
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 16
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vprord  zmm4, zmm4, 12
        vpaddd  zmm0, zmm0, zmm21
        vpaddd  zmm1, zmm1, zmm16
        vpaddd  zmm2, zmm2, zmm31
        vpaddd  zmm3, zmm3, zmm17
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 8
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vprord  zmm4, zmm4, 7
        vpaddd  zmm0, zmm0, zmm26
        vpaddd  zmm1, zmm1, zmm28
        vpaddd  zmm2, zmm2, zmm30
        vpaddd  zmm3, zmm3, zmm29
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vprord  zmm15, zmm15, 16
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 12
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vpaddd  zmm0, zmm0, zmm23
        vpaddd  zmm1, zmm1, zmm25
        vpaddd  zmm2, zmm2, zmm19
        vpaddd  zmm3, zmm3, zmm31
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vprord  zmm15, zmm15, 8
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 7
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vpaddd  zmm0, zmm0, zmm20
        vpaddd  zmm1, zmm1, zmm27
        vpaddd  zmm2, zmm2, zmm21
        vpaddd  zmm3, zmm3, zmm17
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 16
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vprord  zmm4, zmm4, 12
        vpaddd  zmm0, zmm0, zmm16
        vpaddd  zmm1, zmm1, zmm18
        vpaddd  zmm2, zmm2, zmm24
        vpaddd  zmm3, zmm3, zmm22
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 8
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vprord  zmm4, zmm4, 7
        vpaddd  zmm0, zmm0, zmm28
        vpaddd  zmm1, zmm1, zmm25
        vpaddd  zmm2, zmm2, zmm31
        vpaddd  zmm3, zmm3, zmm30
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vprord  zmm15, zmm15, 16
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 12
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vpaddd  zmm0, zmm0, zmm29
        vpaddd  zmm1, zmm1, zmm27
        vpaddd  zmm2, zmm2, zmm26
        vpaddd  zmm3, zmm3, zmm24
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vprord  zmm15, zmm15, 8
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 7
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vpaddd  zmm0, zmm0, zmm23
        vpaddd  zmm1, zmm1, zmm21
        vpaddd  zmm2, zmm2, zmm16
        vpaddd  zmm3, zmm3, zmm22
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 16
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vprord  zmm4, zmm4, 12
        vpaddd  zmm0, zmm0, zmm18
        vpaddd  zmm1, zmm1, zmm19
        vpaddd  zmm2, zmm2, zmm17
        vpaddd  zmm3, zmm3, zmm20
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 8
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vprord  zmm4, zmm4, 7
        vpaddd  zmm0, zmm0, zmm25
        vpaddd  zmm1, zmm1, zmm27
        vpaddd  zmm2, zmm2, zmm24
        vpaddd  zmm3, zmm3, zmm31
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vprord  zmm15, zmm15, 16
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 12
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vpaddd  zmm0, zmm0, zmm30
        vpaddd  zmm1, zmm1, zmm21
        vpaddd  zmm2, zmm2, zmm28
        vpaddd  zmm3, zmm3, zmm17
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vprord  zmm15, zmm15, 8
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 7
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vpaddd  zmm0, zmm0, zmm29
        vpaddd  zmm1, zmm1, zmm16
        vpaddd  zmm2, zmm2, zmm18
        vpaddd  zmm3, zmm3, zmm20
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 16
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vprord  zmm4, zmm4, 12
        vpaddd  zmm0, zmm0, zmm19
        vpaddd  zmm1, zmm1, zmm26
        vpaddd  zmm2, zmm2, zmm22
        vpaddd  zmm3, zmm3, zmm23
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 8
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vprord  zmm4, zmm4, 7
        vpaddd  zmm0, zmm0, zmm27
        vpaddd  zmm1, zmm1, zmm21
        vpaddd  zmm2, zmm2, zmm17
        vpaddd  zmm3, zmm3, zmm24
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vprord  zmm15, zmm15, 16
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 12
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vpaddd  zmm0, zmm0, zmm31
        vpaddd  zmm1, zmm1, zmm16
        vpaddd  zmm2, zmm2, zmm25
        vpaddd  zmm3, zmm3, zmm22
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm1, zmm1, zmm5
        vpaddd  zmm2, zmm2, zmm6
        vpaddd  zmm3, zmm3, zmm7
        vpxord  zmm12, zmm12, zmm0
        vpxord  zmm13, zmm13, zmm1
        vpxord  zmm14, zmm14, zmm2
        vpxord  zmm15, zmm15, zmm3
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vprord  zmm15, zmm15, 8
        vpaddd  zmm8, zmm8, zmm12
        vpaddd  zmm9, zmm9, zmm13
        vpaddd  zmm10, zmm10, zmm14
        vpaddd  zmm11, zmm11, zmm15
        vpxord  zmm4, zmm4, zmm8
        vpxord  zmm5, zmm5, zmm9
        vpxord  zmm6, zmm6, zmm10
        vpxord  zmm7, zmm7, zmm11
        vprord  zmm4, zmm4, 7
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vpaddd  zmm0, zmm0, zmm30
        vpaddd  zmm1, zmm1, zmm18
        vpaddd  zmm2, zmm2, zmm19
        vpaddd  zmm3, zmm3, zmm23
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 16
        vprord  zmm12, zmm12, 16
        vprord  zmm13, zmm13, 16
        vprord  zmm14, zmm14, 16
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 12
        vprord  zmm6, zmm6, 12
        vprord  zmm7, zmm7, 12
        vprord  zmm4, zmm4, 12
        vpaddd  zmm0, zmm0, zmm26
        vpaddd  zmm1, zmm1, zmm28
        vpaddd  zmm2, zmm2, zmm20
        vpaddd  zmm3, zmm3, zmm29
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm1, zmm1, zmm6
        vpaddd  zmm2, zmm2, zmm7
        vpaddd  zmm3, zmm3, zmm4
        vpxord  zmm15, zmm15, zmm0
        vpxord  zmm12, zmm12, zmm1
        vpxord  zmm13, zmm13, zmm2
        vpxord  zmm14, zmm14, zmm3
        vprord  zmm15, zmm15, 8
        vprord  zmm12, zmm12, 8
        vprord  zmm13, zmm13, 8
        vprord  zmm14, zmm14, 8
        vpaddd  zmm10, zmm10, zmm15
        vpaddd  zmm11, zmm11, zmm12
        vpaddd  zmm8, zmm8, zmm13
        vpaddd  zmm9, zmm9, zmm14
        vpxord  zmm5, zmm5, zmm10
        vpxord  zmm6, zmm6, zmm11
        vpxord  zmm7, zmm7, zmm8
        vpxord  zmm4, zmm4, zmm9
        vprord  zmm5, zmm5, 7
        vprord  zmm6, zmm6, 7
        vprord  zmm7, zmm7, 7
        vprord  zmm4, zmm4, 7
        vpxord  zmm0, zmm0, zmm8
        vpxord  zmm1, zmm1, zmm9
        vpxord  zmm2, zmm2, zmm10
        vpxord  zmm3, zmm3, zmm11
        vpxord  zmm4, zmm4, zmm12
        vpxord  zmm5, zmm5, zmm13
        vpxord  zmm6, zmm6, zmm14
        vpxord  zmm7, zmm7, zmm15
        movzx   eax, byte ptr [rbp+0x78]
        jne     9b
        mov     rbx, qword ptr [rbp+0x90]
        vpunpckldq zmm16, zmm0, zmm1
        vpunpckhdq zmm17, zmm0, zmm1
        vpunpckldq zmm18, zmm2, zmm3
        vpunpckhdq zmm19, zmm2, zmm3
        vpunpckldq zmm20, zmm4, zmm5
        vpunpckhdq zmm21, zmm4, zmm5
        vpunpckldq zmm22, zmm6, zmm7
        vpunpckhdq zmm23, zmm6, zmm7
        vpunpcklqdq zmm0, zmm16, zmm18
        vpunpckhqdq zmm1, zmm16, zmm18
        vpunpcklqdq zmm2, zmm17, zmm19
        vpunpckhqdq zmm3, zmm17, zmm19
        vpunpcklqdq zmm4, zmm20, zmm22
        vpunpckhqdq zmm5, zmm20, zmm22
        vpunpcklqdq zmm6, zmm21, zmm23
        vpunpckhqdq zmm7, zmm21, zmm23
        vshufi32x4 zmm16, zmm0, zmm4, 0x88
        vshufi32x4 zmm17, zmm1, zmm5, 0x88
        vshufi32x4 zmm18, zmm2, zmm6, 0x88
        vshufi32x4 zmm19, zmm3, zmm7, 0x88
        vshufi32x4 zmm20, zmm0, zmm4, 0xDD
        vshufi32x4 zmm21, zmm1, zmm5, 0xDD
        vshufi32x4 zmm22, zmm2, zmm6, 0xDD
        vshufi32x4 zmm23, zmm3, zmm7, 0xDD
        vshufi32x4 zmm0, zmm16, zmm17, 0x88
        vshufi32x4 zmm1, zmm18, zmm19, 0x88
        vshufi32x4 zmm2, zmm20, zmm21, 0x88
        vshufi32x4 zmm3, zmm22, zmm23, 0x88
        vshufi32x4 zmm4, zmm16, zmm17, 0xDD
        vshufi32x4 zmm5, zmm18, zmm19, 0xDD
        vshufi32x4 zmm6, zmm20, zmm21, 0xDD
        vshufi32x4 zmm7, zmm22, zmm23, 0xDD
        vmovdqu32 zmmword ptr [rbx], zmm0
        vmovdqu32 zmmword ptr [rbx+0x1*0x40], zmm1
        vmovdqu32 zmmword ptr [rbx+0x2*0x40], zmm2
        vmovdqu32 zmmword ptr [rbx+0x3*0x40], zmm3
        vmovdqu32 zmmword ptr [rbx+0x4*0x40], zmm4
        vmovdqu32 zmmword ptr [rbx+0x5*0x40], zmm5
        vmovdqu32 zmmword ptr [rbx+0x6*0x40], zmm6
        vmovdqu32 zmmword ptr [rbx+0x7*0x40], zmm7
        vmovdqa32 zmm0, zmmword ptr [rsp]
        vmovdqa32 zmm1, zmmword ptr [rsp+0x1*0x40]
        vmovdqa32 zmm2, zmm0
        vpaddd  zmm2{k1}, zmm0, dword ptr [ADD1_avx5126+rip] {1to16}
        vpcmpltud k2, zmm2, zmm0
        vpaddd  zmm1 {k2}, zmm1, dword ptr [ADD1_avx512+rip] {1to16}
        vmovdqa32 zmmword ptr [rsp], zmm2
        vmovdqa32 zmmword ptr [rsp+0x1*0x40], zmm1
        add     rdi, 128
        add     rbx, 512
        mov     qword ptr [rbp+0x90], rbx
        sub     rsi, 16
        cmp     rsi, 16
        jnc     2b
        test    rsi, rsi
        jne     3f
4:
        vzeroupper
        vmovdqa xmm6, xmmword ptr [rsp+0x90]
        vmovdqa xmm7, xmmword ptr [rsp+0xA0]
        vmovdqa xmm8, xmmword ptr [rsp+0xB0]
        vmovdqa xmm9, xmmword ptr [rsp+0xC0]
        vmovdqa xmm10, xmmword ptr [rsp+0xD0]
        vmovdqa xmm11, xmmword ptr [rsp+0xE0]
        vmovdqa xmm12, xmmword ptr [rsp+0xF0]
        vmovdqa xmm13, xmmword ptr [rsp+0x100]
        vmovdqa xmm14, xmmword ptr [rsp+0x110]
        vmovdqa xmm15, xmmword ptr [rsp+0x120]
        mov     rsp, rbp
        pop     rbp
        pop     rbx
        pop     rsi
        pop     rdi
        pop     r12
        pop     r13
        pop     r14
        pop     r15
        ret
.p2align 6
3:
        test    esi, 0x8
        je      3f
        vpbroadcastd ymm0, dword ptr [rcx]
        vpbroadcastd ymm1, dword ptr [rcx+0x4]
        vpbroadcastd ymm2, dword ptr [rcx+0x8]
        vpbroadcastd ymm3, dword ptr [rcx+0xC]
        vpbroadcastd ymm4, dword ptr [rcx+0x10]
        vpbroadcastd ymm5, dword ptr [rcx+0x14]
        vpbroadcastd ymm6, dword ptr [rcx+0x18]
        vpbroadcastd ymm7, dword ptr [rcx+0x1C]
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        mov     r10, qword ptr [rdi+0x10]
        mov     r11, qword ptr [rdi+0x18]
        mov     r12, qword ptr [rdi+0x20]
        mov     r13, qword ptr [rdi+0x28]
        mov     r14, qword ptr [rdi+0x30]
        mov     r15, qword ptr [rdi+0x38]
        movzx   eax, byte ptr [rbp+0x78]
        movzx   ebx, byte ptr [rbp+0x80]
        or      eax, ebx
        xor     edx, edx
2:
        movzx   ebx, byte ptr [rbp+0x88]
        or      ebx, eax
        add     rdx, 64
        cmp     rdx, qword ptr [rsp+0x80]
        cmove   eax, ebx
        mov     dword ptr [rsp+0x88], eax
        vmovups xmm8, xmmword ptr [r8+rdx-0x40]
        vinsertf128 ymm8, ymm8, xmmword ptr [r12+rdx-0x40], 0x01
        vmovups xmm9, xmmword ptr [r9+rdx-0x40]
        vinsertf128 ymm9, ymm9, xmmword ptr [r13+rdx-0x40], 0x01
        vunpcklpd ymm12, ymm8, ymm9
        vunpckhpd ymm13, ymm8, ymm9
        vmovups xmm10, xmmword ptr [r10+rdx-0x40]
        vinsertf128 ymm10, ymm10, xmmword ptr [r14+rdx-0x40], 0x01
        vmovups xmm11, xmmword ptr [r11+rdx-0x40]
        vinsertf128 ymm11, ymm11, xmmword ptr [r15+rdx-0x40], 0x01
        vunpcklpd ymm14, ymm10, ymm11
        vunpckhpd ymm15, ymm10, ymm11
        vshufps ymm16, ymm12, ymm14, 136
        vshufps ymm17, ymm12, ymm14, 221
        vshufps ymm18, ymm13, ymm15, 136
        vshufps ymm19, ymm13, ymm15, 221
        vmovups xmm8, xmmword ptr [r8+rdx-0x30]
        vinsertf128 ymm8, ymm8, xmmword ptr [r12+rdx-0x30], 0x01
        vmovups xmm9, xmmword ptr [r9+rdx-0x30]
        vinsertf128 ymm9, ymm9, xmmword ptr [r13+rdx-0x30], 0x01
        vunpcklpd ymm12, ymm8, ymm9
        vunpckhpd ymm13, ymm8, ymm9
        vmovups xmm10, xmmword ptr [r10+rdx-0x30]
        vinsertf128 ymm10, ymm10, xmmword ptr [r14+rdx-0x30], 0x01
        vmovups xmm11, xmmword ptr [r11+rdx-0x30]
        vinsertf128 ymm11, ymm11, xmmword ptr [r15+rdx-0x30], 0x01
        vunpcklpd ymm14, ymm10, ymm11
        vunpckhpd ymm15, ymm10, ymm11
        vshufps ymm20, ymm12, ymm14, 136
        vshufps ymm21, ymm12, ymm14, 221
        vshufps ymm22, ymm13, ymm15, 136
        vshufps ymm23, ymm13, ymm15, 221
        vmovups xmm8, xmmword ptr [r8+rdx-0x20]
        vinsertf128 ymm8, ymm8, xmmword ptr [r12+rdx-0x20], 0x01
        vmovups xmm9, xmmword ptr [r9+rdx-0x20]
        vinsertf128 ymm9, ymm9, xmmword ptr [r13+rdx-0x20], 0x01
        vunpcklpd ymm12, ymm8, ymm9
        vunpckhpd ymm13, ymm8, ymm9
        vmovups xmm10, xmmword ptr [r10+rdx-0x20]
        vinsertf128 ymm10, ymm10, xmmword ptr [r14+rdx-0x20], 0x01
        vmovups xmm11, xmmword ptr [r11+rdx-0x20]
        vinsertf128 ymm11, ymm11, xmmword ptr [r15+rdx-0x20], 0x01
        vunpcklpd ymm14, ymm10, ymm11
        vunpckhpd ymm15, ymm10, ymm11
        vshufps ymm24, ymm12, ymm14, 136
        vshufps ymm25, ymm12, ymm14, 221
        vshufps ymm26, ymm13, ymm15, 136
        vshufps ymm27, ymm13, ymm15, 221
        vmovups xmm8, xmmword ptr [r8+rdx-0x10]
        vinsertf128 ymm8, ymm8, xmmword ptr [r12+rdx-0x10], 0x01
        vmovups xmm9, xmmword ptr [r9+rdx-0x10]
        vinsertf128 ymm9, ymm9, xmmword ptr [r13+rdx-0x10], 0x01
        vunpcklpd ymm12, ymm8, ymm9
        vunpckhpd ymm13, ymm8, ymm9
        vmovups xmm10, xmmword ptr [r10+rdx-0x10]
        vinsertf128 ymm10, ymm10, xmmword ptr [r14+rdx-0x10], 0x01
        vmovups xmm11, xmmword ptr [r11+rdx-0x10]
        vinsertf128 ymm11, ymm11, xmmword ptr [r15+rdx-0x10], 0x01
        vunpcklpd ymm14, ymm10, ymm11
        vunpckhpd ymm15, ymm10, ymm11
        vshufps ymm28, ymm12, ymm14, 136
        vshufps ymm29, ymm12, ymm14, 221
        vshufps ymm30, ymm13, ymm15, 136
        vshufps ymm31, ymm13, ymm15, 221
        vpbroadcastd ymm8, dword ptr [BLAKE3_IV_avx512_0+rip]
        vpbroadcastd ymm9, dword ptr [BLAKE3_IV_avx512_1+rip]
        vpbroadcastd ymm10, dword ptr [BLAKE3_IV_avx512_2+rip]
        vpbroadcastd ymm11, dword ptr [BLAKE3_IV_avx512_3+rip]
        vmovdqa ymm12, ymmword ptr [rsp]
        vmovdqa ymm13, ymmword ptr [rsp+0x40]
        vpbroadcastd ymm14, dword ptr [BLAKE3_BLOCK_LEN_avx512+rip]
        vpbroadcastd ymm15, dword ptr [rsp+0x88]
        vpaddd  ymm0, ymm0, ymm16
        vpaddd  ymm1, ymm1, ymm18
        vpaddd  ymm2, ymm2, ymm20
        vpaddd  ymm3, ymm3, ymm22
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vprord  ymm15, ymm15, 16
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 12
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vpaddd  ymm0, ymm0, ymm17
        vpaddd  ymm1, ymm1, ymm19
        vpaddd  ymm2, ymm2, ymm21
        vpaddd  ymm3, ymm3, ymm23
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vprord  ymm15, ymm15, 8
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 7
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vpaddd  ymm0, ymm0, ymm24
        vpaddd  ymm1, ymm1, ymm26
        vpaddd  ymm2, ymm2, ymm28
        vpaddd  ymm3, ymm3, ymm30
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 16
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vprord  ymm4, ymm4, 12
        vpaddd  ymm0, ymm0, ymm25
        vpaddd  ymm1, ymm1, ymm27
        vpaddd  ymm2, ymm2, ymm29
        vpaddd  ymm3, ymm3, ymm31
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 8
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vprord  ymm4, ymm4, 7
        vpaddd  ymm0, ymm0, ymm18
        vpaddd  ymm1, ymm1, ymm19
        vpaddd  ymm2, ymm2, ymm23
        vpaddd  ymm3, ymm3, ymm20
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vprord  ymm15, ymm15, 16
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 12
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vpaddd  ymm0, ymm0, ymm22
        vpaddd  ymm1, ymm1, ymm26
        vpaddd  ymm2, ymm2, ymm16
        vpaddd  ymm3, ymm3, ymm29
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vprord  ymm15, ymm15, 8
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 7
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vpaddd  ymm0, ymm0, ymm17
        vpaddd  ymm1, ymm1, ymm28
        vpaddd  ymm2, ymm2, ymm25
        vpaddd  ymm3, ymm3, ymm31
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 16
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vprord  ymm4, ymm4, 12
        vpaddd  ymm0, ymm0, ymm27
        vpaddd  ymm1, ymm1, ymm21
        vpaddd  ymm2, ymm2, ymm30
        vpaddd  ymm3, ymm3, ymm24
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 8
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vprord  ymm4, ymm4, 7
        vpaddd  ymm0, ymm0, ymm19
        vpaddd  ymm1, ymm1, ymm26
        vpaddd  ymm2, ymm2, ymm29
        vpaddd  ymm3, ymm3, ymm23
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vprord  ymm15, ymm15, 16
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 12
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vpaddd  ymm0, ymm0, ymm20
        vpaddd  ymm1, ymm1, ymm28
        vpaddd  ymm2, ymm2, ymm18
        vpaddd  ymm3, ymm3, ymm30
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vprord  ymm15, ymm15, 8
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 7
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vpaddd  ymm0, ymm0, ymm22
        vpaddd  ymm1, ymm1, ymm25
        vpaddd  ymm2, ymm2, ymm27
        vpaddd  ymm3, ymm3, ymm24
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 16
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vprord  ymm4, ymm4, 12
        vpaddd  ymm0, ymm0, ymm21
        vpaddd  ymm1, ymm1, ymm16
        vpaddd  ymm2, ymm2, ymm31
        vpaddd  ymm3, ymm3, ymm17
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 8
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vprord  ymm4, ymm4, 7
        vpaddd  ymm0, ymm0, ymm26
        vpaddd  ymm1, ymm1, ymm28
        vpaddd  ymm2, ymm2, ymm30
        vpaddd  ymm3, ymm3, ymm29
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vprord  ymm15, ymm15, 16
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 12
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vpaddd  ymm0, ymm0, ymm23
        vpaddd  ymm1, ymm1, ymm25
        vpaddd  ymm2, ymm2, ymm19
        vpaddd  ymm3, ymm3, ymm31
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vprord  ymm15, ymm15, 8
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 7
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vpaddd  ymm0, ymm0, ymm20
        vpaddd  ymm1, ymm1, ymm27
        vpaddd  ymm2, ymm2, ymm21
        vpaddd  ymm3, ymm3, ymm17
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 16
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vprord  ymm4, ymm4, 12
        vpaddd  ymm0, ymm0, ymm16
        vpaddd  ymm1, ymm1, ymm18
        vpaddd  ymm2, ymm2, ymm24
        vpaddd  ymm3, ymm3, ymm22
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 8
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vprord  ymm4, ymm4, 7
        vpaddd  ymm0, ymm0, ymm28
        vpaddd  ymm1, ymm1, ymm25
        vpaddd  ymm2, ymm2, ymm31
        vpaddd  ymm3, ymm3, ymm30
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vprord  ymm15, ymm15, 16
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 12
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vpaddd  ymm0, ymm0, ymm29
        vpaddd  ymm1, ymm1, ymm27
        vpaddd  ymm2, ymm2, ymm26
        vpaddd  ymm3, ymm3, ymm24
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vprord  ymm15, ymm15, 8
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 7
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vpaddd  ymm0, ymm0, ymm23
        vpaddd  ymm1, ymm1, ymm21
        vpaddd  ymm2, ymm2, ymm16
        vpaddd  ymm3, ymm3, ymm22
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 16
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vprord  ymm4, ymm4, 12
        vpaddd  ymm0, ymm0, ymm18
        vpaddd  ymm1, ymm1, ymm19
        vpaddd  ymm2, ymm2, ymm17
        vpaddd  ymm3, ymm3, ymm20
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 8
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vprord  ymm4, ymm4, 7
        vpaddd  ymm0, ymm0, ymm25
        vpaddd  ymm1, ymm1, ymm27
        vpaddd  ymm2, ymm2, ymm24
        vpaddd  ymm3, ymm3, ymm31
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vprord  ymm15, ymm15, 16
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 12
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vpaddd  ymm0, ymm0, ymm30
        vpaddd  ymm1, ymm1, ymm21
        vpaddd  ymm2, ymm2, ymm28
        vpaddd  ymm3, ymm3, ymm17
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vprord  ymm15, ymm15, 8
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 7
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vpaddd  ymm0, ymm0, ymm29
        vpaddd  ymm1, ymm1, ymm16
        vpaddd  ymm2, ymm2, ymm18
        vpaddd  ymm3, ymm3, ymm20
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 16
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vprord  ymm4, ymm4, 12
        vpaddd  ymm0, ymm0, ymm19
        vpaddd  ymm1, ymm1, ymm26
        vpaddd  ymm2, ymm2, ymm22
        vpaddd  ymm3, ymm3, ymm23
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 8
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vprord  ymm4, ymm4, 7
        vpaddd  ymm0, ymm0, ymm27
        vpaddd  ymm1, ymm1, ymm21
        vpaddd  ymm2, ymm2, ymm17
        vpaddd  ymm3, ymm3, ymm24
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vprord  ymm15, ymm15, 16
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 12
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vpaddd  ymm0, ymm0, ymm31
        vpaddd  ymm1, ymm1, ymm16
        vpaddd  ymm2, ymm2, ymm25
        vpaddd  ymm3, ymm3, ymm22
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm1, ymm1, ymm5
        vpaddd  ymm2, ymm2, ymm6
        vpaddd  ymm3, ymm3, ymm7
        vpxord  ymm12, ymm12, ymm0
        vpxord  ymm13, ymm13, ymm1
        vpxord  ymm14, ymm14, ymm2
        vpxord  ymm15, ymm15, ymm3
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vprord  ymm15, ymm15, 8
        vpaddd  ymm8, ymm8, ymm12
        vpaddd  ymm9, ymm9, ymm13
        vpaddd  ymm10, ymm10, ymm14
        vpaddd  ymm11, ymm11, ymm15
        vpxord  ymm4, ymm4, ymm8
        vpxord  ymm5, ymm5, ymm9
        vpxord  ymm6, ymm6, ymm10
        vpxord  ymm7, ymm7, ymm11
        vprord  ymm4, ymm4, 7
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vpaddd  ymm0, ymm0, ymm30
        vpaddd  ymm1, ymm1, ymm18
        vpaddd  ymm2, ymm2, ymm19
        vpaddd  ymm3, ymm3, ymm23
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 16
        vprord  ymm12, ymm12, 16
        vprord  ymm13, ymm13, 16
        vprord  ymm14, ymm14, 16
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 12
        vprord  ymm6, ymm6, 12
        vprord  ymm7, ymm7, 12
        vprord  ymm4, ymm4, 12
        vpaddd  ymm0, ymm0, ymm26
        vpaddd  ymm1, ymm1, ymm28
        vpaddd  ymm2, ymm2, ymm20
        vpaddd  ymm3, ymm3, ymm29
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm1, ymm1, ymm6
        vpaddd  ymm2, ymm2, ymm7
        vpaddd  ymm3, ymm3, ymm4
        vpxord  ymm15, ymm15, ymm0
        vpxord  ymm12, ymm12, ymm1
        vpxord  ymm13, ymm13, ymm2
        vpxord  ymm14, ymm14, ymm3
        vprord  ymm15, ymm15, 8
        vprord  ymm12, ymm12, 8
        vprord  ymm13, ymm13, 8
        vprord  ymm14, ymm14, 8
        vpaddd  ymm10, ymm10, ymm15
        vpaddd  ymm11, ymm11, ymm12
        vpaddd  ymm8, ymm8, ymm13
        vpaddd  ymm9, ymm9, ymm14
        vpxord  ymm5, ymm5, ymm10
        vpxord  ymm6, ymm6, ymm11
        vpxord  ymm7, ymm7, ymm8
        vpxord  ymm4, ymm4, ymm9
        vprord  ymm5, ymm5, 7
        vprord  ymm6, ymm6, 7
        vprord  ymm7, ymm7, 7
        vprord  ymm4, ymm4, 7
        vpxor   ymm0, ymm0, ymm8
        vpxor   ymm1, ymm1, ymm9
        vpxor   ymm2, ymm2, ymm10
        vpxor   ymm3, ymm3, ymm11
        vpxor   ymm4, ymm4, ymm12
        vpxor   ymm5, ymm5, ymm13
        vpxor   ymm6, ymm6, ymm14
        vpxor   ymm7, ymm7, ymm15
        movzx   eax, byte ptr [rbp+0x78]
        jne     2b
        mov     rbx, qword ptr [rbp+0x90]
        vunpcklps ymm8, ymm0, ymm1
        vunpcklps ymm9, ymm2, ymm3
        vunpckhps ymm10, ymm0, ymm1
        vunpcklps ymm11, ymm4, ymm5
        vunpcklps ymm0, ymm6, ymm7
        vshufps ymm12, ymm8, ymm9, 78
        vblendps ymm1, ymm8, ymm12, 0xCC
        vshufps ymm8, ymm11, ymm0, 78
        vunpckhps ymm13, ymm2, ymm3
        vblendps ymm2, ymm11, ymm8, 0xCC
        vblendps ymm3, ymm12, ymm9, 0xCC
        vperm2f128 ymm12, ymm1, ymm2, 0x20
        vmovups ymmword ptr [rbx], ymm12
        vunpckhps ymm14, ymm4, ymm5
        vblendps ymm4, ymm8, ymm0, 0xCC
        vunpckhps ymm15, ymm6, ymm7
        vperm2f128 ymm7, ymm3, ymm4, 0x20
        vmovups ymmword ptr [rbx+0x20], ymm7
        vshufps ymm5, ymm10, ymm13, 78
        vblendps ymm6, ymm5, ymm13, 0xCC
        vshufps ymm13, ymm14, ymm15, 78
        vblendps ymm10, ymm10, ymm5, 0xCC
        vblendps ymm14, ymm14, ymm13, 0xCC
        vperm2f128 ymm8, ymm10, ymm14, 0x20
        vmovups ymmword ptr [rbx+0x40], ymm8
        vblendps ymm15, ymm13, ymm15, 0xCC
        vperm2f128 ymm13, ymm6, ymm15, 0x20
        vmovups ymmword ptr [rbx+0x60], ymm13
        vperm2f128 ymm9, ymm1, ymm2, 0x31
        vperm2f128 ymm11, ymm3, ymm4, 0x31
        vmovups ymmword ptr [rbx+0x80], ymm9
        vperm2f128 ymm14, ymm10, ymm14, 0x31
        vperm2f128 ymm15, ymm6, ymm15, 0x31
        vmovups ymmword ptr [rbx+0xA0], ymm11
        vmovups ymmword ptr [rbx+0xC0], ymm14
        vmovups ymmword ptr [rbx+0xE0], ymm15
        vmovdqa ymm0, ymmword ptr [rsp]
        vmovdqa ymm2, ymmword ptr [rsp+0x40]
        vmovdqa32 ymm0 {k1}, ymmword ptr [rsp+0x1*0x20]
        vmovdqa32 ymm2 {k1}, ymmword ptr [rsp+0x3*0x20]
        vmovdqa ymmword ptr [rsp], ymm0
        vmovdqa ymmword ptr [rsp+0x40], ymm2
        add     rbx, 256
        mov     qword ptr [rbp+0x90], rbx
        add     rdi, 64
        sub     rsi, 8
3:
        mov     rbx, qword ptr [rbp+0x90]
        mov     r15, qword ptr [rsp+0x80]
        movzx   r13, byte ptr [rbp+0x78]
        movzx   r12, byte ptr [rbp+0x88]
        test    esi, 0x4
        je      3f
        vbroadcasti32x4 zmm0, xmmword ptr [rcx]
        vbroadcasti32x4 zmm1, xmmword ptr [rcx+0x1*0x10]
        vmovdqa xmm12, xmmword ptr [rsp]
        vmovdqa xmm13, xmmword ptr [rsp+0x40]
        vpunpckldq xmm14, xmm12, xmm13
        vpunpckhdq xmm15, xmm12, xmm13
        vpermq  ymm14, ymm14, 0xDC
        vpermq  ymm15, ymm15, 0xDC
        vpbroadcastd zmm12, dword ptr [BLAKE3_BLOCK_LEN_avx512+rip]
        vinserti64x4 zmm13, zmm14, ymm15, 0x01
        mov     eax, 17476
        kmovw   k2, eax
        vpblendmd zmm13 {k2}, zmm13, zmm12
        vbroadcasti32x4 zmm15, xmmword ptr [BLAKE3_IV_avx512+rip]
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        mov     r10, qword ptr [rdi+0x10]
        mov     r11, qword ptr [rdi+0x18]
        mov     eax, 43690
        kmovw   k3, eax
        mov     eax, 34952
        kmovw   k4, eax
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
.p2align 5
2:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        mov     dword ptr [rsp+0x88], eax
        vmovdqa32 zmm2, zmm15
        vpbroadcastd zmm8, dword ptr [rsp+0x22*0x4]
        vpblendmd zmm3 {k4}, zmm13, zmm8
        vmovups zmm8, zmmword ptr [r8+rdx-0x1*0x40]
        vinserti32x4 zmm8, zmm8, xmmword ptr [r9+rdx-0x4*0x10], 0x01
        vinserti32x4 zmm8, zmm8, xmmword ptr [r10+rdx-0x4*0x10], 0x02
        vinserti32x4 zmm8, zmm8, xmmword ptr [r11+rdx-0x4*0x10], 0x03
        vmovups zmm9, zmmword ptr [r8+rdx-0x30]
        vinserti32x4 zmm9, zmm9, xmmword ptr [r9+rdx-0x3*0x10], 0x01
        vinserti32x4 zmm9, zmm9, xmmword ptr [r10+rdx-0x3*0x10], 0x02
        vinserti32x4 zmm9, zmm9, xmmword ptr [r11+rdx-0x3*0x10], 0x03
        vshufps zmm4, zmm8, zmm9, 136
        vshufps zmm5, zmm8, zmm9, 221
        vmovups zmm8, zmmword ptr [r8+rdx-0x20]
        vinserti32x4 zmm8, zmm8, xmmword ptr [r9+rdx-0x2*0x10], 0x01
        vinserti32x4 zmm8, zmm8, xmmword ptr [r10+rdx-0x2*0x10], 0x02
        vinserti32x4 zmm8, zmm8, xmmword ptr [r11+rdx-0x2*0x10], 0x03
        vmovups zmm9, zmmword ptr [r8+rdx-0x10]
        vinserti32x4 zmm9, zmm9, xmmword ptr [r9+rdx-0x1*0x10], 0x01
        vinserti32x4 zmm9, zmm9, xmmword ptr [r10+rdx-0x1*0x10], 0x02
        vinserti32x4 zmm9, zmm9, xmmword ptr [r11+rdx-0x1*0x10], 0x03
        vshufps zmm6, zmm8, zmm9, 136
        vshufps zmm7, zmm8, zmm9, 221
        vpshufd zmm6, zmm6, 0x93
        vpshufd zmm7, zmm7, 0x93
        mov     al, 7
9:
        vpaddd  zmm0, zmm0, zmm4
        vpaddd  zmm0, zmm0, zmm1
        vpxord  zmm3, zmm3, zmm0
        vprord  zmm3, zmm3, 16
        vpaddd  zmm2, zmm2, zmm3
        vpxord  zmm1, zmm1, zmm2
        vprord  zmm1, zmm1, 12
        vpaddd  zmm0, zmm0, zmm5
        vpaddd  zmm0, zmm0, zmm1
        vpxord  zmm3, zmm3, zmm0
        vprord  zmm3, zmm3, 8
        vpaddd  zmm2, zmm2, zmm3
        vpxord  zmm1, zmm1, zmm2
        vprord  zmm1, zmm1, 7
        vpshufd zmm0, zmm0, 0x93
        vpshufd zmm3, zmm3, 0x4E
        vpshufd zmm2, zmm2, 0x39
        vpaddd  zmm0, zmm0, zmm6
        vpaddd  zmm0, zmm0, zmm1
        vpxord  zmm3, zmm3, zmm0
        vprord  zmm3, zmm3, 16
        vpaddd  zmm2, zmm2, zmm3
        vpxord  zmm1, zmm1, zmm2
        vprord  zmm1, zmm1, 12
        vpaddd  zmm0, zmm0, zmm7
        vpaddd  zmm0, zmm0, zmm1
        vpxord  zmm3, zmm3, zmm0
        vprord  zmm3, zmm3, 8
        vpaddd  zmm2, zmm2, zmm3
        vpxord  zmm1, zmm1, zmm2
        vprord  zmm1, zmm1, 7
        vpshufd zmm0, zmm0, 0x39
        vpshufd zmm3, zmm3, 0x4E
        vpshufd zmm2, zmm2, 0x93
        dec     al
        jz      9f
        vshufps zmm8, zmm4, zmm5, 214
        vpshufd zmm9, zmm4, 0x0F
        vpshufd zmm4, zmm8, 0x39
        vshufps zmm8, zmm6, zmm7, 250
        vpblendmd zmm9 {k3}, zmm9, zmm8
        vpunpcklqdq zmm8, zmm7, zmm5
        vpblendmd zmm8 {k4}, zmm8, zmm6
        vpshufd zmm8, zmm8, 0x78
        vpunpckhdq zmm5, zmm5, zmm7
        vpunpckldq zmm6, zmm6, zmm5
        vpshufd zmm7, zmm6, 0x1E
        vmovdqa32 zmm5, zmm9
        vmovdqa32 zmm6, zmm8
        jmp     9b
9:
        vpxord  zmm0, zmm0, zmm2
        vpxord  zmm1, zmm1, zmm3
        mov     eax, r13d
        cmp     rdx, r15
        jne     2b
        vmovdqu xmmword ptr [rbx], xmm0
        vmovdqu xmmword ptr [rbx+0x10], xmm1
        vextracti128 xmmword ptr [rbx+0x20], ymm0, 0x01
        vextracti128 xmmword ptr [rbx+0x30], ymm1, 0x01
        vextracti32x4 xmmword ptr [rbx+0x4*0x10], zmm0, 0x02
        vextracti32x4 xmmword ptr [rbx+0x5*0x10], zmm1, 0x02
        vextracti32x4 xmmword ptr [rbx+0x6*0x10], zmm0, 0x03
        vextracti32x4 xmmword ptr [rbx+0x7*0x10], zmm1, 0x03
        vmovdqa xmm0, xmmword ptr [rsp]
        vmovdqa xmm2, xmmword ptr [rsp+0x40]
        vmovdqa32 xmm0 {k1}, xmmword ptr [rsp+0x1*0x10]
        vmovdqa32 xmm2 {k1}, xmmword ptr [rsp+0x5*0x10]
        vmovdqa xmmword ptr [rsp], xmm0
        vmovdqa xmmword ptr [rsp+0x40], xmm2
        add     rbx, 128
        add     rdi, 32
        sub     rsi, 4
3:
        test    esi, 0x2
        je      3f
        vbroadcasti128 ymm0, xmmword ptr [rcx]
        vbroadcasti128 ymm1, xmmword ptr [rcx+0x10]
        vmovd   xmm13, dword ptr [rsp]
        vpinsrd xmm13, xmm13, dword ptr [rsp+0x40], 1
        vpinsrd xmm13, xmm13, dword ptr [BLAKE3_BLOCK_LEN_avx512+rip], 2
        vmovd   xmm14, dword ptr [rsp+0x4]
        vpinsrd xmm14, xmm14, dword ptr [rsp+0x44], 1
        vpinsrd xmm14, xmm14, dword ptr [BLAKE3_BLOCK_LEN_avx512+rip], 2
        vinserti128 ymm13, ymm13, xmm14, 0x01
        mov     r8, qword ptr [rdi]
        mov     r9, qword ptr [rdi+0x8]
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
.p2align 5
2:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        mov     dword ptr [rsp+0x88], eax
        vbroadcasti128 ymm2, xmmword ptr [BLAKE3_IV_avx512+rip]
        vpbroadcastd ymm8, dword ptr [rsp+0x88]
        vpblendd ymm3, ymm13, ymm8, 0x88
        vmovups ymm8, ymmword ptr [r8+rdx-0x40]
        vinsertf128 ymm8, ymm8, xmmword ptr [r9+rdx-0x40], 0x01
        vmovups ymm9, ymmword ptr [r8+rdx-0x30]
        vinsertf128 ymm9, ymm9, xmmword ptr [r9+rdx-0x30], 0x01
        vshufps ymm4, ymm8, ymm9, 136
        vshufps ymm5, ymm8, ymm9, 221
        vmovups ymm8, ymmword ptr [r8+rdx-0x20]
        vinsertf128 ymm8, ymm8, xmmword ptr [r9+rdx-0x20], 0x01
        vmovups ymm9, ymmword ptr [r8+rdx-0x10]
        vinsertf128 ymm9, ymm9, xmmword ptr [r9+rdx-0x10], 0x01
        vshufps ymm6, ymm8, ymm9, 136
        vshufps ymm7, ymm8, ymm9, 221
        vpshufd ymm6, ymm6, 0x93
        vpshufd ymm7, ymm7, 0x93
        mov     al, 7
9:
        vpaddd  ymm0, ymm0, ymm4
        vpaddd  ymm0, ymm0, ymm1
        vpxord  ymm3, ymm3, ymm0
        vprord  ymm3, ymm3, 16
        vpaddd  ymm2, ymm2, ymm3
        vpxord  ymm1, ymm1, ymm2
        vprord  ymm1, ymm1, 12
        vpaddd  ymm0, ymm0, ymm5
        vpaddd  ymm0, ymm0, ymm1
        vpxord  ymm3, ymm3, ymm0
        vprord  ymm3, ymm3, 8
        vpaddd  ymm2, ymm2, ymm3
        vpxord  ymm1, ymm1, ymm2
        vprord  ymm1, ymm1, 7
        vpshufd ymm0, ymm0, 0x93
        vpshufd ymm3, ymm3, 0x4E
        vpshufd ymm2, ymm2, 0x39
        vpaddd  ymm0, ymm0, ymm6
        vpaddd  ymm0, ymm0, ymm1
        vpxord  ymm3, ymm3, ymm0
        vprord  ymm3, ymm3, 16
        vpaddd  ymm2, ymm2, ymm3
        vpxord  ymm1, ymm1, ymm2
        vprord  ymm1, ymm1, 12
        vpaddd  ymm0, ymm0, ymm7
        vpaddd  ymm0, ymm0, ymm1
        vpxord  ymm3, ymm3, ymm0
        vprord  ymm3, ymm3, 8
        vpaddd  ymm2, ymm2, ymm3
        vpxord  ymm1, ymm1, ymm2
        vprord  ymm1, ymm1, 7
        vpshufd ymm0, ymm0, 0x39
        vpshufd ymm3, ymm3, 0x4E
        vpshufd ymm2, ymm2, 0x93
        dec     al
        jz      9f
        vshufps ymm8, ymm4, ymm5, 214
        vpshufd ymm9, ymm4, 0x0F
        vpshufd ymm4, ymm8, 0x39
        vshufps ymm8, ymm6, ymm7, 250
        vpblendd ymm9, ymm9, ymm8, 0xAA
        vpunpcklqdq ymm8, ymm7, ymm5
        vpblendd ymm8, ymm8, ymm6, 0x88
        vpshufd ymm8, ymm8, 0x78
        vpunpckhdq ymm5, ymm5, ymm7
        vpunpckldq ymm6, ymm6, ymm5
        vpshufd ymm7, ymm6, 0x1E
        vmovdqa ymm5, ymm9
        vmovdqa ymm6, ymm8
        jmp     9b
9:
        vpxor   ymm0, ymm0, ymm2
        vpxor   ymm1, ymm1, ymm3
        mov     eax, r13d
        cmp     rdx, r15
        jne     2b
        vmovdqu xmmword ptr [rbx], xmm0
        vmovdqu xmmword ptr [rbx+0x10], xmm1
        vextracti128 xmmword ptr [rbx+0x20], ymm0, 0x01
        vextracti128 xmmword ptr [rbx+0x30], ymm1, 0x01
        vmovdqa xmm0, xmmword ptr [rsp]
        vmovdqa xmm2, xmmword ptr [rsp+0x40]
        vmovdqu32 xmm0 {k1}, xmmword ptr [rsp+0x8]
        vmovdqu32 xmm2 {k1}, xmmword ptr [rsp+0x48]
        vmovdqa xmmword ptr [rsp], xmm0
        vmovdqa xmmword ptr [rsp+0x40], xmm2
        add     rbx, 64
        add     rdi, 16
        sub     rsi, 2
3:
        test    esi, 0x1
        je      4b
        vmovdqu xmm0, xmmword ptr [rcx]
        vmovdqu xmm1, xmmword ptr [rcx+0x10]
        vmovd   xmm14, dword ptr [rsp]
        vpinsrd xmm14, xmm14, dword ptr [rsp+0x40], 1
        vpinsrd xmm14, xmm14, dword ptr [BLAKE3_BLOCK_LEN_avx512+rip], 2
        vmovdqa xmm15, xmmword ptr [BLAKE3_IV_avx512+rip]
        mov     r8, qword ptr [rdi]
        movzx   eax, byte ptr [rbp+0x80]
        or      eax, r13d
        xor     edx, edx
.p2align 5
2:
        mov     r14d, eax
        or      eax, r12d
        add     rdx, 64
        cmp     rdx, r15
        cmovne  eax, r14d
        vpinsrd xmm3, xmm14, eax, 3
        vmovdqa xmm2, xmm15
        vmovups xmm8, xmmword ptr [r8+rdx-0x40]
        vmovups xmm9, xmmword ptr [r8+rdx-0x30]
        vshufps xmm4, xmm8, xmm9, 136
        vshufps xmm5, xmm8, xmm9, 221
        vmovups xmm8, xmmword ptr [r8+rdx-0x20]
        vmovups xmm9, xmmword ptr [r8+rdx-0x10]
        vshufps xmm6, xmm8, xmm9, 136
        vshufps xmm7, xmm8, xmm9, 221
        vpshufd xmm6, xmm6, 0x93
        vpshufd xmm7, xmm7, 0x93
        mov     al, 7
9:
        vpaddd  xmm0, xmm0, xmm4
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 16
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 12
        vpaddd  xmm0, xmm0, xmm5
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 8
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 7
        vpshufd xmm0, xmm0, 0x93
        vpshufd xmm3, xmm3, 0x4E
        vpshufd xmm2, xmm2, 0x39
        vpaddd  xmm0, xmm0, xmm6
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 16
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 12
        vpaddd  xmm0, xmm0, xmm7
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 8
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 7
        vpshufd xmm0, xmm0, 0x39
        vpshufd xmm3, xmm3, 0x4E
        vpshufd xmm2, xmm2, 0x93
        dec     al
        jz      9f
        vshufps xmm8, xmm4, xmm5, 214
        vpshufd xmm9, xmm4, 0x0F
        vpshufd xmm4, xmm8, 0x39
        vshufps xmm8, xmm6, xmm7, 250
        vpblendd xmm9, xmm9, xmm8, 0xAA
        vpunpcklqdq xmm8, xmm7, xmm5
        vpblendd xmm8, xmm8, xmm6, 0x88
        vpshufd xmm8, xmm8, 0x78
        vpunpckhdq xmm5, xmm5, xmm7
        vpunpckldq xmm6, xmm6, xmm5
        vpshufd xmm7, xmm6, 0x1E
        vmovdqa xmm5, xmm9
        vmovdqa xmm6, xmm8
        jmp     9b
9:
        vpxor   xmm0, xmm0, xmm2
        vpxor   xmm1, xmm1, xmm3
        mov     eax, r13d
        cmp     rdx, r15
        jne     2b
        vmovdqu xmmword ptr [rbx], xmm0
        vmovdqu xmmword ptr [rbx+0x10], xmm1
        jmp     4b


.p2align 6
_blake3_compress_in_place_avx512:
blake3_compress_in_place_avx512:
        sub     rsp, 72
        vmovdqa xmmword ptr [rsp], xmm6
        vmovdqa xmmword ptr [rsp+0x10], xmm7
        vmovdqa xmmword ptr [rsp+0x20], xmm8
        vmovdqa xmmword ptr [rsp+0x30], xmm9
        vmovdqu xmm0, xmmword ptr [rcx]
        vmovdqu xmm1, xmmword ptr [rcx+0x10]
        movzx   eax, byte ptr [rsp+0x70]
        movzx   r8d, r8b
        shl     rax, 32
        add     r8, rax
        vmovq   xmm3, r9
        vmovq   xmm4, r8
        vpunpcklqdq xmm3, xmm3, xmm4
        vmovaps xmm2, xmmword ptr [BLAKE3_IV_avx512+rip]
        vmovups xmm8, xmmword ptr [rdx]
        vmovups xmm9, xmmword ptr [rdx+0x10]
        vshufps xmm4, xmm8, xmm9, 136
        vshufps xmm5, xmm8, xmm9, 221
        vmovups xmm8, xmmword ptr [rdx+0x20]
        vmovups xmm9, xmmword ptr [rdx+0x30]
        vshufps xmm6, xmm8, xmm9, 136
        vshufps xmm7, xmm8, xmm9, 221
        vpshufd xmm6, xmm6, 0x93
        vpshufd xmm7, xmm7, 0x93
        mov     al, 7
9:
        vpaddd  xmm0, xmm0, xmm4
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 16
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 12
        vpaddd  xmm0, xmm0, xmm5
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 8
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 7
        vpshufd xmm0, xmm0, 0x93
        vpshufd xmm3, xmm3, 0x4E
        vpshufd xmm2, xmm2, 0x39
        vpaddd  xmm0, xmm0, xmm6
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 16
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 12
        vpaddd  xmm0, xmm0, xmm7
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 8
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 7
        vpshufd xmm0, xmm0, 0x39
        vpshufd xmm3, xmm3, 0x4E
        vpshufd xmm2, xmm2, 0x93
        dec     al
        jz      9f
        vshufps xmm8, xmm4, xmm5, 214
        vpshufd xmm9, xmm4, 0x0F
        vpshufd xmm4, xmm8, 0x39
        vshufps xmm8, xmm6, xmm7, 250
        vpblendd xmm9, xmm9, xmm8, 0xAA
        vpunpcklqdq xmm8, xmm7, xmm5
        vpblendd xmm8, xmm8, xmm6, 0x88
        vpshufd xmm8, xmm8, 0x78
        vpunpckhdq xmm5, xmm5, xmm7
        vpunpckldq xmm6, xmm6, xmm5
        vpshufd xmm7, xmm6, 0x1E
        vmovdqa xmm5, xmm9
        vmovdqa xmm6, xmm8
        jmp     9b
9:
        vpxor   xmm0, xmm0, xmm2
        vpxor   xmm1, xmm1, xmm3
        vmovdqu xmmword ptr [rcx], xmm0
        vmovdqu xmmword ptr [rcx+0x10], xmm1
        vmovdqa xmm6, xmmword ptr [rsp]
        vmovdqa xmm7, xmmword ptr [rsp+0x10]
        vmovdqa xmm8, xmmword ptr [rsp+0x20]
        vmovdqa xmm9, xmmword ptr [rsp+0x30]
        add     rsp, 72
        ret


.p2align 6
_blake3_compress_xof_avx512:
blake3_compress_xof_avx512:
        sub     rsp, 72
        vmovdqa xmmword ptr [rsp], xmm6
        vmovdqa xmmword ptr [rsp+0x10], xmm7
        vmovdqa xmmword ptr [rsp+0x20], xmm8
        vmovdqa xmmword ptr [rsp+0x30], xmm9
        vmovdqu xmm0, xmmword ptr [rcx]
        vmovdqu xmm1, xmmword ptr [rcx+0x10]
        movzx   eax, byte ptr [rsp+0x70]
        movzx   r8d, r8b
        mov     r10, qword ptr [rsp+0x78]
        shl     rax, 32
        add     r8, rax
        vmovq   xmm3, r9
        vmovq   xmm4, r8
        vpunpcklqdq xmm3, xmm3, xmm4
        vmovaps xmm2, xmmword ptr [BLAKE3_IV_avx512+rip]
        vmovups xmm8, xmmword ptr [rdx]
        vmovups xmm9, xmmword ptr [rdx+0x10]
        vshufps xmm4, xmm8, xmm9, 136
        vshufps xmm5, xmm8, xmm9, 221
        vmovups xmm8, xmmword ptr [rdx+0x20]
        vmovups xmm9, xmmword ptr [rdx+0x30]
        vshufps xmm6, xmm8, xmm9, 136
        vshufps xmm7, xmm8, xmm9, 221
        vpshufd xmm6, xmm6, 0x93
        vpshufd xmm7, xmm7, 0x93
        mov     al, 7
9:
        vpaddd  xmm0, xmm0, xmm4
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 16
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 12
        vpaddd  xmm0, xmm0, xmm5
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 8
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 7
        vpshufd xmm0, xmm0, 0x93
        vpshufd xmm3, xmm3, 0x4E
        vpshufd xmm2, xmm2, 0x39
        vpaddd  xmm0, xmm0, xmm6
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 16
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 12
        vpaddd  xmm0, xmm0, xmm7
        vpaddd  xmm0, xmm0, xmm1
        vpxord  xmm3, xmm3, xmm0
        vprord  xmm3, xmm3, 8
        vpaddd  xmm2, xmm2, xmm3
        vpxord  xmm1, xmm1, xmm2
        vprord  xmm1, xmm1, 7
        vpshufd xmm0, xmm0, 0x39
        vpshufd xmm3, xmm3, 0x4E
        vpshufd xmm2, xmm2, 0x93
        dec     al
        jz      9f
        vshufps xmm8, xmm4, xmm5, 214
        vpshufd xmm9, xmm4, 0x0F
        vpshufd xmm4, xmm8, 0x39
        vshufps xmm8, xmm6, xmm7, 250
        vpblendd xmm9, xmm9, xmm8, 0xAA
        vpunpcklqdq xmm8, xmm7, xmm5
        vpblendd xmm8, xmm8, xmm6, 0x88
        vpshufd xmm8, xmm8, 0x78
        vpunpckhdq xmm5, xmm5, xmm7
        vpunpckldq xmm6, xmm6, xmm5
        vpshufd xmm7, xmm6, 0x1E
        vmovdqa xmm5, xmm9
        vmovdqa xmm6, xmm8
        jmp     9b
9:
        vpxor   xmm0, xmm0, xmm2
        vpxor   xmm1, xmm1, xmm3
        vpxor   xmm2, xmm2, xmmword ptr [rcx]
        vpxor   xmm3, xmm3, xmmword ptr [rcx+0x10]
        vmovdqu xmmword ptr [r10], xmm0
        vmovdqu xmmword ptr [r10+0x10], xmm1
        vmovdqu xmmword ptr [r10+0x20], xmm2
        vmovdqu xmmword ptr [r10+0x30], xmm3
        vmovdqa xmm6, xmmword ptr [rsp]
        vmovdqa xmm7, xmmword ptr [rsp+0x10]
        vmovdqa xmm8, xmmword ptr [rsp+0x20]
        vmovdqa xmm9, xmmword ptr [rsp+0x30]
        add     rsp, 72
        ret

.section .rodata
.p2align  6
INDEX0:
        .long    0,  1,  2,  3, 16, 17, 18, 19
        .long    8,  9, 10, 11, 24, 25, 26, 27
INDEX1:
        .long    4,  5,  6,  7, 20, 21, 22, 23
        .long   12, 13, 14, 15, 28, 29, 30, 31
ADD0_avx512:
        .long    0,  1,  2,  3,  4,  5,  6,  7
        .long    8,  9, 10, 11, 12, 13, 14, 15
ADD1_avx512:   .long    1

ADD1_avx5126:  .long   16
BLAKE3_BLOCK_LEN_avx512:
        .long   64
.p2align 6
BLAKE3_IV_avx512:
BLAKE3_IV_avx512_0:
        .long   0x6A09E667
BLAKE3_IV_avx512_1:
        .long   0xBB67AE85
BLAKE3_IV_avx512_2:
        .long   0x3C6EF372
BLAKE3_IV_avx512_3:
        .long   0xA54FF53A
