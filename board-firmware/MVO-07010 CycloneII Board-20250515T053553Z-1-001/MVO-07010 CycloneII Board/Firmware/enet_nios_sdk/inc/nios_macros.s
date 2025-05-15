
;------------------------------
; Macros I: Faux Instructions
;
; The following "faux instructions" are
; implemented here as macros:
;
; MOVIP register,constant		MOVI with optional PFX & MOVHI, or BGEN
; ADDIP register,constant		PFX and ADDI with optional PFX
; SUBIP register,constant		PFX and SUBI with optional PFX
; CMPIP register,constant		PFX and CMPI with optional PFX
;
; MOVI16 register,constant		PFX and MOVI
; MOVI32 register,constant		PFX, MOVI, PFX, and MOVHI
; MOVIA  register,constant		PFX and MOVHI on Nios32, and PFX and MOVI
;
; ANDIP register,constant		PFX and ANDI
; ANDNIP register,constant		PFX and ANDN
; ORIP register,constant		PFX and ORI
; XORIP register,constant		PFX and XORI
;
; _BSR address						MOVIP address to %g7, and CALL
; _BR address						MOVIP address to %g7, and JMP
;
; BEQ address						SKPS cc_nz and BR, has delay slot
; BNE address						SKPS cc_z and BR, has delay slot
; BLE address						SKPS cc_gt and BR, has delay slot
; BLT address						SKPS cc_ge and BR, has delay slot
; RESTRET							RESTORE and JMP %i7 
;
;-------------------------------
; Macros II: Printing
;
; These macros are guaranteed *not*
; to have branch delay slot after them.
;
; nm_printchar char
; nm_print "string"
; nm_println "string"			Follows it with a carriage return
; nm_printregister reg			For debugging, prints register name & value
;
;-------------------------------
; Macros III: Inline Debugging
;
; These macros print various information
; using large sections of expanded inline code.
; They each use either few or no registers.
; Thus, they may be safely used in interrupt handlers.
;
; nm_d_txchar char			print char to UART/JTAG, affects no registers
; nm_d_txregister char,char,register	prints the two characters, and the hex register value

; --------------------------------------

        
        ; |
        ; | _PFX becomes either nothing or a PFX or a PFXIO
        ; | depending on if nasys_has_cache, and the argument X.
        ; |
        ; | If no cache and X is zero, emit nothing,
        ; | If no cache and X, emit PFX,
        ; | If cache, emit PFXIO
        ; |

        .macro  _pfx X
        .if nasys_has_dcache
            pfxio \X
        .else
            .if \X
            pfx   \X
            .endif
        .endif
        .endm


		.macro	_pfx_op	OP,reg,val,pForce=0
		.if		(\pForce) || ((\val) > (31)) || ((\val) < (0))
		PFX		%hi(\val)
		.endif
		\OP		\reg,%lo(\val)
		.endm

		.macro	_bgen reg,val,bit
		.if ((\val)==(1<<\bit))
		BGEN	\reg,\bit
		.equ	_bgenBit,1
		.endif
		.endm

	;------------------------
	; MOVIP %reg,32-bit-value
		.macro	MOVIP reg,val
		; Methodically test every BGEN possibility...
		.equ	_bgenBit,0
.if 1
		_bgen \reg,\val,0
		_bgen \reg,\val,1
		_bgen \reg,\val,2
		_bgen \reg,\val,3
		_bgen \reg,\val,4
		_bgen \reg,\val,5
		_bgen \reg,\val,6
		_bgen \reg,\val,7
		_bgen \reg,\val,8
		_bgen \reg,\val,9
		_bgen \reg,\val,10
		_bgen \reg,\val,11
		_bgen \reg,\val,12
		_bgen \reg,\val,13
		_bgen \reg,\val,14
		_bgen \reg,\val,15
		_bgen \reg,\val,16
		_bgen \reg,\val,17
		_bgen \reg,\val,18
		_bgen \reg,\val,19
		_bgen \reg,\val,20
		_bgen \reg,\val,21
		_bgen \reg,\val,22
		_bgen \reg,\val,23
		_bgen \reg,\val,24
		_bgen \reg,\val,25
		_bgen \reg,\val,26
		_bgen \reg,\val,27
		_bgen \reg,\val,28
		_bgen \reg,\val,29
		_bgen \reg,\val,30
		_bgen \reg,\val,31

		; If no bgen fit...
