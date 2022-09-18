; Sha1Opt.asm -- SHA-1 optimized code for SHA-1 x86 hardware instructions
; 2021-03-10 : Igor Pavlov : Public domain

; 7zAsm.asm -- ASM macros
; 2021-02-07 : Igor Pavlov : Public domain

ifdef RAX
  x64 equ 1
endif

ifdef x64
  IS_X64 equ 1
else
  IS_X64 equ 0
endif

ifdef ABI_LINUX
  IS_LINUX equ 1
else
  IS_LINUX equ 0
endif

ifndef x64
; Use ABI_CDECL for x86 (32-bit) only
; if ABI_CDECL is not defined, we use fastcall abi
ifdef ABI_CDECL
  IS_CDECL equ 1
else
  IS_CDECL equ 0
endif
endif


MY_ASM_START macro
  ifdef x64
    .code
  else
    .386
    .model flat
    _TEXT$00 SEGMENT PARA PUBLIC 'CODE'
  endif
endm

MY_PROC macro name:req, numParams:req
  align 16
  proc_numParams = numParams
  if (IS_X64 gt 0)
    proc_name equ name
  elseif (IS_LINUX gt 0)
    proc_name equ name
  elseif (IS_CDECL gt 0)
    proc_name equ @CatStr(_,name)
  else
    proc_name equ @CatStr(@,name,@, %numParams * 4)
  endif
  proc_name PROC
endm

MY_ENDP macro
    if (IS_X64 gt 0)
        ret
    elseif (IS_CDECL gt 0)
        ret
    elseif (proc_numParams LT 3)
        ret
    else
        ret (proc_numParams - 2) * 4
    endif
  proc_name ENDP
endm


ifdef x64
  REG_SIZE equ 8
  REG_LOGAR_SIZE equ 3
else
  REG_SIZE equ 4
  REG_LOGAR_SIZE equ 2
endif

  x0 equ EAX
  x1 equ ECX
  x2 equ EDX
  x3 equ EBX
  x4 equ ESP
  x5 equ EBP
  x6 equ ESI
  x7 equ EDI

  x0_W equ AX
  x1_W equ CX
  x2_W equ DX
  x3_W equ BX

  x5_W equ BP
  x6_W equ SI
  x7_W equ DI

  x0_L equ AL
  x1_L equ CL
  x2_L equ DL
  x3_L equ BL

  x0_H equ AH
  x1_H equ CH
  x2_H equ DH
  x3_H equ BH

ifdef x64
  x5_L equ BPL
  x6_L equ SIL
  x7_L equ DIL

  r0 equ RAX
  r1 equ RCX
  r2 equ RDX
  r3 equ RBX
  r4 equ RSP
  r5 equ RBP
  r6 equ RSI
  r7 equ RDI
  x8 equ r8d
  x9 equ r9d
  x10 equ r10d
  x11 equ r11d
  x12 equ r12d
  x13 equ r13d
  x14 equ r14d
  x15 equ r15d
else
  r0 equ x0
  r1 equ x1
  r2 equ x2
  r3 equ x3
  r4 equ x4
  r5 equ x5
  r6 equ x6
  r7 equ x7
endif


ifdef x64
ifdef ABI_LINUX

MY_PUSH_2_REGS macro
    push    r3
    push    r5
endm

MY_POP_2_REGS macro
    pop     r5
    pop     r3
endm

endif
endif


MY_PUSH_4_REGS macro
    push    r3
    push    r5
    push    r6
    push    r7
endm

MY_POP_4_REGS macro
    pop     r7
    pop     r6
    pop     r5
    pop     r3
endm


; for fastcall and for WIN-x64
REG_PARAM_0_x   equ x1
REG_PARAM_0     equ r1
REG_PARAM_1     equ r2

ifndef x64
; for x86-fastcall

REG_ABI_PARAM_0_x equ REG_PARAM_0_x
REG_ABI_PARAM_0   equ REG_PARAM_0
REG_ABI_PARAM_1   equ REG_PARAM_1

else
; x64

if  (IS_LINUX eq 0)

; for WIN-x64:
REG_PARAM_2 equ r8
REG_PARAM_3 equ r9

