mcro reg_copy
    mov r1, r2
    mov r2, r3
    mov r3, r4
mcroend

mcro reg_swap
    mov r7, r1
    mov r1, r2
    mov r2, r7
mcroend

START: mov r1, #1
       mov r2, #2
       mov r3, #3
       mov r4, #4
       reg_copy
       reg_swap
END: stop