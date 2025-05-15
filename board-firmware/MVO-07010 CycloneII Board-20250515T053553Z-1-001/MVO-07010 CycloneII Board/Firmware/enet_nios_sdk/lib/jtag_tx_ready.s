

        .include "excalibur.s"
        .text

;----------------------------------------
;         Name: nr_jtag_tx_ready
;  Description: Return the state of np_jtagstatus_trdy_bit
;        Input: %o0: the base address of the OCI, 0 for default
;       Output: %o0 = zero if not ready to transmit, non-zero if it is
; Side Effects: %g0 altered
;    CWP Depth: 0
;

; nr_jtag_tx_ready
        .global	nr_jtag_tx_ready
nr_jtag_tx_ready:

    MOV     %g0,%o0

.ifdef nasys_printf_uart
    IFRnz   %g0                     ; if 0, use "default" (if 0x00000000 was indicated, no harm done)
    BR      txRdyA
    NOP
    MOVIA   %g0,nasys_printf_uart
.endif ; nasys_printf_uart

txRdyA:

    PFX     %hi(np_jtagtxdata)
    ADDI    %g0,%lo(np_jtagtxdata)

    _PFX   0   ; cache safety
    LD      %o0,[%g0]
.ifdef __nios32__
    EXT16D  %o0,%g0
.endif

    PFX     %hi(np_jtagstatus_trdy_mask)
    AND     %o0,%lo(np_jtagstatus_trdy_mask)

    PFX     %hi(np_jtagstatus_trdy_mask)
    XOR     %o0,%lo(np_jtagstatus_trdy_mask)

    JMP     %o7
    NOP