REG_ABI_PARAM_0_x equ REG_PARAM_0_x
REG_ABI_PARAM_0   equ REG_PARAM_0
REG_ABI_PARAM_1   equ REG_PARAM_1
REG_ABI_PARAM_2   equ REG_PARAM_2
REG_ABI_PARAM_3   equ REG_PARAM_3

else
; for LINUX-x64:
REG_LINUX_PARAM_0_x equ x7
REG_LINUX_PARAM_0 equ r7
REG_LINUX_PARAM_1 equ r6
REG_LINUX_PARAM_2 equ r2
REG_LINUX_PARAM_3 equ r1

REG_ABI_PARAM_0_x equ REG_LINUX_PARAM_0_x
REG_ABI_PARAM_0   equ REG_LINUX_PARAM_0
REG_ABI_PARAM_1   equ REG_LINUX_PARAM_1
REG_ABI_PARAM_2   equ REG_LINUX_PARAM_2
REG_ABI_PARAM_3   equ REG_LINUX_PARAM_3

MY_ABI_LINUX_TO_WIN_2 macro
        mov     r2, r6
        mov     r1, r7
endm

MY_ABI_LINUX_TO_WIN_3 macro
        mov     r8, r2
        mov     r2, r6
        mov     r1, r7
endm

MY_ABI_LINUX_TO_WIN_4 macro
        mov     r9, r1
        mov     r8, r2
        mov     r2, r6
        mov     r1, r7
endm

endif ; IS_LINUX


MY_PUSH_PRESERVED_ABI_REGS macro
    if  (IS_LINUX gt 0)
        MY_PUSH_2_REGS
    else
        MY_PUSH_4_REGS
    endif
        push    r12
        push    r13
        push    r14
        push    r15
endm


MY_POP_PRESERVED_ABI_REGS macro
        pop     r15
        pop     r14
        pop     r13
        pop     r12
    if  (IS_LINUX gt 0)
        MY_POP_2_REGS
    else
        MY_POP_4_REGS
    endif
endm

endif ; x64


MY_ASM_START
















CONST   SEGMENT

align 16
Reverse_Endian_Mask db 15,14,13,12, 11,10,9,8, 7,6,5,4, 3,2,1,0






















CONST   ENDS

; _TEXT$SHA1OPT SEGMENT 'CODE'

ifndef x64
    .686
    .xmm
endif

ifdef x64
        rNum    equ REG_ABI_PARAM_2
    if (IS_LINUX eq 0)
        LOCAL_SIZE equ (16 * 2)
    endif
else
        rNum    equ r0
        LOCAL_SIZE equ (16 * 1)
endif

rState equ REG_ABI_PARAM_0
rData  equ REG_ABI_PARAM_1


MY_sha1rnds4 macro a1, a2, imm
        db 0fH, 03aH, 0ccH, (0c0H + a1 * 8 + a2), imm
endm

MY_SHA_INSTR macro cmd, a1, a2
        db 0fH, 038H, cmd, (0c0H + a1 * 8 + a2)
endm

cmd_sha1nexte   equ 0c8H
cmd_sha1msg1    equ 0c9H
cmd_sha1msg2    equ 0caH

MY_sha1nexte macro a1, a2
        MY_SHA_INSTR  cmd_sha1nexte, a1, a2
endm

MY_sha1msg1 macro a1, a2
        MY_SHA_INSTR  cmd_sha1msg1, a1, a2
endm

MY_sha1msg2 macro a1, a2
        MY_SHA_INSTR  cmd_sha1msg2, a1, a2
endm

MY_PROLOG macro
    ifdef x64
      if (IS_LINUX eq 0)
        movdqa  [r4 + 8], xmm6
        movdqa  [r4 + 8 + 16], xmm7
        sub     r4, LOCAL_SIZE + 8
        movdqa  [r4     ], xmm8
        movdqa  [r4 + 16], xmm9
      endif
    else ; x86
      if (IS_CDECL gt 0)
        mov     rState, [r4 + REG_SIZE * 1]
        mov     rData,  [r4 + REG_SIZE * 2]
        mov     rNum,   [r4 + REG_SIZE * 3]
      else ; fastcall
        mov     rNum,   [r4 + REG_SIZE * 1]
      endif
        push    r5
        mov     r5, r4
        and     r4, -16
        sub     r4, LOCAL_SIZE
    endif
