; Error test file
; This file contains intentional errors to test error reporting

; Invalid label names
1INVALID: mov r1, #5     ; Label starting with a number
INVALID@LABEL: mov r2, #6 ; Label with invalid character

; Invalid directives
.invalid 5, 6, 7        ; Unknown directive

; Invalid instructions
mvo r1, r2              ; Typo in instruction name
mox #5, r3              ; Another typo

; Invalid operand counts
mov r1                  ; Missing second operand
mov r1, r2, r3          ; Too many operands
cmp                     ; Missing both operands
stop r1                 ; Extra operand for stop

; Invalid operand types
mov #1, #2              ; Cannot move immediate to immediate
lea #5, r1              ; lea requires direct address as first operand
mov NOTFOUND, r1        ; Undefined label

; Invalid addressing for instruction
lea r1, r2              ; First operand must be a label
jsr #5                  ; Cannot use immediate addressing with jsr

; Invalid operand formats
mov r1, #ABC            ; Non-numeric immediate value
mov r1, r9              ; Invalid register number
mov r1, &               ; Missing label after &

; Macro errors
mcro test_macro
    mov r1, #5
; Missing endmcro

; Duplicate symbol definitions
DUPLICATE: .data 5
DUPLICATE: .string "Test" ; Symbol redefinition

; Entry/extern conflicts
.extern CONFLICT
CONFLICT: .data 10       ; Cannot define external symbol

.entry NOTDEFINED       ; Entry for undefined symbol

; Missing quotes in string
.string Missing quotes here

; Invalid numbers in data
.data 5.4               ; Floating point not supported
.data 1, a, 3           ; Non-numeric data