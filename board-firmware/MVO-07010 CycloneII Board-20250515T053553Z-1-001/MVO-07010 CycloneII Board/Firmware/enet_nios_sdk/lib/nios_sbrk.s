		.include "excalibur.s"


	.text
	.global _sbrk

; ---------------------------
; _sbrk
; Returns a new block of ram, and bumps
; the bottom-pointer up a bit.
;

_sbrk:
	SAVE	%sp,-23

	MOVIA	%l0,RAMLimit	; %l0 -> RAM Limit

	LD	%l1,[%l0]	; %l1 = RAM Limit
	LD	%l2,[%l0]	; %l2 = Old RAM Limit

	ADD	%l1,%i0		; Add requested new amount

	ADDI	%l1,3		; round low 2 bits upwards
	ANDNIP	%l1,3

	;
	; See if we're near top of RAM...
	;
	MOV	%l3,%sp
	SUBIP	%l3,256		; safety margin...
	CMP	%l3,%l1
	IFS	cc_nc		; stack > sbrk + block size?
	 BR	sbrk_stillRoom	;  then, good! plenty of room.

	;
	; If we get here, we're out of RAM.
	;
	MOVI	%i0,1		; (branch delay slot, too)
	NEG	%i0
	JMP	%i7
	RESTORE

sbrk_stillRoom:
	ST	[%l0],%l1

sbrk_done:
	MOV	%i0,%l2		; return previous break point
	JMP	%i7
	RESTORE

; end of file
