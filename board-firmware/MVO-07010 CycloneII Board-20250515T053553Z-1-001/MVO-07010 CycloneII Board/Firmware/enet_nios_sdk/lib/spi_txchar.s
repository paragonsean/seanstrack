

		.include "nios.s"
		.text

;----------------------------------------
;         Name: nr_spi_txchar
;  Description: Send a single byte out the spi
;        Input: %o0 = A character
;               %o1 = the np_spi to use
;       Output: none
; Side Effects: %g0 & %g1 altered, CPU waits for spi
;    CWP Depth: 0
;

; nr_spi_txchar
		.global	nr_spi_txchar
nr_spi_txchar:
; If %o1 is 0, the code below was supposed to substitute 
; a "default" spi.  Unfortunately, nasys_printf_spi is not
; defined.
;	IFRnz	%o1
;	 BR	txCharWait
;	NOP
;	MOVIA	%o1,nasys_printf_spi
txCharWait:
	PFX	np_spistatus			; status register for spi
	LD	%g1,[%o1]
	SKP1	%g1,np_spistatus_trdy_bit	; check the ready bit
	BR	txCharWait			; loop until empty
	NOP

	; Put character to out & return

	PFX	np_spitxdata
	ST	[%o1],%o0	; put char into UART
	JMP	%o7
	NOP

