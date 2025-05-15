P0.31746  ;Position divisor, 20P rod, metric units

;Program
.001 T-1 B0.*3        ;Turn off both slow and accumulator
.002 J0.1 I0.17
.003 T-6 W0.23        ;Open valve to retract setting while not at home
.004 T0 N             ;Then park valve
.005 J0.1 I0.17       ;Jump to block 1 if pump off
.006 T-1 N
.007 T-1 N
.008 J0.8 I1.18       ;Trap start shot or jog shot
.009 J0.8 I1.21
.010 T-1 N
.011 T-1 N
.012 J0.1 I0.17
.013 J0.35 I1.21      ;Jump if jog shot
.014 J0.40 I1.18      ;Jump if start shot
.015 J0.68 I1.24      ;Jump if valve test
.016 J0.12 N
.017 T-1 N
.018 T-1 N
.019 T-1 N
.020 T-1 N
.021 T-9 N            ;Clear the shot in progress status bit
.022 J0.1 I0.17
.023 J0.1 I1.22
.024 J0.21 I0.19      ;Loop if not follow through
.025 T-7              ;Open valve to the follow through setting
.026 T-1 N
.027 T-1 N
.028 J0.1 I0.17
.029 J0.1 I1.22
.030 J0.28 I0.20      ;Loop if not retract
.031 J0.1 N
.032 T-1 N
.033 T-1 N
.034 T-1 N
; JOG SHOT
.035 S0 N
.036 T-5 B1.1
.037 J0.36 I1.21
.038 J0.1 N
.039 T-1 N
; SHOT
.040 T-8 N         ;Set shot in progress bit
.041 J0.60 I1.25   ;Jump if 8IPS
.042 T-4LD0.25 N   ;Set valve to center pos + lvdt and null offsets
.043 S0 B1.1
.044 T-1 B1.2   ;V.15AC5P50 E1.2 <--- Alternative short step and then acc.
.045 VV0ACV1GV2PV3LDV4LIV5 W0.V6
.046 VV7ACV8GV9PV10LDV11LIV12 W0.V13
.047 VV14ACV15GV16PV17LDV18LIV19 W0.V20
.048 VV21ACV22GV23PV24LDV25LIV26 W0.V27
.049 VV28ACV29GV30PV31LDV32LIV33 W0.V34
.050 VV35ACV36GV37PV38LDV39LIV40 W0.V41
.051 J0.21 N
;8IPS SHOT
.060 J0.1 I1.27       ;Don't allow an 8IPS shot in vacuum mode
.061 S0 B1.1
.062 T-4SD0.02 B1.2   ;Set valve to center pos
.063 V.2AC50P2000 N  ;Go until zspeed
.064 J0.21 N
.065 T-1 N
.066 T-1 N
.067 T-1 N
; VALVE TEST
.068 J0.1 I0.24
.069 T12148 N
.070 T-4LD2.0 N       ;Center valve
.071 T-1LD2.0 N
.072 J0.68 N
.073 END


;                     I/O List
;
;         INPUTS
;
; 17 == Pump on
; 18 == Start shot
; 19 == Follow through
; 20 == Retract
; 21 == Jog Shot
; 22 == Manual mode
; 23 == At home
; 24 == Valve test
; 25 == .2 MPS test
; 26 == OK For fast shot
; 27 == Vacuum on
; 28 == Low impact
;
; 32 == E-Stop
;
;
;         OUTPUTS
;
; 1  == Slow Out
; 2  == Accumulator
;
;

