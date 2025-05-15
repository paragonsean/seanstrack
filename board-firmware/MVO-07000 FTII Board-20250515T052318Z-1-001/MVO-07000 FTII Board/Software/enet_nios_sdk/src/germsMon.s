; --------------------------
; File: germsMon.s
;
; Contents: yet another Nios monitor,
;           this one with S-Record, I-Hex
;           records, and Flash Programming
;           for the Nios development board.
;
;
; GermsMon is build-time configurable to
; support various Nios configurations.
; The following variables control
; how it's built:
;
;	GM_UARTBase -- base of the uart for output, presumed
;	               to be Nios compatible registers
;
;	GM_FlashBase,
;	GM_FlashTop --
;			The flash to support, unless FlashTop is zero,
;			which means "no flash". The flash is presumed
;			to be AMD 29LV800, as on the Nios reference design.
;			Other AMD flashes might be compatible.
;
;	GM_FlashExec --
;			The address in flash to check for the Nios
;			signature "Ni". Reference design uses
;			0x14000c. If the signature is found, execution
;			is transferred to GM_FlashExec - 0x0c. If
;			GM_FlashExec is zero, the feature is disabled.
;	GM_PIO --
;			The pio to check bit zero of before executing
;			from flash.
;
;	GM_RAMBase,
;	GM_RAMTop --
;			The range of 32-bit wide RAM to support.
;			We assume that all other memory is
;			16 bit wide. We also assume that GM_RAMTop
;			is where %sp should be set before running
;			a program.
;	GM_VecBase --
;			Interrupt vector table base
;	
;	GM_Clear1Base,GM_Clear1Register,
;	GM_Clear2Base,GM_Clear2Register,
;	GM_Clear3Base,GM_Clear3Register --
;			Up to three addresses which are cleared
;			when GermsMon starts up. These can be
;			used to disable interrupts on peripherals,
;			and such.
;
; To set a variable, include the directive 
;
;	--defsym GM_whatever=value
;
; on the assembler's command line. With nios-build, include
; the directive
;
;	-as "--defsym GM_whatever=value"
;
;

	.include "nios_map.s"
	.include "nios_macros.s"
	.include "nios_peripherals.s"



	.ifndef GM_UARTBase
	.equ GM_UARTBase,na_printf_uart
	.endif

	.ifndef GM_FlashBase
	.equ GM_FlashBase,na_ext_flash
	.endif

	.ifndef GM_FlashTop
	.equ GM_FlashTop,na_ext_flash_end
	.endif

	.ifndef GM_FlashExec
	.equ GM_FlashExec,GM_FlashBase+0x4000C
	.endif

	.ifndef GM_PIO
	.equ GM_PIO,na_button_pio
	.endif

	.ifndef GM_RAMBase
	.equ GM_RAMBase,na_ext_ram
	.endif

	.ifndef GM_RAMTop
	.equ GM_RAMTop,na_ext_ram_end
	.endif

	.ifndef GM_VecBase
	.equ GM_VecBase,na_vecbase
	.endif





;
; Mostly, this is one big goto-tangle.
;
; No RAM used
;
; Global Register Usage:
;
; %l0: UART Base Address
; %g5: the value 0x0000000F
; %g6: the value 0x00000003	for masking addresses
; %g7: residual byte for writing to Flash in words

; %g0,%g1: Any routine's scratch
; %l1-%l2: Any routine's scratch
; %o5: stashed return address
;
; %l3: Upper bytes (or segment) for I-Hex records
;
; %l4: Force-to address for S-Records/I-Hex records
; This gets set by an F command
;
; %l5: Address to read next from <CR>
; %i3: How many words to display on CR
;
; --------------
; During S-Record read
;    %i0 record type
;    %i1 record length in bytes
;    %i2 address we're reading bytes into
;
;
; -------------------------------
; Commands:
; G<address>               Go at address
; E<address>               Erase flash at address
; R<address>-<address>     Relocate S-Records from-to
; M<address>               Display at address
; M<address>-<address>     Display range
; M<address>:val val...    Write at attress
; M<address>-<address>     Display range
; S...                     S-record, no error checking
; :...                     Intel-Hex record, no error checking




	.text
	.global _start
