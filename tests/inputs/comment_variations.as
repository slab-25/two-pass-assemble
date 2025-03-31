; Comment before macro
mcro comment_macro
    ; Inside comment
    mov r1, #1    ; Inline comment
    inc r1        ; Another comment
    ; Multiple
    ; Comments
mcroend

START: mov r7, #0    ; Start comment
       comment_macro ; Macro call comment
END: stop           ; End comment