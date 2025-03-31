mcro init
    mov r1, #1
    mov r2, #2
mcroend

mcro process
    cmp r1, r2
    inc r1
    dec r2
mcroend

mcro finalize
    mov r7, r1
    mov r6, r2
mcroend

START: init
LOOP: process
      cmp r1, #10
      bne LOOP
      finalize
END: stop