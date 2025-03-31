mcro jump_forward
    jmp NEXT
mcroend

mcro jump_back
    jmp START
mcroend

START: mov r1, #0
NEXT: mov r2, #1
      cmp r1, #5
      jump_back
      jump_forward
END: stop