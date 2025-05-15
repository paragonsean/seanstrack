; file: flash_copy.s

.include "nios.s"
.global _start

_start:
	MOVIP		%10,0x140100		; Flash (source) starting address
	;MOVIP		%11, 0x180000		; Flash (source) ending address
	;MOVIP		%12, na_SRAM1_base	; SRAM (destination) address start location
						; which is defined in nios_macros.s


flash_copier_loop:
	LD      %g0,[%10]			; Get current word
	ADDI	%10,#4			; Bump source to next word
	ST      [%12],%g0			; Store word
	ADDI    %12,#4			; Bump destination to next word
	CMP     %10,%11			; Are we there yet?
	IFS     cc_ne
	BR      flash_copier_loop
	NOP

; Jump to program stored at nasys_program_mem, which is defined in nios_map.s.  The
; @h divides by 2 so that the address is a valid one for a subroutine.

	;MOVIA	%g0, nasys_program_mem@h
	JMP %g0
	NOP

; End of file

