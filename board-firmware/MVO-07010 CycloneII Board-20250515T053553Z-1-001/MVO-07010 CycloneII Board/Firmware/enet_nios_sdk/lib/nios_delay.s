



	.include "excalibur.s"
	.text

;----------------------------------------
;         Name: nr_delay
;  Description: Pause for milliseconds.
;        Input: %o0 number of milliseconds to pause
;       Output: none
; Side Effects: none
;    CWP Depth: 0
;


; nr_delay
	.global	nr_delay

nr_delay:

	MOV	%g0,%o0		; millisecond count
delayLoop1:
	MOVIA	%g1,nasys_clock_freq_1000
	LSRI	%g1,3
delayLoop2:
	IFRnz	%g1
	 BR	delayLoop2
	SUBI	%g1,1

	SUBI	%g0,1
	IFRnz	%g0
	 BR	delayLoop1
	NOP
	
	JMP	%o7
	NOP

; End of file.
