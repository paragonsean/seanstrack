; file: nios_math1.s
;
; dvb 2000...
;
; This file contains several versions of
; multiplication for the Nios CPU.
;
; Which one depends on your system as described
; in excalibur.s
;
; For testing purposes, you can force your own
; settings. --defsym __nios_testing_mul__=1.
;
;

.ifndef __nios_testing_mul__
	.include "excalibur.s"
.endif

.ifdef __nios32__
.if __nios_use_fast_mul__


	.text
	.globl __mulsi3
	.globl __mulhi3
	.type	 __mulsi3,@function
	.type	 __mulhi3,@function


.if __nios_use_multiply__

__mulsi3:

	; Sources are on %o0 and %o1, we call X and Y
	; Start multipling X-high and Y-low
	MOV	%g0,%o0
	MUL %o1			; %g0 = xLo . yLo
	MOV	%g1,%g0		; %g1 = xLo . yLo

	MOV	%g0,%o0
	SWAP %g0
	MUL %o1			; %g0 = xHi . yLo >>16
	MOV	%g2,%g0		; %g2 = xHi . yLo >>16

	MOV	%g0,%o0
	SWAP %o1
	MUL %o1			; %g0 = xLo . yHi >>16

	ADD	%g0,%g2		; %o0 = (xLo . yHi + xHi . yLo) >>16
	MOVHI	%g0,0		; throw away the bits above 32
	SWAP %g0			; %g0 = xLo . yHi + xHi . yLo

	ADD	%g0,%g1		; %g0 = xLo . yHi + xHi . yLo + xLo . yLo
	JMP	%o7
	MOV	%o0,%g0		; (delay slot) %o0 = xLo . yHi + xHi . yLo + xLo . yLo

__mulhi3:
    MOV %r0,%o1
    MUL %o0
    JMP %o7
    MOV %o0,%r0     ; (delay slot)


.endif

.if (__nios_use_mstep__) && (!__nios_use_multiply__)

__mulsi3:


	#
	# Multiplication with Nios
	# MSTEP instruction
	# (Not recognized by assembler, so
	# it is here macro'd)
	#

	.MACRO	MSTEP_R1
	.HALF	0x7e81
	.ENDM

	.MACRO MSTEP_R1_16x
	;
	; multiply high bits of %g0 with low bits of %g1
	; assume low bits of %g0 and high bits of %g1 are already ZERO
	; 

    .rept 16
    MSTEP %r1
    .endr

	.ENDM

	; Sources are on %o0 and %o1, we call X and Y
	; Start multipling X-high and Y-low
	MOV	%g0,%o0		
	PFX	%hi(0xffff)
	ANDN	%g0,%lo(0xffff)	; clear low bits of X
	MOV	%g1,%o1
	MOVHI	%g1,0		; clear high bits of Y
	MSTEP_R1_16x

	MOV	%g2,%g0		; g2 := (Xh * Yl) >> 16

	; Multiply X-low and Y-high
	MOV	%g0,%o1		
	PFX	%hi(0xffff)
	ANDN	%g0,%lo(0xffff)	; clear low bits of X
	MOV	%g1,%o0
	MOVHI	%g1,0		; clear high bits of Y
	MSTEP_R1_16x

	ADD	%g2,%g0		; g2 := (Xh * Yl + Xl * Yh) >> 16
	MOVHI	%g2,0
	SWAP %g2	;%g2

	; Multiply X-low and Y-low
	MOV	%g0,%o0
	MOVHI	%g0,0
	SWAP %g0	;%g0
	MOV	%g1,%o1
	EXT16s	%g1,0
	MSTEP_R1_16x

	ADD	%g2,%g0
	JMP	%o7
	MOV	%o0,%g2

; |
; | halfword multiply using mstep
; |

__mulhi3:
    MOV     %r0,%o0
    MOVHI   %r0,0
    SWAP    %r0           ; %r0 now has op1 in high 16 bits

    MOVHI   %o1,0         ; clear upper 16 bits of op2

    .rept   16
    MSTEP   %o1           ; 16 msteps gets the job done
    .endr

    SEXT16  %r0           ; sign-extend the low 16 bit result
    JMP     %o7           ; thanks!
    MOV     %o0,%g0       ; (delay slot)


.endif

.if (!__nios_use_mstep__) && (!__nios_use_multiply__)

__mulsi3:

	#
	# Unrolled loop multiplication
	# routine. For full 32 bit multiplications,
	# this is faster than GNU's default one.
	#


		.MACRO ZSTEP bit
		SKP0	%o0,\bit
		ADD	    %g0,%o1
		LSLI	%o1,1
		.ENDM

	MOVI	%g0,0
	ZSTEP	0
	ZSTEP	1
	ZSTEP	2
	ZSTEP	3
	ZSTEP	4
	ZSTEP	5
	ZSTEP	6
	ZSTEP	7
	ZSTEP	8
	ZSTEP	9
	ZSTEP	10
	ZSTEP	11
	ZSTEP	12
	ZSTEP	13
	ZSTEP	14
	ZSTEP	15
	ZSTEP	16
	ZSTEP	17
	ZSTEP	18
	ZSTEP	19
	ZSTEP	20
	ZSTEP	21
	ZSTEP	22
	ZSTEP	23
	ZSTEP	24
	ZSTEP	25
	ZSTEP	26
	ZSTEP	27
	ZSTEP	28
	ZSTEP	29
	ZSTEP	30
	ZSTEP	31
	; No bit 31: we already set %o0 to positive

	JMP	%o7
	MOV	%o0,%g0

; |
; | halfword unrolled loop multiply
; |

__mulhi3:
    MOVI    %g0,0
    ZSTEP   0
	ZSTEP	1
	ZSTEP	2
	ZSTEP	3
	ZSTEP	4
	ZSTEP	5
	ZSTEP	6
	ZSTEP	7
	ZSTEP	8
	ZSTEP	9
	ZSTEP	10
	ZSTEP	11
	ZSTEP	12
	ZSTEP	13
	ZSTEP	14
	ZSTEP	15

    SEXT16  %g0         ; sign extend for truer 16 bitness
    JMP     %o7         ; thanks, byebye
    MOV     %o0,%g0     ; (delay slot) catch the anwer as you go!

.endif

.Lfe1:
	.size	 __mulsi3,.Lfe1-__mulsi3

.endif
.endif


; end of file