_start:

	;
	; Clear out the vector table
	;
	MOVIP	%l6,GM_VecBase
	BGEN	%l5,8
	ADD	%l5,%l6		; %l5 = %l6+256
	BSR	FillRAM
	MOVI	%o1,0

	;
	; Reset UART, SwitchPIO, and Timers
	;
	MOVIP	%l0,GM_UARTBase			; we'll keep this...
	MOVI	%g0,0
	STP	[%l0,np_uartcontrol],%g0	; control all 0's: no irq's

	;
	; Clear some registers (presumably
	; to disable interrupts)
	;

	.ifdef GM_Clear1Base
	MOVIP	%l1,GM_Clear1Base
	STP	[%l1,GM_Clear1Register],%g0
	.endif

	.ifdef GM_Clear2Base
	MOVIP	%l1,GM_Clear2Base
	STP	[%l1,GM_Clear2Register],%g0
	.endif

	.ifdef GM_Clear3Base
	MOVIP	%l1,GM_Clear3Base
	STP	[%l1,GM_Clear3Register],%g0
	.endif

	.ifdef GM_PIO
	MOVIP	%l1,GM_PIO
	STP	[%l1,np_piointerruptmask],%g0
	.endif

	;
	; Set CWP to top, and IRQ to bottom
	;
	PFX	2
	RDCTL	%g0
	LSRI	%g0,1				; HI_LIMIT in correct bit position
						; Also, the topmost bit IS zero now, after LSR
	ORIP	%g0,0x7e0f			; I_PRI set to 63, NZVC all true (why not)
	WRCTL	%g0

	;
	; Set the stack pointer
	;
	MOVIP	%sp,GM_RAMTop

	;
	; Look for Signature in flash, & jump there.
	;
	.if GM_PIO
	LDP	%g0,[%l1,np_piodata]	; Unless the button is pressed
	IF0	%g0,0
	 BR	postFlash_start
	.endif

	.if GM_FlashTop
	MOVIP	%l2,GM_FlashExec	; 2nd quadrant of four, in flash, + 0xC
	LD	%g0,[%l2]
	CMPIP	%g0,'N'+(256*'i')
	IFS	cc_ne
	 BR	postFlash_start
	.endif

	SUBI	%l2,0x000c
	LSRI	%l2,1
	CALL	%l2			; (if it wants to return, no problem!)
postFlash_start:
	NOP				; clears out possible delay-slot PFX's

	;
	; Registers which need certain
	; values for the rest of the monitor
	; to work.
	; (Other registers are used as
	; needed, although their reservations
	; for certain uses must be respected
	; while running.)
	
	MOVIP	%l0,GM_UARTBase
	MOVI	%g5,0x0f
	MOVI	%g6,0x03

	BSR	PutHash		; but up here, so delay slot is used.
	; Initial safe values

	MOVI	%l3,0
	MOVI	%l4,0
	MOVI	%l5,0		; # address to show on <CR>
	MOVIP	%i3,32		; # words to show on <CR>


	.ifndef __timeStamp__
	.equ __timeStamp__,0
	.endif

	MOVIP	%o1,__timeStamp__
	BSR	PutHex
	NOP

	PFX	6
	RDCTL	%o1
	BSR	PutHex
	NOP
	BSR	PutChar
	MOVI	%o0,13

ReceiveCommand:
	MOVIP	%o0,'+'
	BSR	PutChar
	NOP

	BSR	GetChar
	NOP

	CMPIP	%o0,0x0060		; character from [60,7f]
	IFS	cc_ge				; mapped down to [40,5f]
	 ANDNIP	%o0,0x0020

	SUBIP	%o0,13
	IFRz	%o0
	BR	doCR

		
	SUBIP	%o0,'#'-13		; '#' (hash) is for comments: ignore to EOL
	IFRz	%o0
	BR	happyWaitForEOL

	SUBIP	%o0,':'-'#'
	IFRz	%o0
	BR	doColon

	SUBIP	%o0,'E'-':'
	IFRz	%o0
	BR	doE

	SUBIP	%o0,'G'-'E'
	IFRz	%o0
	BR	doG

	SUBIP	%o0,'M'-'G'
	IFRz	%o0
	BR	doM

	SUBIP	%o0,'R'-'M'
	IFRz	%o0
	BR	doR

	SUBIP	%o0,'S'-'R'
	IFRz	%o0
	BR	doS

	NOP

