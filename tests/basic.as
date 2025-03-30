; Basic functionality test
; Tests labels, basic instructions, and simple data

MAIN: mov r3, LENGTH
     mov r1, #0      ; initialize counter
LOOP: jmp L1
     prn #-5         ; this won't execute
L1:   inc r1          ; increment counter
     mov r0, STR      ; get string location
     cmp r0, #0      ; check if null
     bne LOOP        ; continue if not end

END:  stop

STR:  .string "abcdef"  ; string to be processed
LENGTH: .data 6           ; length of string

; End of program