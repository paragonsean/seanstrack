
		.include "excalibur.s"


	.text
	.global _exit
	.global exit

; --------------------------
; exit: just pretend we finished from "main".

exit:
_exit:
	_BR	_past_main
	RESTORE		; (delay slot) well, if the did exit() from main, this'll be right again...

; end of file