errorWaitForEOL:
	BSR	WaitForEOL
	NOP

	MOVIP	%o0,'?'			; Unknown Command
	BSR	PutChar
	NOP
	BSR	PutChar
	MOVI	%o0,13

	BR		ReceiveCommand
	NOP

doColon:
	; First 2 chars: record length, for data only
	BSR	GetHexBytes
	MOVI	%o3,1			; (delay slot) Fetch record length
	MOV	%i1,%o1			; record length stashed
	;
	; Next 4 chars: address offset
	;
	BSR	GetHexBytes
	MOVI	%o3,2			; (delay slot) we want 2 byte/4 chars of hex)
	MOV	%i2,%l3			; Get most recent upper-bytes address
	OR	%i2,%o1			; construct full address into %i2

	; Get record type
	;
	BSR	GetHexBytes
	MOVI	%o3,1			; (delay slot) 1 byte/2 chars for record type

	IFRz	%o1
	BR	doSReadData		; same code as reading S-Record data
	CMPI	%o1,4			; I-Hex 4 sets upper bits of destination address
	IFS	cc_ne
	 BR	doColonNot4
	NOP

	BSR	GetHexBytes
	MOVI	%o3,2			; (delay slot) upper 16 bits is 2 bytes worth

	LSLI	%o1,16
	MOV	%l3,%o1			; and save the upper bits there...

doColonNot4:
	BR	happyWaitForEOL
	NOP

;------------------------
; M Command
; M<addr>             show 16 bytes
; M<addr>-<addr>      show range
; M<addr>:val val val write values to addr
; M<addr>-<addr>:val  fill range
;
doM:
	MOVIP	%o2,':'
	BSR	GetHexUntil
	NOP

	;
	; Write operation?
	;

	CMPIP	%o0,':'
	IFS	cc_eq
	 BR	doMWrite
	NOP
	
	;
	; If showing range, compute it...
	;
	IFRnz	%o4
	 BR	doMShowRange
	NOP

	;
	; Show %i3 words... bytes
	;
	MOV	%l5,%o1
	BR	doCR

	;
	; Show Range
	; Show from %o4 to %o1, inclusive
	;
doMShowRange:
	MOV	%o3,%o1	
	SUB	%o3,%o4
	LSRI	%o3,1			; o3 = # words
	MOV	%i3,%o3			; if they hit return, they get this many, again
	MOV	%l5,%o4			; use %l5 to walk the addresses

doCR:
	MOV	%o3,%i3			; <CR> shows %i3 words from %l5

doMShowRangeLine:
	;
	; Come here with %o3 = #words to show, %l5 = address to show
	;

	MOVI	%l6,8			; countdown for this line

	; Print #addr:
	BSR	PutHash			; (watch out for delay slot)
	;
	; print the address
	;
.ifdef __nios32__
	; nios32? Show upper 16 bits of address first.
	MOV	%o1,%l5
	LSRI	%o1,16
	SKPRz	%o1
	BSR	PutHex
	NOP
.endif
	MOV	%o1,%l5
	BSR	PutHex
	NOP
	MOVIP	%o0,':'
	BSR	PutChar
	NOP
	BSR	PutChar
	BGEN	%o0,5			; (delay slot) hex 0x20 = space

doMShowRangeLoop:
	LD	%o1,[%l5]
.ifdef __nios32__
	;
	; Handle RAM slightly special: it's 32 bits
	; wide, so we must do an EXT16d to get the
	; word we want.
	;
	BSR		InRAMRange
	MOV		%i2,%l5		; (delay slot) Argument to InRAMRange
	IFS		cc_eq		; Yes, RAM?
	 EXT16d	%o1,%l5
.endif

	BSR	PutHex
	NOP
	MOVIP	%o0,' '
	BSR	PutChar

	;
	; Bump address and decrement to-show count
	;
	ADDI	%l5,2
	SUBI	%o3,1
	IFS	cc_le
	 BR	doneMCR

	; See if it's time for a newline
	SUBI	%l6,1
	IFRnz	%l6
	 BR	doMShowRangeLoop
	NOP

	BSR	PutChar
	MOVI	%o0,13			; (delay slot)
	BR	doMShowRangeLine
	NOP
