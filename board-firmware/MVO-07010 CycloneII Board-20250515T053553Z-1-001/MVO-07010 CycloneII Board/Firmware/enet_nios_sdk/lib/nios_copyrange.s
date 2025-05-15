; file: nr_copyrange.s
;

        .include "excalibur.s"
        .text

;----------------------------------------
;         Name: nr_copyrange
;  Description: Copy a range of memory
;        Input: %o0 = source, %o1 = destination, %o2 = destination_end
;       Output: none
; Side Effects: %g0, %g1, & %g2, destroys %o0, %o1, %o2, %o3
;    CWP Depth: 0
;
; Note: source & dest & dest_end must be aligned, else
; it spins forever.
;

    .global nr_copyrange
nr_copyrange:

    ; |
    ; | IF
    ; |   src, dst, or dst_end are unaligned (not end in binary 00),
    ; | THEN
    ; |   jump on over to the slow copy case
    ; |

    MOV %o3,%o0
    OR  %o3,%o1
    OR  %o3,%o2
.ifdef __nios32__
    ANDIP %o3,0x0003  ; nios 32 -- align by 4 bytes
.else
    ANDIP %o3,0x0001  ; nios 16 -- align by 2 bytes
.endif
    IFRnz %o3
     BR nr_copyrange_slow
    NOP                     ; (branch delay slot)

nr_copyrange_loop:
    CMP %o1,%o2
    IFS cc_eq
     JMP    %o7
    NOP         ; (branch delay slot)

    _PFX    0   ; cache safety -- this is the sort of routine
                ; that one might use to copy blocks of code...

    LD  %g0,[%o0]
    ST  [%o1],%g0
.ifdef __nios32__
    ADDI    %o0,4
    BR  nr_copyrange_loop
    ADDI    %o1,4       ; (branch delay slot)
.else
    ADDI    %o0,2
    BR  nr_copyrange_loop
    ADDI    %o1,2   ; (branch delay slot)
.endif


nr_copyrange_slow:      ; slow, but works for unaligned anythings

nr_copyrange_slow_loop:
    CMP     %o1,%o2     ; done yet?
    IFS     cc_eq
     JMP    %o7         ; if done, return
    NOP

    _PFX    0           ; no cache next instruction
    LD      %g0,[%o0]
    EXT8d   %g0,%o0
    FILL8   %r0,%g0
    ST8d    [%o1],%r0
    ADDI    %o0,1
    BR      nr_copyrange_slow_loop
    ADDI    %o1,1       ; (branch delay slot)


; end of file
