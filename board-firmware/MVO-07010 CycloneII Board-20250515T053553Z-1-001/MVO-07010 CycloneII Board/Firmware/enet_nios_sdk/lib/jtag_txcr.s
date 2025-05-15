

		.include "excalibur.s"
		.text

;----------------------------------------
;         Name: nr_jtag_txcr
;  Description: Print CR/LF
;        Input: none
;       Output: none
; Side Effects: %g0 & %g1 altered
;    CWP Depth: 1
;

		.global	nr_jtag_txcr
nr_jtag_txcr:
	SAVE	%sp,-16

	MOVI	%o1,0		; use "default" jtag

	_BSR	nr_jtag_txchar
	MOVI	%o0,13		; delay slot

	_BSR	nr_jtag_txchar
	MOVI	%o0,10		; delay slot

	RESTRET

