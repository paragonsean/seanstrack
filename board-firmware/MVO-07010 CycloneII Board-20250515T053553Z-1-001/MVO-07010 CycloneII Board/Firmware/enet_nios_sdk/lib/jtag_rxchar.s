

		.include "excalibur.s"
		.text

;----------------------------------------
;         Name: nr_jtag_rxchar
;  Description: Receive a single byte through the OCI JTAG, if available
;        Input: %o0: OCI base to use, 0 for default
;       Output: %o0 = character 0-0xff, or -1 if none present
; Side Effects: %g0 & %g1 altered
;    CWP Depth: 0
;

	.global	nr_jtag_rxchar
nr_jtag_rxchar:

.ifdef nasys_printf_uart
	IFRnz	%o0					; if %o0 is zero, use "default" (if 0x00000000 was indicated, no harm done)
	BR		rxCharA
	NOP
	MOVIA   %o0,nasys_printf_uart
.endif ; nasys_printf_uart

rxCharA:

	PFX	%hi(np_jtagrxdata)
	ADDI	%o0,%lo(np_jtagrxdata)

	MOV	%g1,%o0					; %g1 -> JTAG OCI base

	MOVI	%o0,1
	NEG	%o0					; %o0 gets -1 by default

	_PFX	0					; cache safety

	LD	%g0,[%g1]				; see what's waiting
.ifdef __nios32__
	EXT16D	%g0,%g1
.endif
	SKP1	%g0,np_jtagstatus_rrdy_bit		; check the ready bit
	BR	rxCharDone;

	PFX	%hi(np_jtagstatus_trdy_mask)		; there is a byte waiting for us
	XOR	%g0,%lo(np_jtagstatus_trdy_mask)	; clear the ready bit
	MOV	%o0,%g0					; replace the default return value with the real thing
.ifdef __nios32__
	FILL16	%r0,%g0
	ST16D	[%g1],%r0				; and tell the OCI JTAG we're ready for another
.else
	ST	[%g1],%r0
.endif

rxCharDone:

	JMP	%o7
	CMPI	%o0,0					; Set condition flags as per char.

; end of file
