
	.text
	.global _getpid


; ----------------------------
; _getpid	used by c++
; Do nothing, and return no error!
;
_getpid:
	JMP	%o7
	MOVI	%o0,0


; end of file
