; --------------------------
; File: nios_germs_monitor.s
;
; Contents: a Nios monitor,
;           this one with S-Record, I-Hex
;           records, and Flash Programming
;           for the Nios development board.
;
; ex:tabstop=4:
; ex:shiftwidth=4:
; ex:expandtab:
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
;	GM_VectorTable --
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

	.include "excalibur.s"

	.ifndef GM_UARTBase
	 .ifdef nasys_printf_uart
	  .equ GM_UARTBase,nasys_printf_uart
	 .else
	  #
	  # If there's no UART defined, we'll compile
	  # as IF the uart address is 0x0000.
	  # This, of course, won't be a functioning
	  # monitor, but, hey, there's no UART. What
	  # were you expecting?
	  # At least it puts some bytes in your
	  # ROM, and you can see what code looks like.
	  #
	  #.equ GM_UARTBase,0
	  # !!!
	  # The above comment is obsolete, but is preserved here
	  # for future generations to look upon, and marvel.
	  # See the bottom of this file for the code that's generated
	  # when no UART exists.
	  # !!!
	 .endif
	.endif

	.ifndef GM_FlashBase
	 .ifdef nasys_main_flash
	  .equ GM_FlashBase,nasys_main_flash
	 .endif
	.endif

	.ifdef GM_FlashBase

         #
	 # GM_Good_Old_Flash is the kind of
	 # flash on the original Nios board.
	 # nios_germs_monitor.s includes some
	 # code for accessing it directly.
	 #
	 # If it is not good old flash, then
	 # we tediously call whatever the C
	 # routines are available for erase
	 # and write. This takes a lot more
	 # bytes, but weve decided that in the
	 # year 2003, it is now ok to use
	 # as many as 2 kilobytes for the little
	 # old germs monitor.
	 #
	 # (We just presume that a 1meg flash
	 # is good old flash. A fragile guess,
	 # but likely to work.)
	 #
	 # dvb 2002
	 #

	 .if nasys_main_flash_size==0x100000 || (nasys_main_flash_size<=0x8000)
	  .equ GM_Good_Old_Flash,1
	 .else
	  .equ GM_Good_Old_Flash,0
	 .endif

	 .ifndef GM_FlashTop
	  .ifdef nasys_main_flash_end
	   .equ GM_FlashTop,nasys_main_flash_end
	  .else
	   .equ GM_FlashTop,GM_FlashBase+0x100000
	  .endif
	 .endif

	 .ifndef GM_FlashExec
	  .equ GM_FlashExec,GM_FlashBase+0x4000C
	 .endif

	.endif

	.ifndef GM_StackTop
	 .equ GM_StackTop,nasys_stack_top
	.endif

	.ifndef GM_VectorTable
	 .equ GM_VectorTable,nasys_vector_table
	.endif

	.ifndef GM_PIO
	 .ifdef na_button_pio
	  .equ GM_PIO,na_button_pio
	 .endif
	.endif

	.ifndef GM_WIDTHS
	 .equ GM_WIDTHS,1
	.endif

	.equ CR,13
.if __nios_jtag_stdio__
	.equ LF,10
.endif

;-----------------------------------------
; A simple macro that stores a 16 bit
; word on either the nios32 or nios16
; It uses %g0 (%r0), no foolin!

	.macro	G_ST16	rDst,rSrc
.ifdef __nios32__
	FILL16	%r0,\rSrc
	ST16d	[\rDst],%r0
.else
	ST	[\rDst],\rSrc
.endif
	.endm

;
; Mostly, this is one big goto-tangle.
;
; No RAM used
;
; Global Register Usage:
;
; %l0: UART Base Address
; %i4: JTAG rxchar address
; %i5: JTAG txchar address
; %g5: the value 0x0000000F
; %g6: the value 0x00000003	for masking addresses
; %g7: residual byte for writing to Flash in words

; %g0,%g1: Any routine's scratch
; %g3    : number of bytes read by getHex routine
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
	; Disable interrupts
	;

	PFX	8
	WRCTL	%g0

	;
	; Set CWP to top, and IRQ to bottom
	;

	PFX	2
	RDCTL	%g0
	LSRI	%g0,1				; HI_LIMIT in correct bit position
						; Also, the topmost bit IS zero now, after LSR
	ORIP	%g0,0x7e0f			; I_PRI set to 63, NZVC all true (why not)

				;
				; By a happy accident, this leaves the
				; Nios-2.3 cache bits cleared, which is
				; exactly what we want, if there happens
				; to be a cache on this Nios.
				; dvb 2002
				;
	WRCTL	%g0
    NOP         ; always NOP after a WRCTL that changes CWP

