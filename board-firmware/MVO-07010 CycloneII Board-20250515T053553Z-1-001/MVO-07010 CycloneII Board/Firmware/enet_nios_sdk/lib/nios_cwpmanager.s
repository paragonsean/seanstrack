

		.include "excalibur.s"
		.text

		.global nr_installcwpmanager
		;; .global CWPUnderflowTrapHandler
		;; .global CWPOverflowTrapHandler

		.equ	na_underflowtrap,1
		.equ	na_overflowtrap,2

		.equ	cwp_debugging,0

;--------------------------
; Install the underflow and
; overflow trap handlers.
; They rely upon vecBase being
; defined, and of course once
; they're running, the stack
; must exist and work.

nr_installcwpmanager:
	MOVIA	%g0,nasys_vector_table


	MOVIA	%g1,CWPUnderflowTrapHandler@h
	PFX	na_underflowtrap
	ST	[%g0],%g1

	MOVIA	%g1,CWPOverflowTrapHandler@h
	PFX	na_overflowtrap
	ST	[%g0],%g1

	PFX	9
	WRCTL	%g0		; enable traps
	JMP	%o7
	NOP


	.comm cwpUnderflowG,32,4	; this is storage for the %g's while we're
					; crawling up the stack and stashing registers.

CWPUnderflowTrapHandler:

; Current state:
; CWP=LO_LIMIT has just performed a SAVE, which completes
; normally except that now CWP=(LO_LIMIT-1), which is not allowed.
; So, we wind up here, and we shall make CWP=6 seem like
; CWP=(LO_LIMIT-1).
;
	; It's pretty roomy down here: we have our very own set
	; of %l's. But it just wouldn't be possible without
	; a little bit of fixed, temporary storage, namely,
	; the cwpUnderflowG temp space, for all the g registers.

	; As we march up the CWP frames, only the %g's can
	; be relied on, as they're fixed.

	; Save the %g registers.

	MOVIA	%l0,cwpUnderflowG
	STP	[%l0,0],%g0		; %g's are ours now.
	STP	[%l0,1],%g1
	STP	[%l0,2],%g2
	STP	[%l0,3],%g3
	STP	[%l0,4],%g4
	STP	[%l0,5],%g5
	STP	[%l0,6],%g6
	STP	[%l0,7],%g7

	; Figure out HI- and LO-limit...

	PFX	2
	RDCTL	%g0
	MOV	%g1,%g0
	LSLI	%g1,4
	ANDIP	%g1,0x01f0		; %g1 = CWP LO_LIMIT, in CTL 0 position
	MOV	%g2,%g0
	LSRI	%g2,1
	ANDIP	%g2,0x01f0		; %g2 = CWP HI_LIMIT, in CTL 0 position

	MOV	%g6,%o6			; %g6 & %g7 get the stack pointer & return address...
	MOV	%g7,%o7

	; We neeed LO_LIMIT-1's stack pointer, see.

	MOV	%g3,%sp			; %g3 -> (to be) saved %i and %l.
					; we'll need this, to put the LO_LIMIT's %o's into
					; the HI_LIMIT's %i's.
	;
	; %g3 now contains the LOWEST stack value!
	; as we march up the register windows, if we have any
	; thoughts of using the stack for, say, debugging
	; code, saving registers, or whatever, it's
	; important to use THIS value, not whatever value
	; happens to be represented in the register window!

	; Optimization note: we don't have to save
	; CWP=(LO_LIMIT-1)'s %l registers! They're not really in use yet...

cwpUnderflowLoop:

	; Store the %l's and %i's on the stack, in the
	; 16 words that every routine provides.
	; Store the %l's first, and the %o's at sp+8

	ST	[%sp],%l0	; use the stack to save a register before we proceed.
	MOV	%l0,%sp		; use %l0 now as our write-pointer
	MOV	%sp,%g3		; set the stack pointer to something we're allowed to use...

	STP	[%l0,1],%l1
	STP	[%l0,2],%l2
	STP	[%l0,3],%l3
	STP	[%l0,4],%l4
	STP	[%l0,5],%l5
	STP	[%l0,6],%l6
	STP	[%l0,7],%l7

	STP	[%l0,8],%i0
	STP	[%l0,9],%i1
	STP	[%l0,10],%i2
	STP	[%l0,11],%i3
	STP	[%l0,12],%i4
	STP	[%l0,13],%i5
	STP	[%l0,14],%i6
	STP	[%l0,15],%i7

	CMP	%g1,%g2		; compare to HI_LIMIT
	SKPS	cc_ls		; keep going, up to and inluding...
	BR	cwpUnderflowLoopDone
	NOP
	RDCTL	%g0
	ANDNIP	%g0,0x01f0
	OR	%g0,%g1
	WRCTL	%g0
        NOP   
	ADDI	%g1,16		; bump to next CWP (in CTL 0 position)
	_BR	cwpUnderflowLoop,%g0
	NOP

