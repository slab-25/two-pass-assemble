; Macro with labels test
; Tests interaction between macros and labels
; Note: This test respects the limitation that labels inside macros
; are only accessible within the macro scope

; ============================
; Basic increment macro
; ============================
mcro inc_macro
inc r1
mcroend

; ============================
; Macro with jump to external label
; ============================
mcro jump_to_end
cmp r1, #0
bne ENDLBL
mcroend

; ============================
; Macro with conditional branching
; ============================
mcro check_value
cmp r2, #5
bne CONTLBL
mov r3, #100
CONTLBL: inc r2
mcroend

; ============================
; Main program
; ============================
STARTLBL: mov r1, #1
          mov r2, #2

; Test macro after a label
BEFOREMCR: inc_macro

; Define a label that will be jumped to from inside a macro
ENDLBL: mov r5, #5

; Use the jump macro
jump_to_end

; Test using the same macro multiple times
LOOP1: inc_macro
LOOP2: inc_macro
LOOP3: inc_macro

; Test macro with internal label (remains local to the macro)
check_value

; End program
FINAL: stop

; Data section
VALUES: .data 1, 2, 3, 4