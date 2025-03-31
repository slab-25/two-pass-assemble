mcro test_macro
    mov r1, #1    ; No label inside macro
    inc r1
mcroend

FIRST: mov r2, #2
       test_macro
NEXT: mov r3, #3
      test_macro
END: stop