.ifdef GM_UARTBase

	; (previous germs monitors cleared the vector table
	; here, but it's more useful to be able to still
	; see it after you finish. Also, every byte is
	; precious, to keep it under 1k bytes! so its gone.)

	;
	; Reset UART, SwitchPIO, and Timers
	;
	MOVIP	%l0,GM_UARTBase			; we'll keep this...

.if __nios_jtag_stdio__
	;
	; this isn't necessary here since it appears the duplicate
	; code under postFlash_start will always be executed
	;
;	MOV	%i4,%l0
;	PFX	%hi(np_jtagrxdata)
;	ADDI	%i4,%lo(np_jtagrxdata)		; JTAG rxchar "register"

;	MOV	%i5,%l0
;	PFX	%hi(np_jtagtxdata)
;	ADDI	%i5,%lo(np_jtagtxdata)		; JTAG txchar "register"
	; instead of the above, save a couple bytes by assuming that np_jtagtxdata - np_jtagrxdata doesn't require a PFX
;	MOV	%i5,%i4
;	ADDI	%i5,np_jtagtxdata-np_jtagrxdata	; JTAG txchar "register"
.else
	MOVI	%g0,0
	STP	[%l0,np_uartcontrol],%g0	; control all 0's: no irq's
.endif

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
	; Set the stack pointer
	;
	.ifdef GM_StackTop
	MOVIP	%sp,GM_StackTop
	.endif

	;
	; Look for Signature in flash, & jump there.
	;
	.ifdef GM_PIO
	LDP	%g0,[%l1,np_piodata]	; Unless the button is pressed
	IF0	%g0,0
	 BR	postFlash_start
	.endif

	.ifdef GM_FlashTop
	MOVIP	%l2,GM_FlashExec	; 2nd quadrant of four, in flash, + 0xC
	LD	%g0,[%l2]
.ifdef __nios32__
	EXT16d	%g0,%l2
.endif
	CMPIP	%g0,'N'+(256*'i')
	IFS	cc_ne
	 BR	postFlash_start

	SUBI	%l2,0x000c
	LSRI	%l2,1
	CALL	%l2			; (if it wants to return, no problem!)
	.endif

postFlash_start:

	NOP				; clears out possible delay-slot PFX's

        ;;  We end up here upon exit of a user-program run with the 'G'
        ;;  command.  User-programs can do -anything-, including exitng
        ;;  with interrupts left running.  This is inconvenient when
        ;;  using the monitor.  Disable CPU interrupts as a defensive measure.
	PFX	8
	WRCTL	%g0
        
	;
	; Registers which need certain
	; values for the rest of the monitor
	; to work.
	; (Other registers are used as
	; needed, although their reservations
	; for certain uses must be respected
	; while running.)
	
	MOVIP	%l0,GM_UARTBase

.if __nios_jtag_stdio__
	MOV	%i4,%l0
	PFX	%hi(np_jtagrxdata)
	ADDI	%i4,%lo(np_jtagrxdata)		; JTAG rxchar "register"

	MOV	%i5,%l0
	PFX	%hi(np_jtagtxdata)
	ADDI	%i5,%lo(np_jtagtxdata)		; JTAG txchar "register"
	; instead of the above, save a couple bytes by assuming that np_jtagtxdata - np_jtagrxdata doesn't require a PFX
;	MOV	%i5,%i4
;	ADDI	%i5,np_jtagtxdata-np_jtagrxdata	; JTAG txchar "register"
.endif

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
	MOVI	%o0,CR

	#
	# Print the system name GM_SystemName, if defined
	#

	BR	pastSystemNameString
	NOP
systemNameString:
	nm_monitor_string	; include monitor name string
	.align	1		; align by 2^n
pastSystemNameString:
	MOVIA	%i2,systemNameString
printSystemNameLoop:
	LD	%o0,[%i2]
	EXT8d	%o0,%i2
	IFRz	%o0
	 BR	donePrintSystemNameLoop
	NOP
	BSR	PutChar
	ADDI	%i2,1
	BR	printSystemNameLoop
	NOP
donePrintSystemNameLoop:
	BSR	PutChar
	MOVI	%o0,CR

ReceiveCommand:
	MOVIP	%o0,'+'
	BSR	PutChar
	NOP

	BSR	GetChar
	NOP

	CMPIP	%o0,0x0060		; character from [60,7f]
	IFS	cc_ge				; mapped down to [40,5f]
	 ANDNIP	%o0,0x0020

	SUBIP	%o0,CR
	IFRz	%o0
	BR	doCR

		
	SUBIP	%o0,'#'-CR		; '#' (hash) is for comments: ignore to EOL
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
	MOVI	%o0,CR

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
	BSR	PutHex
