; Tests boundary conditions and extreme cases

mcro max_macro
    mov r1, #127    ; Maximum immediate value
    mov r2, #-128   ; Minimum immediate value
mcroend

mcro long_macro
    mov r3, #1
mcroend

mcro empty

mcroend

START: mov r1, #0
LABEL1: max_macro       ; Shortened label name
        long_macro      ; Shortened macro name
        empty
END: stop