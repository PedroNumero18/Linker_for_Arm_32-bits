#include <stdlib.h> 
#include <stdio.h>

#include "utils.h"
#include "elf.h"

const char* get_rel_type_name(R_ARM_Type r_type) {
    switch(r_type) {
        case R_ARM_NONE:            return "R_ARM_NONE";
        case R_ARM_ABS32:           return "R_ARM_ABS32";
        case R_ARM_REL32:           return "R_ARM_REL32";
        case R_ARM_LDR_PC_G0:       return "R_ARM_LDR_PC_G0";
        case R_ARM_ABS16:           return "R_ARM_ABS16";
        case R_ARM_ABS12:           return "R_ARM_ABS12";
        case R_ARM_THM_ABS5:        return "R_ARM_THM_ABS5";
        case R_ARM_ABS8:            return "R_ARM_ABS8";
        case R_ARM_SBREL32:         return "R_ARM_SBREL32";
        case R_ARM_THM_CALL:        return "R_ARM_THM_CALL";
        case R_ARM_THM_PC8:         return "R_ARM_THM_PC8";
        case R_ARM_GOTOFF32:        return "R_ARM_GOTOFF32";
        case R_ARM_BASE_PREL:       return "R_ARM_BASE_PREL";
        case R_ARM_GOT_BREL:        return "R_ARM_GOT_BREL";
        case R_ARM_CALL:            return "R_ARM_CALL";
        case R_ARM_JUMP24:          return "R_ARM_JUMP24";
        case R_ARM_THM_JUMP24:      return "R_ARM_THM_JUMP24";
        case R_ARM_BASE_ABS:        return "R_ARM_BASE_ABS";
        case R_ARM_TARGET1:         return "R_ARM_TARGET1";
        case R_ARM_V4BX:            return "R_ARM_V4BX";
        case R_ARM_TARGET2:         return "R_ARM_TARGET2";
        case R_ARM_PREL31:          return "R_ARM_PREL31";
        case R_ARM_MOVW_ABS_NC:     return "R_ARM_MOVW_ABS_NC";
        case R_ARM_MOVT_ABS:        return "R_ARM_MOVT_ABS";
        case R_ARM_MOVW_PREL_NC:    return "R_ARM_MOVW_PREL_NC";
        case R_ARM_MOVT_PREL:       return "R_ARM_MOVT_PREL";
        case R_ARM_THM_MOVW_ABS_NC: return "R_ARM_THM_MOVW_ABS_NC";
        case R_ARM_THM_MOVT_ABS:    return "R_ARM_THM_MOVT_ABS";
        case R_ARM_THM_MOVW_PREL_NC:return "R_ARM_THM_MOVW_PREL_NC";
        case R_ARM_THM_MOVT_PREL:   return "R_ARM_THM_MOVT_PREL";
        default:                    return "UNKNOWN";
    }
}

const char* get_rel_type(uint32_t r_info) {
    R_ARM_Type r_type = (R_ARM_Type)ELF32_R_TYPE(r_info);
    return get_rel_type_name(r_type);
}

uint32_t get_rel_sym(uint32_t r_info){
    return ELF32_R_SYM(r_info);
}

uint32_t lire_contenu_32bits(elf32_sections* sect, uint32_t offset) {
    if (offset + 4 > sect->h_section.sh_size) {
        error("Offset hors limites de la section");
    }
    
    uint32_t valeur = 0;
    memcpy(&valeur, sect->contenu + offset, 4);
    
    /* Conversion big-endian si nécessaire */
    if (!is_big_endian()) {
        valeur = reverse_4(valeur);
    }
    
    return valeur;
}

void ecrire_contenu_32bits(elf32_sections* sect, uint32_t offset, uint32_t valeur) {
    if (offset + 4 > sect->h_section.sh_size) {
        error("Offset hors limites de la section");
    }
    
    /* Conversion big-endian si nécessaire */
    if (!is_big_endian()) {
        valeur = reverse_4(valeur);
    }
    
    memcpy(sect->contenu + offset, &valeur, 4);
}

