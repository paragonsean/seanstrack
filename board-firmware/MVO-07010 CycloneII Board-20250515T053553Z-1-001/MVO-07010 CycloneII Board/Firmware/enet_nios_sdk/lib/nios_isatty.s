
	.text
	.global isatty

; ---------------------------
; isatty
; Returns "true": we are hooked up
; to a tty.
isatty:
	JMP	%o7		; that wasn't so bad!
	MOVI	%o0,1

; end of file
