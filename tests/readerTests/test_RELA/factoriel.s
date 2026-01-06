.global factorial
factorial:
    MOV R1, #1         
factorial_loop:
    MUL R2, R1, R0     
    MOV R1, R2
    SUBS R0, R0, #1    
    BNE factorial_loop 
    MOV R0, R1         
    BX LR

.global _start_fact
_start_fact:
    MOV R0, #5         
    BL factorial       

    MOV R7, #1         
    SVC #0
