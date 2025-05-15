

		.include "excalibur.s"
		.text

;----------------------------------------
;         Name: nr_zerorange
;  Description: Clear a range of memory
;        Input: %o0 = first address, %o1 = byte count
;       Output: none
; Side Effects: %g0, %g1, & %g2
;    CWP Depth: 0
;

	.global	nr_zerorange
nr_zerorange:
	IFRz	%o1		; byte count zero?
	 JMP	%o7		; just exit

	MOV	%g2,%o0		; %g2 = first byte address
	MOV	%g1,%g2
	ADD	%g1,%o1		; %g1 = last address + 1
	SUBI	%g1,1		; %g1 = last byte address
	MOVI	%r0,0		; %r0 (%g0) is always ST8d's source.
;
; Byte by byte, for simplicity.
; Not as fast, though.
;
zeroRangeLoop:
	ST8d	[%g2],%r0
	CMP	%g2,%g1
	IFS	cc_ne
	 BR	zeroRangeLoop
	ADDI	%g2,1		; (delay slot)

	JMP	%o7
	NOP

