; Macro expansion test
; Tests macro definition and usage

; Simple increment macro
mcro inc_macro
inc r1
mcroend

; Simple move macro
mcro mov_macro
mov r1, #1
mcroend

; Main program
START: mov r1, #5
; Use macros
       inc r1         ; Regular instruction
       mov r1, #1     ; Regular instruction

; Add actual macro invocations to test expansion
       inc_macro      ; This should expand to "inc r1"
       mov_macro      ; This should expand to "mov r1, #1"

; More instructions
LABTEST: mov r2, r1
       stop

; Data section
DATA1: .data 1, 2, 3, 4
MSG:   .string "Macro Test"