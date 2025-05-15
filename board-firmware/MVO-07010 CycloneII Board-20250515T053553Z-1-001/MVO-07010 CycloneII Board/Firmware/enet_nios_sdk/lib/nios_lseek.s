	.text
	.global _lseek

; ---------------------------
; _lseek
; Returns error: serial port unseekable
; to a tty.
_lseek:
	MOVI	%o0,1
	JMP	%o7		; that wasn't so bad!
	NEG	%o0

; end of file