void lire_Reimple(FILE* file, elf32_t* elf){
    if (file == NULL || elf == NULL) error("Erreur : fichier ou structure ELF NULL\n");

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

void corriger_addend_section(elf32_sections* sect_fusion, uint32_t r_offset, R_ARM_Type r_type, uint32_t offset_section_def) {
    uint32_t valeur = lire_contenu_32bits(sect_fusion, r_offset);
    
    switch(r_type) {
        /* Adresses absolues 32 bits */
        case R_ARM_ABS32:
        case R_ARM_TARGET1:     
        case R_ARM_TARGET2:      
            valeur += offset_section_def;
            ecrire_contenu_32bits(sect_fusion, r_offset, valeur);
            break;
        
        /* Adresses absolues 16 bits */    
        case R_ARM_ABS16:
        case R_ARM_ABS12:
            valeur = (valeur & 0xFFFF0000) | ((valeur + offset_section_def) & 0xFFFF);
            ecrire_contenu_32bits(sect_fusion, r_offset, valeur);
            break;
            
        /* Adresses absolues 8 bits */
        case R_ARM_ABS8:
        case R_ARM_THM_ABS5:
            valeur = (valeur & 0xFFFFFF00) | ((valeur + offset_section_def) & 0xFF);
            ecrire_contenu_32bits(sect_fusion, r_offset, valeur);
            break;
        
        /* Offsets relatifs 32 bits */
        case R_ARM_REL32:
        case R_ARM_SBREL32:
        case R_ARM_GOTOFF32:
        case R_ARM_BASE_PREL:
        case R_ARM_GOT_BREL:
            valeur += offset_section_def;
            ecrire_contenu_32bits(sect_fusion, r_offset, valeur);
            break;
        
        /* Offset relatif 31 bits (bit 31 = 0) */
        case R_ARM_PREL31:
            {
                int32_t offset_val = valeur & 0x7FFFFFFF;
                /* Extension de signe si nécessaire */
                if (offset_val & 0x40000000) {
                    offset_val |= 0x80000000;
                }
                offset_val += offset_section_def;
                valeur = (valeur & 0x80000000) | (offset_val & 0x7FFFFFFF);
                ecrire_contenu_32bits(sect_fusion, r_offset, valeur);
            }
            break;
            
        /* Branchements ARM 24 bits */
        case R_ARM_JUMP24:
        case R_ARM_CALL:
            {
                /* Extraire l'offset signé sur 24 bits */
                int32_t offset_instr = valeur & 0x00FFFFFF;
                
                /* Extension de signe sur 32 bits */
                if (offset_instr & 0x00800000) {
                    offset_instr |= 0xFF000000;
                }
                
                offset_instr <<= 2;
                offset_instr += offset_section_def;
                offset_instr >>= 2;
                offset_instr &= 0x00FFFFFF;
                
                /* Remettre dans l'instruction (conserver les 8 bits de poids fort) */
                valeur = (valeur & 0xFF000000) | offset_instr;
                ecrire_contenu_32bits(sect_fusion, r_offset, valeur);
            }
            break;
        
        /* Instructions MOVW/MOVT - 16 bits répartis dans l'instruction */
        case R_ARM_MOVW_ABS_NC:   /* MOVW pour bits 0-15 d'une adresse */
        case R_ARM_THM_MOVW_ABS_NC:
            {
                uint32_t imm = ((valeur >> 4) & 0xF000) | (valeur & 0xFFF);
                imm += offset_section_def;
                imm &= 0xFFFF;
                valeur = (valeur & 0xFFF0F000) | ((imm & 0xF000) << 4) | (imm & 0xFFF);
                ecrire_contenu_32bits(sect_fusion, r_offset, valeur);
            }
            break;
            
        case R_ARM_MOVT_ABS:      /* MOVT pour bits 16-31 d'une adresse */
        case R_ARM_THM_MOVT_ABS:
            {
                uint32_t imm = ((valeur >> 4) & 0xF000) | (valeur & 0xFFF);
                uint32_t high_offset = (offset_section_def >> 16) & 0xFFFF;
                imm += high_offset;
                imm &= 0xFFFF;
                valeur = (valeur & 0xFFF0F000) | ((imm & 0xF000) << 4) | (imm & 0xFFF);
                ecrire_contenu_32bits(sect_fusion, r_offset, valeur);
            }
            break;
        
        case R_ARM_MOVW_PREL_NC: 
        case R_ARM_THM_MOVW_PREL_NC:
        case R_ARM_MOVT_PREL:     /*MOVW et MOVT relatif au PC */
        case R_ARM_THM_MOVT_PREL:
            {
                uint32_t imm = ((valeur >> 4) & 0xF000) | (valeur & 0xFFF);
                imm += offset_section_def;
                imm &= 0xFFFF;
                valeur = (valeur & 0xFFF0F000) | ((imm & 0xF000) << 4) | (imm & 0xFFF);
                ecrire_contenu_32bits(sect_fusion, r_offset, valeur);
            }
            break;
        
        case R_ARM_THM_JUMP24:
        case R_ARM_THM_CALL:
        case R_ARM_THM_PC8:
        case R_ARM_V4BX:
        case R_ARM_NONE:
            break;
        
        case R_ARM_LDR_PC_G0:
        case R_ARM_BASE_ABS:
            /* Types spécifiques, correction similaire aux absolues */
            valeur += offset_section_def;
            ecrire_contenu_32bits(sect_fusion, r_offset, valeur);
            break;

        default:
            /* Type inconnu ou non supporté - afficher un avertissement */
            fprintf(stderr, "Avertissement: Type de réimplantation %s (%d) non géré "
                           "pour correction d'addend SECTION\n", 
                           get_rel_type_name(r_type), r_type);
            break;
    }
}