doneMCR:
	BSR	PutChar
	MOVI	%o0,13			; (delay slot)
	BR	ReceiveCommand
	NOP
	
	
doMWrite:
	IFRnz	%o4
	 BR	doMFill
	MOV	%l5,%o1			; (delay slot, sometimes) start for next <CR> show

	;
	; Write space-separated values until CR
	;
	MOV	%i2,%o1
doMWriteLoop:
	MOVIP	%o2,' '
	BSR	GetHexUntil
	NOP
	MOV	%l7,%o0			; save the break-character...
	; Send in two bytes to the StashByte routine
	; so that it handles writing to flash

	BSR	StashByte
	MOV	%l6,%o1			; reserve high (later) byte
	LSRI	%l6,8
	MOV	%o1,%l6			; send the high byte
	BSR	StashByte
	ADDI	%i2,1			; (delay slot) to the next address

	CMPI	%l7,13			; %l7 has break char from GetHex
	IFS	cc_ne
	 BR	doMWriteLoop
	ADDI	%i2,1			; (delay slot) increment address
	BR	ReceiveCommand
	NOP

doMFill:
	MOV	%l6,%o4			; preserve low end of range
	BSR	GetHex
	MOV	%l5,%o1			; (delay slot) preserve end of range
	BSR	FillRAM
	MOV	%o3,%l6			; preserve for <CR> later
	BR	ReceiveCommand
	MOV	%l5,%o3			; restore for <CR> later to show from start of range

;--------------
; FillRAM
;
; %l6 = start, %l5 = end, %o1 = value
;

FillRAM:
.ifdef __nios32__
	FILL16	%r0,%o1			; stores done as all words
.endif
fillRAMLoop:
.ifdef __nios32__
	ST16d	[%l6],%r0
.else
	ST	[%l6],%o1
.endif
	ADDI	%l6,2			; go by twos in case Nios 16 (or funny bus width)
	CMP	%l6,%l5
	IFS	cc_ne
	 BR	fillRAMLoop
	NOP
	JMP	%o7
	NOP

; -------------------------
; S-Record handlin'
;
doS:
	BSR	GetChar			; Fetch the record type
	NOP
	MOV	%i0,%o0			; record type stashed

	BSR	GetHexBytes
	MOVI	%o3,1				; (delay slot) Fetch record length
	MOV	%i1,%o1			; record length (including address) stashed
	SUBI	%i1,1				; decrement to ignore the checksum @ the end

	AND	%i0,%g5			; mask of low bits of presumed ascii digit
	IFS	cc_eq				; zero?
	 BR	ignoreS			; that's a kind we ignore
	CMPI	%i0,4				; data record?
	IFS	cc_lt
	BR		doS123			; yup! go do data record
	NOP
	SUBI	%i0,7				; 789->012
	IFS	cc_mi				; was a 45 or 6...
	 BR	ignoreS
	CMPI	%i0,3
	IFS	cc_lt
	BR	doS789
	NOP
ignoreS:
happyWaitForEOL:		; Ignores until the end of line, then begins anew.
	BSR	WaitForEOL
	NOP
	BR	ReceiveCommand
	NOP

doS123:
	ADDI	%i0,1				; got here with %i0 = 12 or 3, change to addr bytes
	SUB	%i1,%i0			; %i1 = number of data bytes to absorb, now
	BSR	GetHexBytes
	MOV	%o3,%i0			; (delay slot) count of bytes to get
	MOV	%i2,%o1			; %i2 = address to read into

; -----------------
; Come here with %i2 = address to read bytes into
;                %i1 = number of bytes to read
;                %l4 = forced offset (relocation) to apply to all addresses (or zero)
doSReadData:
	ADD	%i2,%l4			; Override the start address with %l4 (force address)
