mcro inner_macro
    mov r1, #5
    inc r1
mcroend

mcro outer_macro
    mov r2, #10
    dec r2
mcroend

START: mov r7, #0
       outer_macro
       inner_macro
END: stop