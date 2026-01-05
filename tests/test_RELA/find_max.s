.global find_max
find_max:
    LDR R2, [R0], #4   
    SUBS R1, R1, #1    
    BEQ end_find_max   
find_max_loop:
    LDR R3, [R0], #4   
    CMP R2, R3         
    IT HI              
    MOVHI R2, R3       
    SUBS R1, R1, #1    
    BNE find_max_loop  
end_find_max:
    MOV R0, R2         
    BX LR

.global _start_max
_start_max:
    LDR R0, =array     
    MOV R1, #5         
    BL find_max        

    MOV R7, #1         
    SWI 0              

.data
array: .word 3, 8, 2, 10, 6
