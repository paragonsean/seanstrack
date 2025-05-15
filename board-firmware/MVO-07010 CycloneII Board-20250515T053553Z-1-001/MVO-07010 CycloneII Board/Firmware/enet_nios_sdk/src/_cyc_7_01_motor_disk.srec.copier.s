; file: <file_name>
;
; This is a copy routine to copy code
; from flash to RAM, and then run it.
;
; CPU Architecture: nios_32
;
; Flash source address: 524288
; RAM destination address: 33619968
; Size to copy: 120480

	.include "nios.s"
	.global _start

	.equ	flash_address,524288
	.equ	ram_address,33619968
	.equ	copy_size,120480

	.org	0
	MOVIA	%r0,(flash_address+0x10)/2
	JMP	%r0
	NOP

	.org	0x0c
	.byte   'N','i','o','s'

	.org	0x10
_start:

.ifdef na_seven_seg_pio
	MOVIP	%L0,na_seven_seg_pio
	MOVIP	%L1,0xb7b7
	ST	[%L0],%L1
.endif

        ; %L0 will be the source
        ; %L1 will be the dest
        ; %l2 will be the size

        MOVIP   %L0,flash_address+256
        MOVIP   %L1,ram_address
        MOV     %L3,%L1
        MOVIP   %L2,copy_size

loop:
        LD      %L5,[%L0]
        EXT8d   %L5,%L0
        FILL8   %r0,%L5
        ST8d    [%L1],%r0
        ADDI    %L0,1
        SUBI    %L2,1
        IFRnz   %L2
         BR     loop
        ADDI    %L1,1   ; (delay slot)

.ifdef na_seven_seg_pio
	MOVIP	%L0,na_seven_seg_pio
	MOVIP	%L1,0xeded
	ST	[%L0],%L1
.endif

        LSRI    %L3,1
        JMP     %L3
        NOP

;end
