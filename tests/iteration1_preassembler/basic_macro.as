; Test ID: PRE-001
; Test Objective: Validate basic macro expansion

mcro INCREMENT
    inc r1
    mov r1, r2
endmcro

MAIN: mov r3, r2
      INCREMENT
      stop
