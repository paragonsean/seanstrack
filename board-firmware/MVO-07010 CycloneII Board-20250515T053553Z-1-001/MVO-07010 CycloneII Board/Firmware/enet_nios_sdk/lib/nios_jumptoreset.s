

	.include "excalibur.s"
	.text

;----------------------------------------
;         Name: nr_jumptoreset(void)
;  Description: Jump to nasys_reset_address
;        Input: none
;       Output: none
; Side Effects: Shouldnt care: your system's being reset
;    CWP Depth: 0
;

	.global	nr_jumptoreset

nr_jumptoreset:
	MOVIP	%g0,(nasys_reset_address/2)
	JMP	%g0
	NOP


;----------------------------------------
;         Name: nr_callfromreset(destination)
;  Description: call the destination after preloading the
;               return address with the reset address, so
;               when they're done, it's reset time.
;        Input: %o0 (arg1): destination address
;       Output: none
; Side Effects: Shouldnt care: your system's being reset

    .global nr_callfromreset

nr_callfromreset:
    MOVIP   %o7,(nasys_reset_address/2)
    JMP     %o0
    NOP
; End of file.
