; Macro errors test
; Tests various error conditions in macro handling
; This file is EXPECTED to fail compilation

; ============================
; Error: Macro with reserved word name
; ============================
mcro mov
inc r1
mcroend

; ============================
; Error: Duplicate macro name
; ============================
mcro test_macro
mov r1, #5
mcroend

mcro test_macro
mov r2, #10
mcroend

; ============================
; Error: Invalid tokens after mcro
; ============================
mcro extra_tokens extra_stuff
mov r3, #15
mcroend

; ============================
; Error: Invalid tokens after mcroend
; ============================
mcro valid_macro
mov r4, #20
mcroend extra_stuff

; ============================
; Error: mcroend without matching mcro
; ============================
mcroend

; ============================
; Error: Nested macro definition (not supported)
; ============================
mcro outer_macro
mov r5, #25
mcro inner_macro
inc r5
mcroend
dec r5
mcroend

; ============================
; Main program
; ============================
START: mov r1, #1
       stop