cwpUnderflowLoopDone:
	; Now, get LO_LIMIT's %o's into HI_LIMIT's %i's.
	; CTL 0 is at HI_LIMIT now, and %g3 still points to
	; the stashed registers. And hey, HI_LIMIT isn't
	; using it's %l's yet, so we will.
	MOV	%l0,%g3
	LDP	%i0,[%l0,8]
	LDP	%i1,[%l0,9]
	LDP	%i2,[%l0,10]
	LDP	%i3,[%l0,11]
	LDP	%i4,[%l0,12]
	LDP	%i5,[%l0,13]
	LDP	%i6,[%l0,14]
	LDP	%i7,[%l0,15]

	; Modify CTL 1, which is the
	; saved status register, so that
	; the only difference upon return
	; that instead of CWP = 0, we're
	; at CWP = HI_LIMIT...

	PFX	1
	RDCTL	%g0		; %g0 has status stored before this rude interrupt
	ANDNIP	%g0,0x01f0	; CTL 0 with old CWP masked out
	OR	%g0,%g2		; Insert HI_LIMIT
	PFX	1
	WRCTL	%g0		; Now, a TRET will land here.

	; Set up the stack pointer and return address
	; from CWP=(LO_LIMIT-1)...
	MOV	%o6,%g6
	MOV	%o7,%g7

	; And finally, restore the %g's...
	MOVIA		%l0,cwpUnderflowG	; we're at CWP=6, about to use CWP=6: %l is fair game
	LDP	%g0,[%l0,0]
	LDP	%g1,[%l0,1]
	LDP	%g2,[%l0,2]
	LDP	%g3,[%l0,3]
	LDP	%g4,[%l0,4]
	LDP	%g5,[%l0,5]
	LDP	%g6,[%l0,6]
	LDP	%g7,[%l0,7]

	TRET	%o7


CWPOverflowTrapHandler:

	; After RESTORE-ing too many times, we get
	; a trap that puts us right here.
	;
	; We get here with CWP=HI_LIMIT, and
	; ISTATUS CWP=HI_LIMIT+1, and
	; we should leave here with CWP=LO_LIMIT,
	; and lots of registers restored from how we
	; stashed em on the stack.
	;
	; Since %i6 was the stack pointer at
	; CWP=HI_LIMIT, it will now be %o6
	; here at CWP=HI_LIMIT+1.
	;
	; Up here at CWP=HI_LIMIT+1, we 
	; don't have any %l or %i registers.
	; The %l's alias to the %g's, in fact.

	;
	; Start by saving the globals, since
	; that's our only safe space...


	MOVIA	%l0,cwpUnderflowG	; They just did "RESTORE": they're done with %l0!
	STP	[%l0,0],%g0		; %g's are ours if we save them now.
	STP	[%l0,1],%g1
	STP	[%l0,2],%g2
	STP	[%l0,3],%g3
	STP	[%l0,4],%g4
	STP	[%l0,5],%g5
	STP	[%l0,6],%g6
	STP	[%l0,7],%g7

	; Figure out HI- and LO-limit...

	PFX		2
	RDCTL		%g0
	MOV		%g1,%g0
	LSLI		%g1,4			; %g1 in position to mask into CTL 0
	ANDIP		%g1,0x01f0		; %g1 = CWP LO_LIMIT, in CTL 0 position
	MOV		%g2,%g0
	LSRI		%g2,1
	ANDIP		%g2,0x01f0		; %g2 = CWP HI_LIMIT, in CTL 0 position

	MOV		%g3,%sp			; This is the lowest stack pointer...

	; Save the %i's from CWP=HI_LIMIT for CWP=(LO_LIMIT-1)'s %i's...
	MOV	%l0,%g3
	STP	[%l0,8],%i0
	STP	[%l0,9],%i1
	STP	[%l0,10],%i2
	STP	[%l0,11],%i3
	STP	[%l0,12],%i4
	STP	[%l0,13],%i5
	STP	[%l0,14],%i6
	STP	[%l0,15],%i7

	STP	[%l0,7],%o7		; NORMALLY this hold's an I register, but we dont
					; need those from a TRAP; we need the return address
					; from the trap, and we'll keep it down at CWP 0's %l7.
	STP	[%l0,6],%o6

	; Move to CWP=LO_LIMIT-1 and start climbing...
	RDCTL	%g0
	ANDNIP	%g0,0x01f0
        OR      %g0, %g1                ; add the present LO_LIMIT, in place
        SUBI    %g0, 0x0010             ; subtract 1 from CWP, in place
	WRCTL	%g0
	NOP

        ; load up the %i's at CWP=(LO_LIMIT-1) just like the %i's from CWP=6...
	MOV	%l0,%g3
	LDP	%i0,[%l0,8]
	LDP	%i1,[%l0,9]
	LDP	%i2,[%l0,10]
	LDP	%i3,[%l0,11]
	LDP	%i4,[%l0,12]
	LDP	%i5,[%l0,13]
	LDP	%i6,[%l0,14]
	LDP	%i7,[%l0,15]
	LDP	%o7,[%l0,7]
	LDP	%o6,[%l0,6]		; (also in %g3, actually...)

	; a RESTORE puts CWP=1 (LO_LIMIT) and the %o's ready for action.
	; This includes %sp, so CWP=1 can load up its %l's and %i's,
	; and so on.
	RESTORE

