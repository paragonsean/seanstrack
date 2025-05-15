
	.include "excalibur.s"

	.text
;----------------------------------------------------------------------
;
; nm_print_test.asm
;
;----------------------------------------------------------------------

.global _start
_start:

	MOVIP		%g2,0xDEADBEEF

	NM_D_TXCHAR	'\n'
	NM_D_TXREGISTER	'g','2',%g2
	NM_D_TXCHAR	'\n'

	NM_PRINTREGISTER %g2

	NM_PRINTCHAR	'\n'
	NM_PRINTLN	"Hello (via assembly & JTAG!) from Nios."
	NM_PRINTCHAR	'\n'

	NM_PRINT	"ABC"

	NM_PRINT2CHARS	'1','2'
	NM_PRINTCHAR	'3'
	NM_PRINTLN

	MOVI		%g0,0
infinite:
	BR		infinite
	ADDI		%g0,1

; end of file
