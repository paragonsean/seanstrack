; file: nios_setup.s
;

; ----------------------------------------
; If you don't define your own _start,
; this one will get linked in (if you link
; against libnios.a in your ld stage).
;
; This _start sets up the stack pointer,
; puts the register window to the top,
; and jumps to the user's "main".
;
; If main returns, TRAP 0 is executed.
;

	.include "excalibur.s"
	.text
	.global _start
	.global _past_main
	
_start:
	;---------------
	; Stash the return address in _main_return,
	; so that exit() can really go where it goes.

	MOV	%g5,%o7			; stash in g5, so we can set CWP _and_ clear .bss, then stash

	;---------------
	; Preset CWP up to top
	; 
	PFX	2
	RDCTL	%g1
	LSRI	%g1,1
	ANDIP	%g1,0x01f0		; g1 = HI_LIMIT in position for ctl0
	RDCTL	%g0
	ANDNIP	%g0,0x81f0		; IRQ disabled, and space for cwp
	OR	%g0,%g1
	WRCTL	%g0
	NOP

	;---------------
	; Set Stack Pointer
	;

.ifdef __nios32__
	MOVIA	%sp,nasys_stack_top-72	; storage for registers, if underflowed
.else
	MOVIA	%sp,nasys_stack_top-36
.endif

    ; -----------------
    ; As a courtesy to debuggers, which tend to halt
    ; at <main> rather than <main after save %sp>,
    ; preload %i7 with the same value as %o7. This lets
    ; local variables views already show the right places,
    ; even though those places are below the stack
    ; pointer and are completely suspect
    MOV %fp,%sp

    ; -----------------
    ; in general, we do not and should not concern ourselves
    ; with peripheral initialization here. this startup
    ; code is supposed to be generic.
    ;
    ; But we have been troubled often enough by the
    ; main timer being left on after using nr_timer_milliseconds
    ; that we shall just go ahead and disable interrupts
    ; on it, if there happens to be one in this design
    ;
    ; dvb 2003

.ifdef nasys_timer_0
    MOVIP   %L0,nasys_timer_0
    MOVI    %L1,0
    STP     [%L0,np_timercontrol],%L1
.endif


	;---------------
	; Zero out the .bss segment (uninitialized common data)
	;
	MOVIA	%o0,__bss_start		; presume nothing is between
	MOVIA	%o1,_end		; the .bss and _end.
	_BSR	nr_zerorange
	SUB	%o1,%o0			; (delay slot) ZeroRange takes a count

	; --------------
	; If code and data are in different
	; memory ranges, copy the initial data to
	; where it gets used.
;;.if (nasys_program_mem!=nasys_data_mem)
	MOVIA	%o0,_nasys_data_source
	MOVIA	%o1,_nasys_data_destination
	MOVIA	%o2,_nasys_data_destination_end
	_BSR	nr_copyrange
	NOP
;;.endif

	;------------------
	; Stash the return address in memory now
	;
	MOVIA	%g0,_main_return
	ST	[%g0],%g5

	;---------------
	; Set the SBrk variable "RAMLImit"
	;
	MOVIA	%o0,_end
	MOVIA	%o1,RAMLimit
	ST	[%o1],%o0

	;---------------
	; Set up us the vector table
	; to catch any spurious interrupt
	; for great justice.
	;
.if __nios_catch_irqs__
.ifdef nasys_printf_uart
	MOVIA	%o0,r_spurious_irq_handler@h
	MOVIP	%o1,nasys_vector_table
	MOVIP	%o2,64
_init_vector_table_loop:
	ST	[%o1],%o0
.ifdef __nios32__
	ADDI	%o1,4
.else
	ADDI	%o1,2
.endif
	SUBI	%o2,1
	IFRnz	%o2
	 BR _init_vector_table_loop
	NOP
.endif ; nasys_printf_uart
.endif ; __nios_catch_irqs__

	;---------------
	; Install Underflow/Overlow Handlers
	;
.if __nios_use_cwpmgr__
	_BSR	nr_installcwpmanager
	NOP
.endif

	
	;---------------
	; Call c++ constructors
	;
