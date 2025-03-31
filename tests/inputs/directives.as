; Directives test
; Tests directive types (.data, .string, .entry, .extern)

; External declarations
.extern EXTFN
.extern EXTVAR

; Main code
START: jsr EXTFN        ; Call external function
       mov r1, DATA1    ; Load data address
       mov r2, STR1     ; Load string address
       add r1, EXTVAR   ; Use external variable
       mov SUM, r1
       jsr FUN1         ; Call entry function
       jsr FUN2         ; Call another entry function
       stop             ; End program

; Data directive with values
DATA1: .data 100, -37, 450, 0, 347

; String directives
STR1: .string "Test string"
STR2: .string "Special chars: !@#$"
STR3: .string "Empty next:"
STR4: .string ""

; Entry point declarations
.entry FUN1
.entry FUN2
.entry SUM

; Functions that are entry points
FUN1: inc r1
      mov r5, r1
      rts

FUN2: mov r6, #-10
      prn r6
      rts

; Variable that is an entry point
SUM: .data 0