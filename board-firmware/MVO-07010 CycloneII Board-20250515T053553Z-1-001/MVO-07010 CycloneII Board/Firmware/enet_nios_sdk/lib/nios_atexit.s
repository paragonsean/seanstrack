; +-------------------------
; | file: nios_atexit.s
; |
; | Implements the standard C library routine
; | "atexit", which registers a routine to be
; | called after "main" exits.
; |

	.include "excalibur.s"

	.comm atexit_func,nios_wordsize,nios_wordsize	; one word, word-aligned

	.global atexit

atexit:
	MOVIA	%g0,atexit_func
	ST	[%g0],%o0
	JMP	%o7
	MOVI	%o0,0	; (delay slot) return 0: aok

; end of file
