.global multiply_numbers
multiply_numbers:
    MUL R2, R0, R1
    BX LR

.global _start_mult
_start_mult:
    MOV R0, #3         
    MOV R1, #4         
    BL multiply_numbers 

    MOV R7, #1        
    SVC #0              