doSReadData1:
	BSR	GetHexBytes		; Read a byte of data
	MOVI	%o3,1			; (delay slot) bytecount = 1

	BSR	StashByte		; puts %o1 into [%i2], and does FLASH stuff if needed

	SUBI	%i1,1			; (delay slot, but OK)
	IFRnz	%i1			; zero bytes remaining?
	BR		doSReadData1	; no: loop back around
	ADDI	%i2,1			; (delay slot) bump to next byte in any case

	BR		ignoreS		; ignore the checksum, and continue onward
	NOP

doS789:
	NEG	%i0		; got here with %i0=012 for 789
	ADDI	%i0,4		; now 987->234, byte count for GO address
	BSR	GetHexBytes
	MOV	%o3,%i0		; (delay slot) bytes of address to read
	BSR	WaitForEOL	; don't do anything until the line is finished
	NOP
	BR	Go		; %o1 is ready...
	NOP

doG:
	BSR	GetHex
	NOP
Go:
	LSRI	%o1,1		; divide in half for JMP action
	CALL	%o1
	NOP
	BR	postFlash_start
	NOP

doE:
	BSR	GetHex
	NOP
	ANDN	%o1,%g6	; mask off low 2 bits if address

.if GM_FlashTop
	BSR	InFlashRange
	MOV	%i2,%o1		; (delay slot, put address to check in %i2) see if it's flash
	IFS	cc_ne		; if not, just skip
	 BR	ReceiveCommand
	NOP

	;
	; Begin strange Flash-Erase sequence
	;

	BSR	flash555aa
	NOP
	BSR	flash2AA55
	NOP
	MOVIP	%g0,0x80
	BSR	flash555g0
	NOP
	BSR	flash555aa
	NOP
	BSR	flash2AA55
	NOP

; And finally tell the Flash what sector to erase...

	MOVIP	%g0,0x30
	ST	[%i2],%g0

; Loop until the data reads as 0xFFFF	(and we dont know whats in the high word)
	BGEN	%g2,25			; watchdog timeout, an excessive number
flashEraseLoop:
	IFRz	%g2
	 BR	ReceiveCommand
	SUBI	%g2,1

	LD	%g0,[%i2]
.ifdef __nios32__
	SEXT16	%g0
.endif
	ADDI	%g0,1				; IF done, %g0 == 0 now
	IFRnz	%g0
	 BR	flashEraseLoop
	NOP					; safety first

	;
	; Now, count to a million, just to be terribly sure.
	;
	BGEN	%g0,20				; m32 only, eh.
flashErasePostLoop:
	IFRnz	%g0
	 BR	flashErasePostLoop
	SUBI	%g0,#1				; (delay slot)

	BR		ReceiveCommand
	NOP

.endif  ; GM_FlashTop

; -------------------------
; doR: Set Relocate
; usage:
;          +r10800   <--- add 0x10800 to addresses in s-records/i-hex
;          +r40000-180000   <--- add 0x140000 to addresses in s-records/i-hex
doR:
	BSR	GetHex
	NOP
	SUB	%o1,%o4		; offset end address - start address (or just end)
	BR	ReceiveCommand
	MOV	%l4,%o1		; (delay slot)


; -----------------------
; GetChar and PutChar
; both work on %o0
; GetChar always echoes

getCharLFEcho:			; front end loopover *just* for echoing those ignored Linefeeds.
	LDP	%g0,[%l0,np_uartstatus]	; status register
	IF0	%g0,6
	 BR	getCharLFEcho
	NOP
	STP	[%l0,1],%o0		; (delay slot) send %o0 out

