mcro data_macro
    .data 1,2,3
mcroend

mcro string_macro
    .string "test"
mcroend

START: mov r1, #0
DATA1: data_macro
DATA2: string_macro
STR: .string "end"
END: stop