; Minimal addressing modes test
; No comments or extra spaces that might cause issues

START: mov r1, #100
       mov r2, DATA
       mov r3, r1
       add r3, r2
       add DATA, #5
       cmp r1, r2
       cmp r1, #25
       cmp r1, DATA
       jmp &SUBR
       bne &SUBR
       jsr &SUBR
       clr r5
       not DATA
       inc DATA

SUBR: prn #100
      rts

ENDP: stop

DATA: .data 100, -25, 78