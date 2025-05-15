

		.include "nios.s"
		.text

;----------------------------------------
;         Name: nr_uart_txcr
;  Description: Print CR/LF
;        Input: none
;       Output: none
; Side Effects: %g0 & %g1 altered
;    CWP Depth: 1
;

		.global	nr_uart_txcr
nr_uart_txcr:
	SAVE	%sp,-16

	MOVI	%o1,0		; use "default" uart

	_BSR	nr_uart_txchar
	MOVI	%o0,13		; delay slot

	_BSR	nr_uart_txchar
	MOVI	%o0,10		; delay slot

	RESTRET

