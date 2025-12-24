#include <stdlib.h> 
#include <stdio.h>

#include "utils.h"
#include "elf.h"

void lire_Reimple(FILE* file, elf32_t* elf){
    if (file == NULL || elf == NULL) error("Erreur : fichier ou structure ELF NULL\n");
    int index_Rela = -1, index_rel = -1;
    for (int i = 0; i < elf->header.e_shnum; i++){
        if (elf->sections[i].h_section.sh_type == SHT_RELA){
            index_Rela = i;
            i++;
        }
        else if(elf->sections[i].h_section.sh_type == SHT_REL){
            index_rel = i;
            i++;
        }
        else{
            i++;
        }
    }
    if (index_Rela == -1){
        error(" Pas de RELA section Well Woopie Diddoo");
    }
    if (index_rel == -1){
        error(" Aucune REL table mon Biquet");
    }
    /*OCCUPONS  NOUS DE LA RELA TABLE*/
    Elf32_Shdr RELA_table = elf->sections[index_Rela].h_section;
    int nb_RELA = RELA_table.sh_size / RELA_table.sh_entsize;//nombre d'eleemnts
    elf->RELA_table = malloc(sizeof(Elf32_Rela) * nb_RELA);
    if (!elf->RELA_table) {
        free(elf->RELA_table);
        error("It’s a me, Rela_pas_de_Malloc!");
    }
    fseek(file, RELA_table.sh_offset, SEEK_SET);//debut des RELAAAAhhh
    for (int i = 0; i < nb_RELA; i++) {
        get_32B(&elf->RELA_table[i].r_offset, file);
        get_32B(&elf->RELA_table[i].r_info,   file);
        get_32B((uint32_t *)&elf->RELA_table[i].r_addend, file);
    }//on remplit nos champs
    

    /*Time to think about some Rels*/
    Elf32_Shdr rel_table = elf->sections[index_rel].h_section;    
    
    int nb_rel = rel_table.sh_size / rel_table.sh_entsize;
    elf->rel_table = malloc(sizeof(Elf32_Sym) * nb_rel);
    if (elf->rel_table == NULL){
        free(elf->rel_table);
        error("REL TABLE pas de malloc bon bein Chaos Control! ");
    }
    fseek(file, rel_table.sh_offset, SEEK_SET);//debut des rel
    for (int i = 0; i < nb_rel; i++) {
        get_32B(&elf->rel_table[i].r_offset, file);
        get_32B(&elf->rel_table[i].r_info,   file);
    }//on remplit nos champs

}

void afficher_Reimple(elf32_t* elf){
    int index_Rela = -1, index_rel = -1;
    for (int i = 0; i < elf->header.e_shnum; i++){
        if (elf->sections[i].h_section.sh_type == SHT_RELA){
            index_Rela = i;
            i++;
        }
        else if(elf->sections[i].h_section.sh_type == SHT_REL){
            index_rel = i;
            i++;
        }
    }
    Elf32_Shdr RELA_table = elf->sections[index_Rela].h_section;
    int nb_RELA = RELA_table.sh_size / RELA_table.sh_entsize;//nombre d'eleemnts
    Elf32_Shdr rel_table = elf->sections[index_rel].h_section;    
    int nb_rel = rel_table.sh_size / rel_table.sh_entsize;//same thing brochacho
    if (nb_rel == 0 && nb_RELA == 0 ){
        printf("pas de reimplementation\n");
        return;
    }

    switch (elf->header.e_type) {
        case ET_REL:  
            for (int i = 0; i < nb_rel; i++){
                unsigned char ELF32_type = ELF32_R_TYPE(elf->rel_table[i].r_info);
                int indice =(int) (elf->rel_table[i].r_offset - elf->header.e_shoff);
                printf(" cible :%s  ",elf->section_str_table + elf->sections[indice].h_section.sh_name);
                if  (ELF32_type==0                      || 
                    (2<=ELF32_type && ELF32_type<=11)   ||
                    (24<=ELF32_type && ELF32_type<=26)  ||
                    (28<=ELF32_type && ELF32_type<=31)  ||
                    ELF32_type==38                      ||
                    (40<=ELF32_type && ELF32_type<=99)  ||
                    (102<=ELF32_type && ELF32_type<=111)||
                    (129<=ELF32_type && ELF32_type<=159)){
                    printf("Type : STATIC   ");
                } 
                else{
                    printf("");//faire plus tard;
                }
                printf("Index %c \n",ELF32_R_SYM(elf->rel_table[i].r_info));
            }
            for (int i = 0; i < nb_RELA; i++){

            }
            break;
        case ET_EXEC: 
            printf("ET_EXEC");
        case ET_DYN:
            printf("ET_DYN");
            //plus tard
            break;
        default:      
            break;
    }
}
