; Test ID: INT-001
; Test Objective: Complete assembly process

mcro MAC1
    mov r1, r2
    inc r1
endmcro

    .extern EXT
MAIN: MAC1             ; Expands to mov r1,r2 and inc r1
      add r3, VAL      ; Uses data label
      jsr EXT          ; Uses external symbol
      jmp LOOP         ; Forward reference
LOOP: stop
VAL:  .data 6, -9
      .entry MAIN      ; Entry point
