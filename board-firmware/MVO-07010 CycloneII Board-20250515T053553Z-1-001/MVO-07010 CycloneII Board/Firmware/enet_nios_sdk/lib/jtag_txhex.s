
		.include "excalibur.s"

		.text
;----------------------------------------
;         Name: nr_jtag_txhex
;  Description: Print value in hexadecimal
;        Input: %o0 = value to be printed
;       Output: none
; Side Effects: %g0 & %g1 altered, %o1 used for jtag base
;    CWP Depth: 1
;

		.global nr_jtag_txhex
nr_jtag_txhex:
	SAVE	%sp,-16
.ifdef __nios32__
	MOVI	%l0,28	; initial shift: 8 hex digits on n32
.else
	MOVI	%l0,12	; initial shift: 4 hex digits on n16
.endif

txHexLoop:
	MOV	%o0,%i0

	LSR	%o0,%l0


	ANDIP	%o0,0x000f	; mask low hex digit
	CMPI	%o0,10		; convert to ascii 0-9a-f
	SKPS	cc_lt
	ADDI	%o0,'A'-'0'-10
	PFX	%hi('0')
	ADDI	%o0,%lo('0')
	MOVI	%o1,0		; use "default" jtag
	_BSR	nr_jtag_txchar
	NOP
	SUBI	%l0,4		; shift a little less, next loop
	SKPS	cc_n
	BR	txHexLoop
	NOP

	RESTRET


; End of file

