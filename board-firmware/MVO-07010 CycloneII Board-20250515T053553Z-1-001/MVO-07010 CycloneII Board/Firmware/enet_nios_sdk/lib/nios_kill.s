	.text
	.global _kill

; ----------------------------
; _kill		used by c++
; Do nothing, and return no error!
;
_kill:
	JMP	%o7
	MOVI	%o0,0


; end of file
