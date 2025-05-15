

		.include "nios.s"
		.text

;----------------------------------------
;         Name: nr_uart_txchar
;  Description: Send a single byte out the UART
;        Input: %o0 = A character
;               %o1 = the UART to use, 0 for default
;       Output: none
; Side Effects: %g0 & %g1 altered, CPU waits for UART
;    CWP Depth: 0
;

; nr_uart_txchar
		.global	nr_uart_txchar
nr_uart_txchar:

.ifdef nasys_printf_uart
	IFRnz	%o1
	 BR	txCharWait
	NOP
	MOVIA	%o1,nasys_printf_uart
.endif ; nasys_printf_uart

txCharWait:
	_PFX	np_uartstatus			; status register for uart
	LD	%g1,[%o1]
	SKP1	%g1,np_uartstatus_trdy_bit	; check the ready bit
	BR	txCharWait			; loop until empty
	NOP

	; Put character to out & return

	PFX	np_uarttxdata
	ST	[%o1],%o0	; put char into UART
	JMP	%o7
	NOP

