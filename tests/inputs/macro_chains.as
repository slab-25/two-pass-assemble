mcro first_macro
    mov r1, #1
mcroend

mcro second_macro
    mov r2, #2
mcroend

mcro third_macro
    mov r3, #3
mcroend

START: mov r7, #0
       first_macro
       second_macro
       third_macro
END: stop