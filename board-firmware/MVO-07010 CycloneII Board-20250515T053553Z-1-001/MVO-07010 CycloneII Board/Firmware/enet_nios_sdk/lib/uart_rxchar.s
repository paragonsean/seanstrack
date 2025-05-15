
		.include "excalibur.s"
		.text

;----------------------------------------
;         Name: nr_uart_rxchar
;  Description: Read character if available
;        Input: %o0: UART base to use
;       Output: %o0 = character 0-0xff, or -1 if none present
; Side Effects: %g0 & %g1 altered
;    CWP Depth: 0
;

	.global	nr_uart_rxchar
nr_uart_rxchar:

.ifdef nasys_printf_uart
	IFRnz	%o0		; if %o0 is zero, use "default" uart
	BR	rxCharA
	NOP
	MOVIA   %o0,nasys_printf_uart
.endif ; nasys_printf_uart

rxCharA:

	MOV	%g0,%o0		; %g0 -> uart
	MOVI	%o0,1
	NEG	%o0	    	; %o0 gets -1 by default
	_PFX	2		; cache safety
	LD	%g1,[%g0]	; status register
	SKP0	%g1,7	; rx buffer empty? if no, keep the -1
	_PFX	0		; cache safety
	LD	%o0,[%g0]	; something for us? read into %o0.

	JMP	%o7
	CMPI	%o0,0	; Set condition flags as per char.

; end of file
