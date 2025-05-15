

# This file gets linked first by nios-build, so that
# if you type "G40100" (or whatever is the code's base
# address) it will find the correct actual starting
# point of the code.
#
# It can only be linked explicitly; it has no labels
# so it won't be found in an archive or anything.
#
# ex: set tabstop=8:
#

	.include "excalibur.s"

	.section .text.prefix		; linker script knows to put this first

	.global nr_jumptostart		; Global so we can see it in dumps.

nr_jumptostart:
	PFX	%hi(_start@h)		; 0x00
	MOVI	%g0,%lo(_start@h)	; 0x02
.ifdef __nios32__
	PFX	%xhi(_start@h)		; 0x04
	MOVHI	%g0,%xlo(_start@h)	; 0x06
.endif
	JMP	%g0			; 0x08 / 0x04 on Nios 16
	NOP				; 0x0a / 0x06 on Nios 16
	.byte	'N','i','o','s'		; 0x0c / 0x08 on Nios 16 Signature, for ROM use.

; End of file