.endif
		.if !_bgenBit
			.if ((\val) & 0xFFE0)
				PFX %hi(\val)
			.endif
			MOVI \reg,%lo(\val)
			.ifdef __nios32__
				.if ((\val) & 0xffff0000)
					.if ((\val) & 0xFFE00000)
						PFX %xhi(\val)
					.endif
					MOVHI \reg,%xlo(\val)
				.endif
			.endif
		.endif

		.endm

	; ADDIP %reg,16-bit-value
		.macro	ADDIP reg,val
		_pfx_op	ADDI,\reg,\val
		.endm

	; SUBIP %reg,16-bit-value
		.macro	SUBIP reg,val
		_pfx_op	SUBI,\reg,\val
		.endm

	; CMPIP %reg,16-bit-value
		.macro	CMPIP reg,val
		_pfx_op	CMPI,\reg,\val
		.endm

	; ANDIP %reg,16-bit-value
		.macro	ANDIP reg,val
		PFX		%hi(\val)
		AND		\reg,%lo(\val)
		.endm

	; ANDNIP %reg,16-bit-value
		.macro	ANDNIP reg,val
		PFX		%hi(\val)
		ANDN		\reg,%lo(\val)
		.endm

	; ORIP %reg,16-bit-value
		.macro	ORIP reg,val
		PFX		%hi(\val)
		OR			\reg,%lo(\val)
		.endm

	; XORIP %reg,16-bit-value
		.macro	XORIP reg,val
		PFX		%hi(\val)
		XOR		\reg,%lo(\val)
		.endm

	; BEQ addr
		.macro	BEQ addr
		IFS		cc_eq
		BR			\addr
		.endm

	; BNE addr
		.macro	BNE addr
		IFS		cc_ne
		BR			\addr
		.endm

	; BLE addr
		.macro	BLE addr
		SKPS		cc_gt
		BR			\addr
		.endm

	; BLT addr
		.macro	BLT addr
		SKPS		cc_ge
		BR			\addr
		.endm

		.macro	digitToChar reg
		ANDIP	\reg,0x000f
		CMPI	\reg,10
		SKPS	cc_lt
		ADDI	\reg,'A'-'0'-10
		PFX		%hi('0')
		ADDI	\reg,%lo('0')
		.endm

; PUSHRET == dec sp, and stash return addr
	.macro	PUSHRET
	SUBI		%sp,2
	ST			[%sp],%o7
	.endm
; POPRET == pop and jump
	.macro	POPRET
	LD			%o7,[%sp]
	JMP		%o7
	ADDI		%sp,2		; branch delay slot
	.endm

; RESTRET = restore & return
	.macro	RESTRET
	JMP		%i7
	RESTORE
	.endm

	;--------------------
	; MOVI16 %reg,Address
	;
	.macro	MOVI16	reg,val
	PFX	%hi(\val)
	MOVI	\reg,%lo(\val)
	.endm

	;--------------------
	; MOVI32 %reg,Address
	;
	.macro	MOVI32	reg,val
	PFX	%hi(\val)
	MOVI	\reg,%lo(\val)
	PFX	%xhi(\val)
	MOVHI	\reg,%xlo(\val)
	.endm

	;--------------------
	; MOVIA %reg,Address
	;
	.macro	MOVIA		reg,val
	.ifdef __nios32__
		MOVI32 \reg,\val
	.else
		MOVI16 \reg,\val
	.endif
	.endm

	;--------------------
	; _BR

	.macro _BR target,viaRegister=%g7
	MOVIA	\viaRegister,\target@h
	JMP	\viaRegister
	.endm

	;--------------------
	; _BSR

	.macro _BSR target,viaRegister=%g7
	MOVIA	\viaRegister,\target@h
	CALL	\viaRegister
	.endm


	;---------------------
	; nm_print "Your String Here"
	;
	.macro	nm_print	string

	BR		pastStringData\@
	NOP

stringData\@:
	.asciz	"\string"
	.align 1		; aligns by 2^n
pastStringData\@:
	MOVIA		%o0,stringData\@
.if __nios_jtag_stdio__
	_BSR		nr_jtag_txstring
.else
	_BSR		nr_uart_txstring
.endif
	NOP
	.endm

	.macro	nm_println string
	nm_print	"\string"
.if __nios_jtag_stdio__
	_BSR		nr_jtag_txcr
.else
	_BSR		nr_uart_txcr
.endif
	NOP
	.endm

	.macro	nm_printregister reg	; affects %g0 & %g1 & %g7, but thrashes the CWP a bit
	SAVE		%sp,-16
	nm_print	"\reg = "
	RESTORE
	MOV		%g0,\reg
	SAVE		%sp,-16
	MOV		%o0,%g0
.if __nios_jtag_stdio__
	_BSR		nr_jtag_txhex
	NOP
	_BSR		nr_jtag_txcr
	NOP
.else
	_BSR		nr_uart_txhex
	NOP
	_BSR		nr_uart_txcr
	NOP
.endif
	RESTORE
	.endm

	.macro	nm_printchar char
	MOVIP		%o0,\char
.if __nios_jtag_stdio__
	_BSR		nr_jtag_txchar
.else
	_BSR		nr_uart_txchar
.endif
	MOVI		%o1,0
	.endm

	.macro	nm_print2chars char1,char2
	MOVIP		%o0,(\char2<<8)+\char1
.if __nios_jtag_stdio__
	_BSR		nr_jtag_txchar
	NOP
	_BSR		nr_jtag_txchar
