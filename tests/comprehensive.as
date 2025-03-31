; Comprehensive test program
; This program demonstrates most features of the assembler
; including macro expansion, various addressing modes,
; and multiple directives.

; =================================================
; Macro Definitions
; =================================================

; Macro to print a value in register r1
mcro print
prn r1
mcroend

; Macro to initialize registers
mcro init
mov r1, #0
mov r2, #0
mov r3, #0
mcroend

; =================================================
; Main Program
; =================================================

; Initialize program
START: init           ; This will expand to three mov instructions
       mov r1, #10     ; Load initial value
       jsr PRINTVAL    ; Print the initial value

; Loop to sum values from 1 to 10
LOOP:  cmp r1, #0      ; Check if we've reached 0
       bne &CONTINUE   ; If not, continue the loop
       jmp &DONE       ; If we're done, jump to DONE

CONTINUE: add r2, r1    ; Add current value to sum
       dec r1          ; Decrement counter
       jmp &LOOP       ; Repeat the loop

; Print results and exit
DONE:  mov r1, r2      ; Move sum to r1 for printing
       print           ; Use the print macro
       mov r3, RESULT  ; Store result in memory
       stop

; =================================================
; Subroutines
; =================================================

; Print value subroutine
PRINTVAL: print        ; Use the print macro
       rts             ; Return from subroutine

; =================================================
; Data Section
; =================================================

; String constants
PROMPT: .string "Sum calculation program"
RESULT: .data 0        ; Storage for result

; =================================================
; External references
; =================================================

; Example of using external symbols
.extern EXTPRINT
.entry PRINTVAL        ; Export our print routine