.if __nios_use_constructors__
	MOVIA	%l0,_ctors_begin
	MOVIA	%l1,_ctors_end
call_constructors:
	CMP	%l0,%l1
	IFS	cc_eq
	 BR	done_calling_constructors
	LD	%l2,[%l0]
	CALL	%l2
.ifdef __nios32__
	ADDI	%l0,4		; (delay slot)
.else
	ADDI	%l0,2		; (delay slot)
.endif
	BR	call_constructors
	NOP
done_calling_constructors:
.endif

	; install the GDB stub if debugging is enabled
.ifdef __nios_debug__
	MOVIA	%g0,nios_premain_hook_function@h
	CALL	%g0
	MOVI	%o0,1
.endif

	;---------------
	; Enable IRQ's
	;
	RDCTL	%g0
	ORIP	%g0,0x8000		; set the interrupt enable bit.
	WRCTL	%g0
        NOP
        
	;---------------
	; initialize caches if present
	;

	.if nasys_has_icache
	_BSR nr_icache_init
    NOP                    ; (delay slot)
	.endif

	.if nasys_has_dcache
	_BSR nr_dcache_init
    NOP                    ; (delay slot)
	.endif

	;---------------
	; Call main()
	;
	MOVIA	%g0,main@h		; call main as a subroutine, then trap 0.
	CALL	%g0
	NOP

_past_main:				; exit() jumps to here.
	;---------------
	; Call exit func, if any
	; This is used by gprof's mcleanup
	; You might trim this, to save a few more bytes.
	;
	MOVIA	%g0,atexit_func
	LD	%g0,[%g0]
	IFRnz	%g0
	 CALL	%g0
	NOP

	;---------------
	; disable caches if present
	;

	.if (nasys_has_icache || nasys_has_dcache)
	RDCTL	%g1
	MOVIP	%g0,(nasys_has_icache * np_nios_icache_bit) + (nasys_has_dcache * np_nios_dcache_bit)
	ANDN	%g1,%g0
	WRCTL	%g1
        NOP
	.endif

	;---------------
	; Call c++ destructors
	;
.if __nios_use_constructors__
	MOVIA	%l0,_dtors_begin
	MOVIA	%l1,_dtors_end
call_destructors:
	CMP	%l0,%l1
	IFS	cc_eq
	 BR	done_calling_destructors
	LD	%l2,[%l0]
	CALL	%l2
.ifdef __nios32__
	ADDI	%l0,4		; (delay slot)
.else
	ADDI	%l0,2		; (delay slot)
.endif
	BR	call_destructors
	NOP
done_calling_destructors:
.endif

	;---------------
	; If we return from main,
	; return to the caller (usually the monitor)
	;

	.ifdef na_cpu_oci_core
	;
	; unless there's an OCI core. then just
	; halty.
	;

halt_forever:
	BR	halt_forever
	ADDI	%g0,1
	.endif

	MOVIA	%g0,_main_return
	LD	%o7,[%g0]
	JMP	%o7
	NOP

;---------------------------
; If debugging, include handler for spurious interrupts

;
; This handler is just going to print some things, and
; then exit to the reset address.
;

.if __nios_catch_irqs__
.ifdef nasys_printf_uart

r_spurious_irq_handler:
	RDCTL %g4
	RESTORE			; so we can make subroutine calls

	LSRI	%g4,9
	ANDIP	%g4,0x003f	; mask out just the irq number

	MOV	%l4,%g4

    nm_txcr

	nm_print "spurious interrupt number: "

	_BSR nr_txhex
	MOV	%o0,%l4
	
    nm_txcr

	MOVIA	%o0,nasys_reset_address@h
	JMP	%o0
	NOP

.endif ; nasys_printf_uart
.endif ; __nios_catch_irqs__


;---------------------------------------------------------
; This is a global variable used by the memory allocation
; routines.

	.align	2
	.section ".data"
RAMLimit:
	.global	RAMLimit
	.nword	_end		; this is a linker-supplied value

	.comm atexit_func,nios_wordsize,nios_wordsize	; one word, word-aligned
	.comm _main_return,nios_wordsize,nios_wordsize

; End of file.
