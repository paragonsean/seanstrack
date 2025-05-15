

	.include "excalibur.s"
	.text

;----------------------------------------
;         Name: nr_getctlreg(x)
;  Description: Return value of a control register
;        Input: %o0 control register to read, 0..7
;       Output: %o0 contents of control register
; Side Effects: uses %g0
;    CWP Depth: 0
;

	
	; Since there's no way to move a register
	; value into the Special K (pfx) register,
	; we need to straightline it. Fortunately,
	; a SKP/IF instruction skips over a pfx, too,
	; so it's not so bad.
	
	.macro get_ctl_if x
	CMPI	%o0,\x
	IFS	cc_eq
	 PFX	\x
	 RDCTL	%g0
	.endm

	.global	nr_getctlreg

nr_getctlreg:
	MOVI	%g0,0
	get_ctl_if 0
	get_ctl_if 1
	get_ctl_if 2
	get_ctl_if 3
	get_ctl_if 4
	get_ctl_if 5
	get_ctl_if 6
	get_ctl_if 7
	JMP	%o7
	MOV	%o0,%g0

; End of file.
