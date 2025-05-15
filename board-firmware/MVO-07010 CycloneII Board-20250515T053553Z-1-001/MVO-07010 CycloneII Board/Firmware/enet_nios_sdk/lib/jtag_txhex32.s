
		.include "excalibur.s"

		.text
;----------------------------------------
;         Name: nr_jtag_txhex32
;  Description: Print 32-bit value in hexadecimal
;        Input: %o0 = value to be printed
;       Output: none
; Side Effects: %g0 & %g1 altered, %o1 used for jtag base
;    CWP Depth: 1
;

		.global nr_jtag_txhex32
nr_jtag_txhex32:
	SAVE	%sp,-16
	MOVI	%l0,28	; initial shift: 7 hex digits on d32

txHex32Loop:
	MOV	%o0,%i0

	LSR	%o0,%l0

	ANDIP	%o0,0x000f	; mask low hex digit
	CMPI	%o0,10		; convert to ascii 0-9a-f
	SKPS	cc_lt
	ADDI	%o0,'A'-'0'-10
	PFX	%hi('0')
	ADDI	%o0,%lo('0')
	_BSR	nr_jtag_txchar
	MOVI	%o1,0		; (branch delay slot) use "default" jtag
	SUBI	%l0,4		; shift a little less, next loop
	SKPS	cc_n
	BR	txHex32Loop
	NOP

		RESTRET


