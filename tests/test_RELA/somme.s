.global add_numbers
add_numbers:
    ADD R0, R0, R1
    BX LR

.global _start
_start:
    MOV R0, #5         
    MOV R1, #7         
    BL add_numbers     

    MOV R7, #1         
    SWI 0           
