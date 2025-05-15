
		.include "nios.s"

		.text
;----------------------------------------
;         Name: nr_uart_txstring
;  Description: Print zero-terminated string
;        Input: %o0 = pointer to string
;       Output: none
; Side Effects: %g0 & %g1 altered, %o1 for default uart
;    CWP Depth: 1
;

		.global	nr_uart_txstring
nr_uart_txstring:
	SAVE	%sp,-16
	MOV	%l0,%i0

txStringChar:
	LD	%o0,[%l0]		
	EXT8D	%o0,%l0		; o0 now has byte from l0

	IFRz	%o0		; bail out on zero
	 BR	txStringEnd
	NOP

	MOVI	%o1,0		; default uart
	_BSR	nr_uart_txchar	; send what's in o0
	NOP

	BR	txStringChar; go do next
	ADDI	%l0,#1		; delay slot

txStringEnd:
	RESTRET

;end of file