.else
	_BSR		nr_uart_txchar
	NOP
	_BSR		nr_uart_txchar
.endif
	LSRI		%o0,8
	.endm



; ---------------------------
; Completely inline UART sends
; Send the char, or %g7 if not there.
; Trashes %r0 and %g5 and %g6 and %g7...

	.macro	nm_txchar char=0

.if \char
	MOVIP	%g7,\char
.endif
	
.if __nios_jtag_stdio__

	;
	; Inline printing of a character %g7
	; out the jtag/OCI port
	;

	MOVIA	%g6,nasys_printf_uart+np_jtagtxdata

txCharLoop_jtag\@:
	LD	%g5,[%g6]
	EXT16d	%g5,%g6
	SKP0	%g5,np_jtagstatus_trdy_bit
	BR	txCharLoop_jtag\@
	NOP
	ORIP	%g7,np_jtagstatus_trdy_mask	; set the ready bit
	FILL16	%r0,%g7
	ST16D	[%g6],%r0			; put char into OCI JTAG register

.else
	;
	; Inline printing of a character %g7
	; out a nios uart
	;

	MOVIA	%g6,nasys_printf_uart

txCharLoop\@:
	_pfx np_uartstatus
	LD	%g5,[%g6]
	SKP1	%g5,np_uartstatus_trdy_bit
	BR	txCharLoop\@
	NOP
	_pfx np_uarttxdata
	ST	[%g6],%g7
.endif
	.endm


		.macro nm_txcr
		nm_txchar 13
		nm_txchar 10
		.endm

		.macro nm_txhexdigit,reg,shift
		MOV		%g7,\reg
		LSRI		%g7,\shift
		ANDIP		%g7,0x000f
		CMPI		%g7,10
		SKPS		cc_lt
		ADDIP		%g7,'A'-'0'-10
		ADDIP		%g7,'0'
		nm_txchar
		.endm

		.macro nm_txhex

	.ifdef __nios32__
		nm_txhexdigit %g0,28
		nm_txhexdigit %g0,24
		nm_txhexdigit %g0,20
		nm_txhexdigit %g0,16
	.endif

		nm_txhexdigit %g0,12
		nm_txhexdigit %g0,8
		nm_txhexdigit %g0,4
		nm_txhexdigit %g0,0
		.endm










; ----------------------
; The following macros are
; rather mighty. They expand
; to large inline code for
; printing various things to
; the serial port. They are
; useful for debugging
; trap handlers, where you
; can't just go and call
; nr_uart_TxChar and such, because,
; well, the CWP might be
; off limits!
;
; They do, however, presume
; that the stack is in good
; working order.


.macro nm_d_pushgregisters
 	SUBIP %sp,16+69				; oddball number so if we accidentally see it, it looks funny.
	STS	[%sp,16+0],%g0
	STS	[%sp,16+1],%g1
	STS	[%sp,16+2],%g2
	STS	[%sp,16+3],%g3
	STS	[%sp,16+4],%g4
	STS	[%sp,16+5],%g5
	STS	[%sp,16+6],%g6
	STS	[%sp,16+7],%g7
	.endm

.macro nm_d_popgregisters
	LDS	%g0,[%sp,16+0]
	LDS	%g1,[%sp,16+1]
	LDS	%g2,[%sp,16+2]
	LDS	%g3,[%sp,16+3]
	LDS	%g4,[%sp,16+4]
	LDS	%g5,[%sp,16+5]
	LDS	%g6,[%sp,16+6]
	LDS	%g7,[%sp,16+7]
	ADDIP	%sp,16+69				; must match the push
	.endm


.macro nm_d_txchar	c
	SUBI	%sp,16+8		; 32 or 16 bit, that's enough space
	STS	[%sp,16+0],%g6
	STS	[%sp,16+1],%g7
	nm_txchar \c
	LDS	%g6,[%sp,16+0]
	LDS	%g7,[%sp,16+1]
	ADDI	%sp,16+8
	.endm

.macro nm_d_txchar3 c1,c2,c3
 nm_d_txchar '<'
 nm_d_txchar \c1
 nm_d_txchar \c2
 nm_d_txchar \c3
 nm_d_txchar '>'
.endm

.macro nm_d_txregister r,n,reg
 nm_d_pushgregisters
 nm_txchar '('
 nm_txchar \r
 nm_txchar \n
 nm_txchar ':'
 MOV		%g0,\reg
 nm_txhex
 nm_txchar ')'
 nm_d_popgregisters
.endm

.macro nm_d_txreg r,n,reg
	nm_d_txregister \r,\n,\reg
.endm

; Do a delay loop, affects no registers.

.macro nm_d_delay d
	SUBI	%sp,16+4
	STS	[%sp,16+0],%g0
	MOVIP	%g0,\d
nm_d_delayloop\@:
	IFRnz	%g0
	 BR	nm_d_delayloop\@
	SUBI	%g0,1
	LDS	%g0,[%sp,16+0]
	ADDI	%sp,16+4
.endm