.ifdef __nios32__
	EXT16d	%o1,%l5		; (delay slot)
.else
	NOP			; (delay slot)
.endif

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
	MOVI	%o0,CR			; (delay slot)
	BR	doMShowRangeLine
	NOP
doneMCR:
	BSR	PutChar
	MOVI	%o0,CR			; (delay slot)
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
	MOVI	%g3,0	; (GetHexUntil needs this to count bytes)
	MOV	%l7,%o0			; save the break-character...

.if GM_WIDTHS
	; decide which kind of write to do: 8, 16, or 32

	CMPI	%g3,2
	IFS	cc_eq
	 BR	doMWrite16
	NOP
.ifdef __nios32__
	IFS	cc_lt
	 BR	doMWrite8
	NOP

	BR	doMWrite_did
	ST	[%i2],%o1
.endif
doMWrite8:
	BSR	StashByte
	NOP
	BR	doMWrite_did
	NOP

doMWrite16:
	BSR	Stash16
	NOP

.else
	; Send in two bytes to the StashByte routine
	; so that it handles writing to flash
	BSR	StashByte
	MOV	%l6,%o1			; reserve high (later) byte
	LSRI	%l6,8
	MOV	%o1,%l6			; send the high byte
	BSR	StashByte
	ADDI	%i2,1			; (delay slot) to the next address
.endif

doMWrite_did:
	CMPI	%l7,CR			; %l7 has break char from GetHex
	IFS	cc_ne
	 BR	doMWriteLoop
	ADD	%i2,%g3			; (delay slot) increment address
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
fillRAMLoop:
	G_ST16	%l6,%o1
	ADDI	%l6,2			; go by twos and hit every 16 bit word (nios16 _and_ 32)
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

.ifdef GM_FlashTop
.if GM_Good_Old_Flash

	;
	; Traditional Nios Dev Board flash writing
	;

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
	G_ST16	%i2,%g0

; Loop until the data reads as 0xFFFF	(and we dont know whats in the high word)

.ifdef __nios32__
	BGEN	%g2,25			; watchdog timeout, an excessive number
.else
	BGEN	%g2,15			; (or a less-excessive number, scaled, on n16)
.endif

flashEraseLoop:

.ifdef __nios16__
	BGEN	%g1,10			; for nios 16, nest a loop here
flashEraseLoop_16:
	IFRnz	%g1
	 BR	flashEraseLoop_16
	SUBI	%g1,1
.endif

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

.ifdef __nios32__
	BGEN	%g0,20				; m32 only, eh.
.else
	BGEN	%g0,15			; (or a less-excessive number, scaled, on n16)
.endif

flashErasePostLoop:
	IFRnz	%g0
	 BR	flashErasePostLoop
	SUBI	%g0,#1				; (delay slot)


.else	; GM_Good_Old_Flash

	; call newfangled C version of flash erase
	; flash address in %o1
	;

	MOVIA	%g0,nr_flash_erase_sector@h
	BSR	call_c_routine
	NOP

.endif

	BR	ReceiveCommand
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

.ifeq __nios_jtag_stdio__
	;
	; The UART version of GetChar
	; 
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
.else
	;
	; The JTAG version of GetChar
	;
getCharLFEcho:			; front end loopover *just* for echoing those ignored Linefeeds.
	MOV	%g1,%o7					; save the previous return address...
	BSR	PutChar					; (this will break if PutChar uses %g1)
	NOP
	MOV	%o7,%g1					; ...and retore it
	
GetChar:
	LD	%o0,[%i4]				; see what's waiting
.ifdef __nios32__
	EXT16D	%o0,%i4
.endif
	SKP1	%o0,np_jtagstatus_rrdy_bit		; check the ready bit
	BR	GetChar
	NOP
	PFX	%hi(np_jtagstatus_trdy_mask)		; there is a byte waiting for us
	XOR	%o0,%lo(np_jtagstatus_trdy_mask)	; clear the ready bit
.ifdef __nios32__
	FILL16	%r0,%o0
	ST16D	[%i4],%r0				; and tell the OCI JTAG we're ready for another
.else
	ST	[%i4],%r0
.endif

.endif ;.ifeq __nios_jtag_stdio__

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
.ifeq __nios_jtag_stdio__
	;
	; The UART version of PutChar
	; 
	LDP	%g0,[%l0,np_uartstatus]	; status register
	SKP1	%g0,6		; bit 6: TRdy
	BR	PutChar
	NOP
	JMP	%o7
	STP	[%l0,1],%o0	; (delay slot) send %o0 out
