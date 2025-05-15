; file: timer_milliseconds.s
;
; ex:set tabstop=4:
; ex:set shiftwidth=4:
; ex:set expandtab:


	.include "excalibur.s"

	.text

;----------------------------------------
;         Name: nr_timer_milliseconds
;  Description: Routine to use Nios Timer
;               peripheral as a basic clock
;               It installs a timer handler the
;               first time you call it.
;        Input: none
;       Output: %o0 number of milliseconds since started, low
;               %o1 high (upper word or long)
; Side Effects: 
;    CWP Depth: 1
;
; These routines are hardcoded to
; a timer peripheral named "na_timer1."
;
; If you don't have a timer named timer1,
; you do not get the routine.
;
; nr_timer_milliseconds
;
	.global	nr_timer_milliseconds


.ifdef na_timer1

nr_timer_milliseconds:


	SAVE	%sp,-16
	MOVIA	%l3,nrMillisecondVars
	LDP	%l0,[%l3,0]
	IFRnz	%l0				; initialized?
	BR	pastInitialize
	NOP

	MOVIA	%l0,timerTrapHandler@h
	MOVIA	%l1,nasys_vector_table
	PFX	na_timer1_irq
	ST	[%l1],%l0		; install trap handler

	MOVIA	%l2,na_timer1

.ifdef __nios32__
	MOVIA	%l0,nasys_clock_freq_1000
	STP	[%l2,np_timerperiodl],%l0
	LSRI	%l0,16
	STP	[%l2,np_timerperiodh],%l0
.else
	PFX	%hi(nasys_clock_freq_1000)
	MOVI	%l0,%lo(nasys_clock_freq_1000)	; write low 16 bits of millisecond timeout
	STP	[%l2,np_timerperiodl],%l0
	PFX	%xhi(nasys_clock_freq_1000)
	MOVI	%l0,%xlo(nasys_clock_freq_1000)	; write high 16 bits of millisecond timeout
	STP	[%l2,np_timerperiodh],%l0
.endif

	MOVI	%l0,np_timercontrol_start_mask+np_timercontrol_cont_mask+np_timercontrol_ito_mask
						; interrupt enable + continuous + start
	STP	[%l2,np_timercontrol],%l0

	STP	[%l3,0],%l0			; write nonzero to the "yep, initialized" field

pastInitialize:
	LDP	%i0,[%l3,1]		; return high & low parts; on nios 32, just the low matters.
	LDP	%i1,[%l3,2]
	JMP	%i7
	RESTORE

	;
	; The system ISR
	;

timerTrapHandler:
	MOVI	%l0,0
	MOVIA	%l3,na_timer1
	NOP
	ST	[%l3],%l0		; any write clears the interrupt
	NOP

	MOVIA	%l3,nrMillisecondVars
	NOP
	LDP	%l0,[%l3,1]			; increment milliseconds low
	ADDI	%l0,1
	STP	[%l3,1],%l0

	IFRnz	%l0				; overflowed the low?
	 BR	timerTrapHandlerDone
	NOP

	LDP	%l0,[%l3,2]			; increment milliseconds high
	ADDI	%l0,1
	STP	[%l3,2],%l0
	NOP
timerTrapHandlerDone:
	TRET	%o7

	.comm	nrMillisecondVars,12,4

.endif     ; na_timer1 defined

; end of file