GetChar:
	LDP	%g0,[%l0,np_uartstatus]
	IF0	%g0,np_uartstatus_rrdy_bit	; bit 7: RRdy
	 BR	GetChar
	NOP
	LDP	%o0,[%l0,np_uartrxdata]
	CMPI	%o0,0x0A		; Ignore LF's
	SKPS	cc_ne
	BR	getCharLFEcho		; (echo the LF, but don't return it)
	CMPI	%o0,0x1B		; <ESC> restarts monitor: the only line editing.
	IFS	cc_eq
	 BR	postFlash_start
	NOP
	; and fall into PutChar for echo

.ifdef __supportGDB__
	;
	; Unless we're supporting GDB! in which
	; case, a $ immediately yanks us over
	; to the echo-less GDB parser!
	;
	; Once we get a $ and start doing GDB commands,
	; we stay in GDB mode.
	;

	CMPIP	%o0,'$'
	IFS	cc_eq
	 BR	doGDBCommand
.endif

PutChar:
	LDP	%g0,[%l0,np_uartstatus]	; status register
	SKP1	%g0,6				; bit 6: TRdy
	BR	PutChar
	NOP
	JMP	%o7
	STP	[%l0,1],%o0			; (delay slot) send %o0 out

PutHash:
	MOVIP	%o0,'#'
	BR	PutChar				; (watch out for delay slot below...)

WaitForEOL:
	MOV	%o5,%o7
waitForEOLLoop:
	BSR	GetChar
	NOP
	CMPI	%o0,0x0D
	SKPS	cc_eq
	BR	waitForEOLLoop
	NOP
	JMP	%o5
	NOP

; --------------
; PutHex %o1
;
; Prints four characters (16 bit value)
;
; Uses %o2 for shift amount
; Uses %o5 as alternate return address
;

PutHex:									; value %o1
	MOV	%o5,%o7
	MOVI	%o2,12
putHexLoop:
	MOV	%o0,%o1
	LSR	%o0,%o2
	AND	%o0,%g5
	CMPI	%o0,9
	SKPS	cc_le
	ADDI	%o0,'A'-'0'-10
	ADDIP	%o0,'0'
	BSR	PutChar
	NOP
	SKPRz	%o2
	BR		putHexLoop
	SUBI	%o2,4					; (delay slot) decrement shift amount
	JMP	%o5
	NOP

; -----------------------
; GetHex
;
; In: %o3 - number of characters to read (GetHexBytes only)
;     %o2 - character to break on, in addition to CR
;
; Out: %o1 - value read
;      %o0 - last character typed (CR or %o2 break char)
;
;
; Gets hex value until CR
; returns value in %o1
; A hyphen-separated pair returns
; the 2nd on %o1, and the 1st in %o4
;
; GetHexUntil will scan hex chars until either a CR
; or the character in %o2 (still with a hyphen moving
; stuff to %o4).

GetHexUntil:
	BR		getHexEntry1	; dive in with user's %o2 and 31 char limit
	MOVI	%o3,31

GetHex:
	MOVI	%o3,31		; maximum character count
GetHexBytes:			; entry point to use %o3 to signify byte count
	MOVI	%o2,13		; default return-break char
getHexEntry1:
	ADD	%o3,%o3		; double for chars
	MOV	%o5,%o7		; return address...
	MOVI	%o1,0		; result
getNextHex:
	MOV	%o4,%o1		; come here after a hypen, too
	MOVI	%o1,0		; result'

getHexLoop:
	BSR	GetChar
	NOP

	CMPIP	%o0,'-'		; hypen separated pair...
	IFS	cc_eq
	 BR	getNextHex

	CMP	%o0,%o2		; user-passed return character?
	IFS	cc_eq		; go home
	 JMP	%o5		; exit loop from here
	CMPI	%o0,13		; carriage return?
	IFS	cc_eq		; go home
	 JMP	%o5		; exit loop from here
	SUBI	%o3,1		; decrement char count
	SKP0	%o0,6		; bit 6 indicates alpha char
	ADDI	%o0,9		; if so, move up to 10-15
	AND	%o0,%g5		; and with 0x0f.
	LSLI	%o1,4
	SKPRz	%o3		; was this the last character?
	BR	getHexLoop	; loop back if not
	ADD	%o1,%o0		; (delay slot) last or not, add in the last nibble
	JMP	%o5		; return
	NOP

;--------------------
; Return Z flag set if %i2 is in flash range.
; Trash %l1.

	.if GM_FlashTop		; nonzero FlashTop means we have some flash
InFlashRange:

	.if ((GM_FlashBase==0x100000)&&(GM_FlashTop==0x200000))
	; Clever trick because flash range is [0x100000,0x200000)
	MOV	%l1,%i2
	LSRI	%l1,20
	JMP	%o7
	CMPI	%l1,1
	.else
	MOVI	%l1,1
	MOVIP	%r0,GM_FlashBase
	CMP	%i2,%r0
	IFS	cc_lt
	 MOVI	%l1,0
	MOVIP	%r0,GM_FlashTop
	CMP	%i2,%r0
	IFS	cc_ge
	 MOVI	%l1,0
	JMP	%o7
	CMPI	%l1,1
	.endif

	.endif

;--------------------
; Return Z flag set if %i2 is in RAM range.
; Trash %l1 and %g0.
; RAM range is [0x40000,0x80000).
; (hardcoded hard coded)

InRAMRange:
	.if (GM_RAMBase==0x40000)&&(GM_RAMTop==0x80000)
	MOV	%l1,%i2
	LSRI	%l1,18
	JMP	%o7
	CMPI	%l1,1
	.else
	MOVI	%l1,1
	MOVIP	%g0,GM_RAMBase
	CMP	%i2,%g0
	IFS	cc_lt
	 MOVI	%l1,0
	MOVIP	%g0,GM_RAMTop
	CMP	%i2,%g0
	IFS	cc_ge
	 MOVI	%l1,0
	JMP	%o7
	CMPI	%l1,1
	.endif




;--------------------
; Stash byte routine shared
; by S-records and 'W' command
;
; Store byte %o1 into address %i2
; and don't corrupt anything important!
;
StashByte:
	MOV	%o4,%o7		; stash return address, deeper than usual!

	.if GM_FlashTop
	BSR	InFlashRange
	NOP
	IFS	cc_ne
	 BR	stashByteRAM
	NOP
	; --------------
	; We're storing to flash here...
	IF1	%i2,0		; odd address? then do the FlashWrite routine
	 BR	FlashWrite
	EXT8s	%o1,0		; (delay slot) always clear out the high bits

	JMP	%o4		; return
	MOV	%g7,%o1		; (delay slot) save the low byte

FlashWrite:
	LSLI	%o1,8
	OR	%o1,%g7		; %o1 has word to write, and %i2 is address+1

	; -----------
	; Begin bizarre Write Word flash sequence

	BSR	flash555aa
	NOP
	BSR	flash2AA55
	NOP
	MOVIP	%g0,0xa0
	BSR	flash555g0
	NOP

	;
	; And the actual data byte
	;
.ifdef __nios32__
	FILL16	%r0,%o1
	ST16d	[%i2],%r0
.else
	ST	[%i2],%o1
.endif

	;
	; Now, follow the AMD polling algorithm...
	;
	MOVIP	%g2,500000	; watchdog timeout, an excessive number

flashWriteLoop:
	;
	; Decrement watchdog
	;
	IFRz	%g2
	 BR	flashWriteFail
	SUBI	%g2,1

	;
	; Two reads, see if they match
	;
	LD	%g0,[%i2]
	LD	%g1,[%i2]
	CMP	%g0,%g1
	IFS	cc_ne
	 BR	flashWriteLoop

	LD	%g0,[%i2]
.ifdef __nios32__
;
; On Nios 32, be sure to look only at
; low 16 bits
;
	EXT16s	%g0,0
	EXT16s	%o1,0
.endif
	CMP	%o1,%g0		; match originally written data?
	IFS	cc_eq
	 BR	flashWriteOK
	NOP

flashWriteFail:
	;
	; the write failed, we shall print '!' for error.
	;
	MOVIP	%o0,'!'
	BSR	PutChar
	NOP
flashWriteOK:
	JMP	%o4
	NOP

stashByteRAM:
	.endif			; if not handling flash case...

	FILL8	%r0,%o1
	JMP	%o4		; return
	ST8d	[%i2],%r0	; (delay slot) actually stash the byte


; ----------------------------------
; particles of flash routines...
.if GM_FlashTop
flash555aa:				; write aa to 555
	MOVIP	%g0,0xAA
flash555g0:					; write %r0 to 555
	MOVIP	%l1,GM_FlashBase+(2*0x555)	; magic programming address 1
	JMP	%o7				; return
	ST		[%l1],%g0		; do the store in the delay slot

flash2AA55:					; write 55 to 2AA
	MOVIP	%g0,0x55
	MOVIP	%l1,GM_FlashBase+(2*0x2AA)	; magic programming address 2
	JMP	%o7				; return
	ST		[%l1],%g0		; do the store in the delay slot
.endif	; GM_FlashTop



