; Macro edge cases test
; Tests various edge cases in macro handling

; Test case 1: Empty macro
mcro empty_macro
mcroend

; Test case 2: Multi-line macro with comments
mcro comment_macro
    ; Comment line 1
    mov r1, #5    ; Inline comment
    ; Comment line 2
    mov r2, r1
mcroend

; Test case 3: Macro with label
mcro label_macro
LOCAL1: mov r3, #10
LOCAL2: inc r3
mcroend

; Test case 4: Single instruction macro
mcro inc_r7
    inc r7
mcroend

; Test case 5: Nested label usage
mcro branch_macro
    cmp r1, OUTER
    jmp INNER
    INNER: mov r1, #1
mcroend

; Main program
OUTER: mov r1, #0
       mov r2, #1
       mov r3, #2
       mov r7, #0

; Test empty macro expansion
       empty_macro

; Test comment handling
       comment_macro

; Test label scoping
       label_macro

; Test multiple invocations
       inc_r7
       inc_r7

; Test label interaction
NEXT: mov r4, #0     ; Changed: Label must have real instruction
      branch_macro
      mov r4, r7

; Test macro after label
LAST: mov r5, #0     ; Changed: Label must have real instruction
      empty_macro

; End program
END: stop

; Data section
DATA: .data 1,2,3,4
STR: .string "test"