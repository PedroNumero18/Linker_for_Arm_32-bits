#include <stdlib.h> 
#include <stdio.h>

#include "utils.h"
#include "elf.h"

const char* get_rel_type(uint32_t r_info){
    unsigned char type = ELF32_R_TYPE(r_info);
    switch(type){
        case 0:         return "R_ARM_NONE";
        case 2:         return "R_ARM_ABS32";
        case 3:         return "R_ARM_REL32";
        case 4:         return "R_ARM_LDR_PC_G0";
        case 5 ... 8:   return "R_ARM_ABS";
        case 9:         return "R_ARM_SBREL32";
        case 10:        return "R_ARM_THM_CALL";
        case 11:        return "R_ARM_THM_PC8";
        case 24:        return "R_ARM_GOTOFF32";
        case 25:        return "R_ARM_BASE_PREL";
        case 26:        return "R_ARM_GOT_BREL";
        case 28:        return "R_ARM_CALL";
        
        case 29:            
        case 30:        return "R_ARM_JUMP24";        
        
        case 31:        return "R_ARM_BASE_ABS";
        case 38:        return "R_ARM_TARGET1";
        case 40:        return "R_ARM_V4BX";
        case 41:        return "R_ARM_TARGET2";
        case 42:        return "R_ARM_PREL31";

        case 47:
        case 43:        return "R_ARM_MOVW_ABS_NC";
        case 48:
        case 44:        return "R_ARM_MOVT_ABS";
        case 49:
        case 45:        return "R_ARM_MOVW_PREL_NC";
        case 50:
        case 46:        return "R_ARM_MOVT_PREL";
        default:        return "UNKNOWN";
    }
}

uint32_t get_rel_sym(uint32_t r_info){
    return ELF32_R_SYM(r_info);
}


void lire_Reimple(FILE* file, elf32_t* elf){
    if (file == NULL || elf == NULL) error("Erreur : fichier ou structure ELF NULL\n");

    //recherche section rel et rela
    int index_Rela = -1, index_rel = -1;

    for (int i = 0; i < elf->header.e_shnum; i++){
        uint32_t type = elf->sections[i].h_section.sh_type;
        if (type == SHT_REL)   index_rel = i;
        if (type == SHT_RELA)  index_Rela = i;
    }

    //init
    elf->rel_table = NULL;
    elf->RELA_table = NULL;
    elf->nb_rel = 0;
    elf->nb_RELA = 0;

    //rela
    if (index_Rela != -1){
        Elf32_Shdr s = elf->sections[index_Rela].h_section;
        int nb = s.sh_size / s.sh_entsize;
        if (nb > 0) {
            elf->RELA_table = malloc(sizeof(Elf32_Rela) * nb);
            elf->nb_RELA = 0;
            if (!elf->RELA_table) error("Impossible d'allouer RELA\n");
            fseek(file, s.sh_offset, SEEK_SET);
            for (int i = 0; i < nb; i++){
                get_32B(&elf->RELA_table[i].r_offset, file);
                get_32B(&elf->RELA_table[i].r_info,   file);
                get_32B((uint32_t*)&elf->RELA_table[i].r_addend, file);
            }
            elf->nb_RELA = nb;
        }
    }

    //rel
    if (index_rel != -1){
        Elf32_Shdr s = elf->sections[index_rel].h_section;
        int nb = s.sh_size / s.sh_entsize;
        if (nb > 0) {
            elf->rel_table = malloc(sizeof(Elf32_Rel) * nb);
            if (!elf->rel_table) error("Impossible d'allouer REL\n");
            fseek(file, s.sh_offset, SEEK_SET);
            for (int i = 0; i < nb; i++){
                get_32B(&elf->rel_table[i].r_offset, file);
                get_32B(&elf->rel_table[i].r_info, file);
            }
            elf->nb_rel = nb;
        }
    }
}


void afficher_Reimple(elf32_t* elf){
    if (!elf) return;

    //rela
    if (elf->RELA_table && elf->nb_RELA > 0){
        printf("\nRelocation section '.rela.text' contains %u entries:\n", elf->nb_RELA);
        printf(" Offset     Info    Type            Sym.Value  Sym.Name + Addend\n");
        for (uint32_t i = 0; i < elf->nb_RELA; i++){
            Elf32_Rela r = elf->RELA_table[i];
            uint32_t sym_idx = get_rel_sym(r.r_info);
            const char* sym_name = (sym_idx < elf->nb_symboles) ? 
                &elf->symbol_str_table[elf->table_symbole[sym_idx].st_name] : "?";
            printf("%08x  %08x %-15s %08x   %s + %d\n",
                r.r_offset,              
                r.r_info,                
                get_rel_type(r.r_info),  
                elf->table_symbole[sym_idx].st_value,
                sym_name,
                r.r_addend
            );
        }
    } else if (!elf->RELA_table || elf->nb_RELA == 0) {
        printf("\nThere are no relocations in this file.\n");
    }

    //rel
    if (elf->rel_table && elf->nb_rel > 0){
        printf("\nRelocation section '.rel.text' contains %u entries:\n", elf->nb_rel);
        printf(" Offset     Info    Type            Sym.Value  Sym.Name\n");
        for (uint32_t i = 0; i < elf->nb_rel; i++){
            Elf32_Rel r = elf->rel_table[i];
            uint32_t sym_idx = get_rel_sym(r.r_info);
            const char* sym_name = (sym_idx < elf->nb_symboles) ? 
                &elf->symbol_str_table[elf->table_symbole[sym_idx].st_name] : "?";
            printf("%08x  %08x %-15s %08x   %s\n",
                r.r_offset,              
                r.r_info,                
                get_rel_type(r.r_info),  
                elf->table_symbole[sym_idx].st_value,
                sym_name
            );
        }
    }
}
