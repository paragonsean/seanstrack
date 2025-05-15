;
; Nios implementation of setjmp
;
; Note that you must include "nios.h"
; (or "nios.s") to get this routine,
; rather than <setjmp.h>.
;
; setjmp() and longjmp() are both
; included here. If you use one, you'll
; most likely want both. Yeah.
;

	.include "excalibur.s"
	.text

;----------------------------------------
;         Name: _setjmp
;  Description: Save the current context so
;               a _longjmp works later.
;        Input: %o0: jmp_buf: (ptr to) array to store context in
;       Output: %o0 = 0 the first time we're called, or
;               whatever longjmp returns later
; Side Effects: 
;    CWP Depth: 0
;

	.global	_setjmp
	.global	setjmp
_setjmp:
setjmp:
	ST	[%o0],%i7	; present return address
	PFX	1
	ST	[%o0],%o7	; where the longjmp will later execute from
	JMP	%o7
	MOVI	%o0,0		; (delay slot) return value is zero


;----------------------------------------
;         Name: _longjmp
;  Description: Restore the current context
;               as saved by a previous _setjmp
;        Input: %o0: jmp_buf (ptr to) array to restore context from
;               %o1: integer to return
;       Output: %o0 = 0 the first time we're called, or
;               whatever longjmp returns later
; Side Effects: uses %g0, %g1 & %g2
;    CWP Depth: 0
;

	.global	_longjmp
	.global	longjmp
_longjmp:
longjmp:
	;
	; The way we'll do this is by executing
	; RESTORE instructions until the old
	; return address matches. Then we'll
	; jump to where setjmp was called from.
	;
	; Since we're moving the window pointer
	; all over the place, we'll naturally
	; only use the %g registers.
	;

	MOV	%g0,%o0		; %g0 -> jmp_buf
	MOV	%g1,%o1		; %g1 = return value
	LD	%g2,[%g0]	; %g2 = old return address
nr_longjmp_loop:
	CMP	%g2,%i7		; Are we there yet?
	IFS	cc_eq
	 BR	nr_longjmp_done
	 NOP			; (delay slot)

	BR	nr_longjmp_loop
	RESTORE			; (delay slot)
	;
	; One might put in a watchdog counter here, to
	; prevent a runaway stack crawl... but what would that
	; accomplish? What error can we throw? To whom?
	;

nr_longjmp_done:
	PFX	1
	LD	%o7,[%g0]	; set fake return address
	JMP	%o7		; and kinda return there.
	MOV	%o0,%g1		; (delay slot) return value

; end of file
