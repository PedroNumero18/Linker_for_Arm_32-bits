.global filter_even
filter_even:
    MOV R3, #0
filter_even_loop:
    LDR R4, [R0], #4
    ANDS R5, R4, #1
    BEQ store_even
    SUBS R1, R1, #1
    BNE filter_even_loop
    B end_filter_even
store_even:
    STR R4, [R2], #4
    ADD R3, R3, #1
    SUBS R1, R1, #1
    BNE filter_even_loop
end_filter_even:
    MOV R0, R3
    BX LR

.global _start_filter
_start_filter:
    LDR R0, =array
    MOV R1, #5
    LDR R2, =result
    BL filter_even

    MOV R7, #1
    SVC #0

.data
array:  .word 1, 2, 3, 4, 5
result: .space 20
