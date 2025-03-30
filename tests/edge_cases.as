; Minimal edge cases test
; No special characters or complex formatting

START: mov r1, #100
       mov r2, #1000
       mov r3, #-1000

STR1: .string "Basic string"
STR2: .string ""
DATA: .data 1, 2, 3, 4
ZERO: .data 0

       mov r0, r1
       mov r1, r2
       add r1, r2
       inc r5
       rts
       stop

END: stop