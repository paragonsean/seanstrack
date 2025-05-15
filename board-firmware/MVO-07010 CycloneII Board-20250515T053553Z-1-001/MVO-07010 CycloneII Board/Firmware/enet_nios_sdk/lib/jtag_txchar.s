

		.include "excalibur.s"
		.text

;----------------------------------------
;         Name: nr_jtag_txchar
;  Description: Send a single byte out the OCI JTAG
;        Input: %o0 = A character
;               %o1 = the base address of the OCI, 0 for default
;       Output: none
; Side Effects: %g0 & %g1 altered, CPU waits for OCI
;    CWP Depth: 0
;

; nr_jtag_txchar
		.global	nr_jtag_txchar
nr_jtag_txchar:

	MOV	%g1,%o1

.ifdef nasys_printf_uart
	IFRnz	%g1					; if 0, use "default" (if 0x00000000 was indicated, no harm done)
	BR		txCharA
	NOP
	MOVIA	%g1,nasys_printf_uart
.endif ; nasys_printf_uart

txCharA:

	PFX	%hi(np_jtagtxdata)
	ADDI	%g1,%lo(np_jtagtxdata)

txCharWait:
	_PFX	0		; cache safety
	LD	%g0,[%g1]
.ifdef __nios32__
	EXT16D	%g0,%g1
.endif
	SKP0	%g0,np_jtagstatus_trdy_bit		; check the ready bit
	BR	txCharWait				; loop until empty
	NOP

	; Put character to out & return

	MOV	%g0,%o0
	PFX	%hi(np_jtagstatus_trdy_mask)
	OR	%g0,%lo(np_jtagstatus_trdy_mask)	; set the ready bit
.ifdef __nios32__
	FILL16	%r0,%g0
	ST16D	[%g1],%r0				; put char into OCI JTAG register
.else
	ST	[%g1],%r0
.endif
	JMP	%o7
	NOP