.else
	;
	; The JTAG version of PutChar
	; 
	LD	%g0,[%i5]
.ifdef __nios32__
	EXT16D	%g0,%i5
.endif
	SKP0	%g0,np_jtagstatus_trdy_bit		; check the ready bit
	BR	PutChar					; loop until empty
	MOV	%g0,%o0					; (delay slot)
	PFX	%hi(np_jtagstatus_trdy_mask)
	OR	%g0,%lo(np_jtagstatus_trdy_mask)	; set the ready bit
.ifdef __nios32__
	FILL16	%r0,%g0
	JMP	%o7
	ST16D	[%i5],%r0				; (delay slot) put char into OCI JTAG register
.else
	JMP	%o7
	ST	[%i5],%r0				; (delay slot) put char into OCI JTAG register
.endif
.endif ;.ifeq __nios_jtag_stdio__

PutHash:
	MOVIP	%o0,'#'
	BR	PutChar		; (watch out for delay slot below...)

WaitForEOL:
	MOV	%o5,%o7
waitForEOLLoop:
	BSR	GetChar
	NOP
	CMPI	%o0,CR
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
;      %g3 - number of bytes read (if caller cleared %g3 first)
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
;
; Return number of bytes read in %g3 (caller MUST clear
; %g3 first, for this feature to work)

GetHexUntil:
	BR		getHexEntry1	; dive in with user's %o2 and 31 char limit
	MOVI	%o3,31

GetHex:
	MOVI	%o3,31		; maximum character count
GetHexBytes:			; entry point to use %o3 to signify byte count
	MOVI	%o2,CR		; default return-break char
getHexEntry1:
	ADD	%o3,%o3		; double for chars
	MOV	%o5,%o7		; return address...
	MOVI	%o1,0		; result
getNextHex:
	MOV	%o4,%o1		; come here after a hypen, too
	MOVI	%o1,0		; result'

getHexLoop:
	BSR	GetChar
	ADDI	%g3,1		; bump char count

	CMPIP	%o0,'-'		; hyphen separated pair...
	IFS	cc_eq
	 BR	getNextHex

	CMP	%o0,%o2		; user-passed return character?
	IFS	cc_eq		; go home
	 BR	getHexDone	; exit loop from here
	CMPI	%o0,CR		; carriage return?
	IFS	cc_eq		; go home
	 BR	getHexDone	; exit loop from here
	SUBI	%o3,1		; decrement char count
	SKP0	%o0,6		; bit 6 indicates alpha char
	ADDI	%o0,9		; if so, move up to 10-15
	AND	%o0,%g5		; and with 0x0f.
	LSLI	%o1,4
	SKPRz	%o3		; was this the last character?
	BR	getHexLoop	; loop back if not
	ADD	%o1,%o0		; (delay slot) last or not, add in the last nibble
getHexDone:
	JMP	%o5		; return
	LSRI	%g3,1		; turn char count to byte count

;--------------------
; Return Z flag set if %i2 is in flash range.
; Trash %l1.

	.ifdef GM_FlashTop		; nonzero FlashTop means we have some flash
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
	IFS	cc_mi
	 MOVI	%l1,0			; out of range
	MOVIP	%r0,GM_FlashTop
	CMP	%i2,%r0
	IFS	cc_pl
	 MOVI	%l1,0			; out of range
	JMP	%o7
	CMPI	%l1,1
	.endif

	.endif

.if GM_WIDTHS
;--------------------
; Stash16 -- either
; do a st16 (or st, on nios16)
; or pass it off to flashwrite
Stash16:
 .ifdef GM_FlashTop
	MOV	%o4,%o7		; stash return address, deeper than usual!
	BSR	InFlashRange
	NOP
	IFS	cc_eq
	 BR	FlashWrite16
	NOP
	G_ST16	%i2,%o1
	JMP	%o4
	NOP
 .else
	G_ST16	%i2,%o1
	JMP	%o7
	NOP
 .endif
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

	.ifdef GM_FlashTop
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

FlashWrite16:			; %o4 must have return address already...

	;
	; At this point, %o1 has a halfword to write,
	; %i2 is the halfword address (plus 1, sorry)
	;


.if GM_Good_Old_Flash
	;
	; Traditional nios-dev-board flash
	;

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
	; And the actual data halfword
	;
	G_ST16	%i2,%o1

	;
	; Now, follow the AMD polling algorithm...
	;