endm

MY_EPILOG macro
    ifdef x64
      if (IS_LINUX eq 0)
        movdqa  xmm8, [r4]
        movdqa  xmm9, [r4 + 16]
        add     r4, LOCAL_SIZE + 8
        movdqa  xmm6, [r4 + 8]
        movdqa  xmm7, [r4 + 8 + 16]
      endif
    else ; x86
        mov     r4, r5
        pop     r5
    endif
    MY_ENDP
endm


e0_N       equ 0
e1_N       equ 1
abcd_N     equ 2
e0_save_N  equ 3
w_regs     equ 4

e0      equ @CatStr(xmm, %e0_N)
e1      equ @CatStr(xmm, %e1_N)
abcd    equ @CatStr(xmm, %abcd_N)
e0_save equ @CatStr(xmm, %e0_save_N)


ifdef x64
        abcd_save    equ  xmm8
        mask2        equ  xmm9
else
        abcd_save    equ  [r4]
        mask2        equ  e1
endif

LOAD_MASK macro
        movdqa  mask2, XMMWORD PTR Reverse_Endian_Mask
endm

LOAD_W macro k:req
        movdqu  @CatStr(xmm, %(w_regs + k)), [rData + (16 * (k))]
        pshufb  @CatStr(xmm, %(w_regs + k)), mask2
endm


; pre2 can be 2 or 3 (recommended)
pre2 equ 3
pre1 equ (pre2 + 1)

NUM_ROUNDS4 equ 20
   
RND4 macro k
        movdqa  @CatStr(xmm, %(e0_N + ((k + 1) mod 2))), abcd
        MY_sha1rnds4 abcd_N, (e0_N + (k mod 2)), k / 5

        nextM = (w_regs + ((k + 1) mod 4))

    if (k EQ NUM_ROUNDS4 - 1)
        nextM = e0_save_N
    endif
        
        MY_sha1nexte (e0_N + ((k + 1) mod 2)), nextM
        
    if (k GE (4 - pre2)) AND (k LT (NUM_ROUNDS4 - pre2))
        pxor @CatStr(xmm, %(w_regs + ((k + pre2) mod 4))), @CatStr(xmm, %(w_regs + ((k + pre2 - 2) mod 4)))
    endif

    if (k GE (4 - pre1)) AND (k LT (NUM_ROUNDS4 - pre1))
        MY_sha1msg1 (w_regs + ((k + pre1) mod 4)), (w_regs + ((k + pre1 - 3) mod 4))
    endif
    
    if (k GE (4 - pre2)) AND (k LT (NUM_ROUNDS4 - pre2))
        MY_sha1msg2 (w_regs + ((k + pre2) mod 4)), (w_regs + ((k + pre2 - 1) mod 4))
    endif
endm


REVERSE_STATE macro
                               ; abcd   ; dcba
                               ; e0     ; 000e
        pshufd  abcd, abcd, 01bH        ; abcd
        pshufd    e0,   e0, 01bH        ; e000
endm





MY_PROC Sha1_UpdateBlocks_HW, 3
    MY_PROLOG

        cmp     rNum, 0
        je      end_c

        movdqu   abcd, [rState]               ; dcba
        movd     e0, dword ptr [rState + 16]  ; 000e

        REVERSE_STATE
       
        ifdef x64
        LOAD_MASK
        endif

    align 16
    nextBlock:
        movdqa  abcd_save, abcd
        movdqa  e0_save, e0
        
        ifndef x64
        LOAD_MASK
        endif
        
        LOAD_W 0
        LOAD_W 1
        LOAD_W 2
        LOAD_W 3

        paddd   e0, @CatStr(xmm, %(w_regs))
        k = 0
        rept NUM_ROUNDS4
          RND4 k
          k = k + 1
        endm

        paddd   abcd, abcd_save


        add     rData, 64
        sub     rNum, 1
        jnz     nextBlock
        
        REVERSE_STATE

        movdqu  [rState], abcd
        movd    dword ptr [rState + 16], e0
       
  end_c:
MY_EPILOG

; _TEXT$SHA1OPT ENDS

end