cwpOverflowLoop:
	; Restore %l's and %i's from where %sp->points.
	MOV	%l0,%sp
	MOV	%g4,%sp		; to restore at end

	MOV	%sp,%g3

	LDP	%i7,[%l0,15]
	LDP	%i6,[%l0,14]
	LDP	%i5,[%l0,13]
	LDP	%i4,[%l0,12]
	LDP	%i3,[%l0,11]
	LDP	%i2,[%l0,10]
	LDP	%i1,[%l0,9]
	LDP	%i0,[%l0,8]

	LDP	%l7,[%l0,7]
	LDP	%l6,[%l0,6]
	LDP	%l5,[%l0,5]
	LDP	%l4,[%l0,4]
	LDP	%l3,[%l0,3]
	LDP	%l2,[%l0,2]
	LDP	%l1,[%l0,1]
	LD	%l0,[%l0]

	MOV	%sp,%g4		; leave SP exactly as it was, of course.

	ADDI	%g1,16
	CMP	%g1,%g2
	SKPS	cc_le
	BR	cwpOverflowLoopDone
	NOP
	RESTORE
	_BR	cwpOverflowLoop,%g0
	NOP

cwpOverflowLoopDone:
	; Figure out LO-limit...
	PFX		2
	RDCTL		%g0
	MOV		%g1,%g0
	LSLI		%g1,4			; %g1 in position to mask into CTL 0
	ANDIP		%g1,0x01f0		; %g1 = CWP LO_LIMIT, in CTL 0 position
	; Set CWP = (LO_LIMIT-1), and ISTATUS's CWP at LO_LIMIT, 
        ; so that TRET does its thing.
	RDCTL	%g0
	ANDNIP	%g0,0x01f0      
        OR      %g0, %g1                ; add the present LO_LIMIT
        SUBI    %g0, 0x0010             ; subtract 1 from CWP, in place
	WRCTL	%g0
        NOP

	; modify CTL 1 so that TRET takes us back
	; to CWP=LO_LIMIT next
	PFX	1		; read saved status
	RDCTL	%g0
	ANDNIP	%g0,0x01f0
;	ORIP	%g0,0x0010	; presume LO_LIMIT = 1...
        OR      %g0, %g1        ; add the present LO_LIMIT
	PFX	1
	WRCTL	%g0

	; restore the %g's
	MOVIA		%g0,cwpUnderflowG
	PFX	1
	LD	%g1,[%g0]
	PFX	2
	LD	%g2,[%g0]
	PFX	3
	LD	%g3,[%g0]
	PFX	4
	LD	%g4,[%g0]
	PFX	5
	LD	%g5,[%g0]
	PFX	6
	LD	%g6,[%g0]
	PFX	7
	LD	%g7,[%g0]
	LD	%g0,[%g0]

	TRET	%o7

;end of file
