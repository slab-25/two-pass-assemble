; Macro expansion test
; Tests macro definition and usage

; Simple increment macro
mcro inc_macro
inc r1
endmcro

; Simple move macro
mcro mov_macro
mov r1, #1
endmcro

; Main program
START: mov r1, #5
; Use macros
       inc r1         ; Regular instruction
       mov r1, #1     ; Regular instruction

; More instructions
LABTEST: mov r2, r1
       stop

; Data section
DATA1: .data 1, 2, 3, 4
MSG:   .string "Macro Test"