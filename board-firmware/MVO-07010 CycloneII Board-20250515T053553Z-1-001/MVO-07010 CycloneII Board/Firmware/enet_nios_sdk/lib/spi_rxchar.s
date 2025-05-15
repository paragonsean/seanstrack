
		.include "nios.s"
		.text

;----------------------------------------
;         Name: nr_spi_rxchar
;  Description: Read character if available
;        Input: %o0: np_spi base to use
;       Output: %o0 = character 0-0xff, or -1 if none present
; Side Effects: %g0 & %g1 altered
;    CWP Depth: 0
;

	.global	nr_spi_rxchar
nr_spi_rxchar:

	MOV	%g0,%o0		; %g0 -> spi
	MOVI	%o0,1
	NEG	%o0		; %o0 gets -1 by default
	PFX	#2
	LD	%g1,[%g0]	; status register
	SKP0	%g1,7		; rx buffer empty? if no, keep the -1
	LD	%o0,[%g0]	; something for us? read into %o0.

	JMP	%o7
	CMPI	%o0,0		; Set condition flags as per char.

; end of file
