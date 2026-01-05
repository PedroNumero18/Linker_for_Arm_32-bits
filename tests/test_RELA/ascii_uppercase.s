.global _start          
.global to_uppercase

.text                   
_start:
    MOV R0, #'g'        
    BL to_uppercase      

end_program:
    B end_program        

to_uppercase:
    CMP R0, #'a'        
    BLT end_to_uppercase 
    CMP R0, #'z'         
    BGT end_to_uppercase 
    SUB R0, R0, #32     

end_to_uppercase:
    BX LR                