.ifdef __nios32__
	MOVIP	%g2,0x80000	; watchdog timeout, an excessive number, n32 only (~.5M)
.else
	MOVIP	%g2,0x8000
.endif


flashWriteLoop:
	;
	; Decrement watchdog
	;
	IFRz	%g2
	 BR	flashWriteFail
	SUBI	%g2,1

.ifdef __nios16__
	MOVI	%g1,20		; add extra cycles on n16, !!! could be made a subroutine?
flashWriteLoop_16:
	IFRnz	%g1
	 BR	flashWriteLoop_16
	SUBI	%g1,1
.endif

	LD	%g0,[%i2]	; (historically, this line of code was missing for a while...)
.ifdef __nios32__
;
; On Nios 32, be sure to look only at
; the correct 16 bits
;
	EXT16d	%g0,%i2
.endif
	CMP	%o1,%g0		; match originally written data?
	IFS	cc_eq
	 BR	flashWriteOK	; yes, we're done
	NOP			; (delay slot);
	
	BR	flashWriteLoop	; no, keep trying
	NOP			; (delay slot);

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

.else   ; GM_Good_Old_Flash

	; !!! call C routine to write a word to flash

	; At this point, %o1 has a halfword to write,
	; %i2 is the halfword address (plus 1, sorry)
	MOV	%o2,%o1
	MOVIA	%g0,nr_flash_write@h
	MOV	%o1,%i2
        ;
        ; When doM calls us, the address is definitely even
        ; When doS (S-record) calls us, the address is always on the
        ; odd phase, as it goes byte-by-byte.
        ;
        ; clear the low bit to force it to be the next-lowest even.
        ;
	ANDNIP	%o1,1	; clear low bit of address, to make it even
	BSR	call_c_routine
	NOP

	IFRz	%o0	; succeeded?
	JMP	%o4
    NOP         ; dont have prefix in delay slot, ha ha.

	MOVIP	%o0,'!'
	BSR	PutChar
	NOP
	JMP	%o4
	NOP


.endif

stashByteRAM:
	.endif			; if not handling flash case...

	FILL8	%r0,%o1
	JMP	%o4		; return
	ST8d	[%i2],%r0	; (delay slot) actually stash the byte


; ----------------------------------
; particles of flash routines...
.ifdef GM_FlashTop
.if GM_Good_Old_Flash

flash555aa:				; write aa to 555
	MOVIP	%g0,0xAA
flash555g0:					; write %r0 to 555
	MOVIP	%l1,GM_FlashBase+(2*0x555)	; magic programming address 1
flashSharedWrite:
	G_ST16	%l1,%g0		; do the store in the delay slot
	JMP	%o7				; return
	NOP

flash2AA55:					; write 55 to 2AA
	MOVIP	%g0,0x55
	MOVIP	%l1,GM_FlashBase+(2*0x2AA)	; magic programming address 2
	BR	flashSharedWrite
	NOP

.else	; GM_Good_Old_Flash

	;
	; Its a newfangled flash. So, instead of
	; providing those flash555 particles, we have
	; a little help for calling C flash routines
	;
	; %g0 should have the address of the routine to
	; call (Nios shifted down by 1, la la), and
	; arguments in %o1 and %o2. All flash routines
	; want nasys_main_flash in o0, with -1 indicating
	; default, so we put that into %o0 every time
	;
	; Note! we actually use register windows here, unlike
	; anywhere else in germs. This is new for the 2003
	; edition. -- dvb 2002
	;

	; 1. Open a register window, and move the parameters
	;    down to it. Ignore the stack pointer. We don't care.

call_c_routine:
	SAVE	%sp,0
	MOVI	%o0,1
	NEG	%o0
	MOV	%o1,%i1
	MOV	%o2,%i2

	; 2. Save some of our best G registers into the newly opened
	;    register window L registers

	MOV	%l3,%g3
	MOV	%l5,%g5
	MOV	%l6,%g6

	; 3. call the C routine

	CALL	%g0
	NOP

	; 4. restore Gs

	MOV	%g3,%l3
	MOV	%g5,%l5
	MOV	%g6,%l6

	; 5. stash result and pop up a register level

	MOV	%i0,%o0
	JMP	%i7
	RESTORE      ; (delay slot)





.endif  ; GM_Good_Old_Flash
.endif	; GM_FlashTop


.else	
# GM_UARTBase -- no uart, the monitor is empty
; There is no monitor here, since the system has no UART.
; Here's an infinite loop for your CPU to enjoy.
NoUART:
	BR	NoUART
	NOP
	.asciz	"There is no monitor here."
.endif


# End of file
