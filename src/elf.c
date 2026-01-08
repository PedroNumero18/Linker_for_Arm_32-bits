#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"

/* ====== Gestion des Structures ====== */

elf32_t* elf_init() {
    elf32_t* elf = malloc(sizeof(elf32_t));
    if (!elf) error("Error allocating memory to pointer\n");
    
    memset(&elf->header, 0, sizeof(Elf32_Ehdr));
    elf->sections = NULL;
    elf->section_str_table = NULL;
    //elf->symbol_str_table = NULL;
    elf->table_symbole = NULL;
    elf->nb_symboles = 0;
    elf->rel_table = NULL;
    elf->nb_rel = 0;
    elf->RELA_table = NULL;
    elf->nb_RELA = 0;
    
    return elf;
}

void elf_free(elf32_t* elf){
    if (!elf) return;
    if (elf->sections) {
        for (int i = 0; i < elf->header.e_shnum; i++) {
            if (elf->sections[i].contenu) free(elf->sections[i].contenu);
        }
        free(elf->sections);
    }
    if (elf->table_symbole) free(elf->table_symbole);
    if (elf->rel_table)     free(elf->rel_table);
    if (elf->RELA_table)    free(elf->RELA_table);
    
    free(elf); 
}

elf32_fusion_sections* fusion_sect_init(){
    elf32_fusion_sections* fusion = malloc(sizeof(elf32_fusion_sections));
    
    fusion->sections = NULL;
    fusion->nb_sections = 0;
    fusion->map_elf2 = NULL;
    fusion->nb_map = 0;
    fusion->shstrtab_orig = NULL;
    fusion->section_str_table = NULL;
    
    return fusion;
}

elf32_fusion_reimpl* fusion_reimpl_init(){
    elf32_fusion_reimpl* fusion = malloc(sizeof(elf32_fusion_reimpl));
    if (!fusion) error("Erreur allocation mémoire pour fusion réimplantation");

    fusion->nb_rel = 0;
    fusion->nb_rel_sections = 0;
    fusion->rel_table = NULL;
    fusion->rel_section_info = NULL;
    return fusion;
}

void free_fusion_reimpl(elf32_fusion_reimpl* fusion) {
    if (fusion) {
        free(fusion->rel_table);
        free(fusion->rel_section_info);
        free(fusion);
    }
}

void afficher_fusion_sections(elf32_fusion_sections* fusion) {
    if (!fusion) {
        printf("Erreur : fusion de sections NULL\n");
        return;
    }
    
    printf("TABLE DES SECTIONS FUSIONNÉES :\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("%-4s %-20s %-12s %-10s %-10s %s\n", 
           "Num", "Nom", "Type", "Taille", "Offset", "Flags");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < fusion->nb_sections; i++) {
        Elf32_Shdr* sh = &fusion->sections[i].h_section;
        
        /* Récupérer le nom de la section */
        const char* nom = "";
        if (sh->sh_name < strlen(fusion->section_str_table)) {
            nom = fusion->section_str_table + sh->sh_name;
        }
        
        /* Type de section */
        const char* type = get_type(sh->sh_type);
        
        /* Flags */
        char flags[10] = "";
        int idx = 0;
        if (sh->sh_flags & SHF_WRITE)     flags[idx++] = 'W';
        if (sh->sh_flags & SHF_ALLOC)     flags[idx++] = 'A';
        if (sh->sh_flags & SHF_EXECINSTR) flags[idx++] = 'X';
        flags[idx] = '\0';
        
        printf("[%2d]  %-20s %-12s %10u 0x%08x %s\n",
               i, nom, type, sh->sh_size, sh->sh_offset, flags);
    }
    
    printf("\n");
    
    /* Afficher la table de remapping ELF2 */
    if (fusion->nb_map > 0) {
        printf("TABLE DE REMAPPING DES SECTIONS (ELF2) :\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("%-15s %-15s %-20s\n", 
               "Ancien index", "Nouvel index", "Offset concat");
        printf("--------------------------------------------------------------------------------\n");
        
        for (int i = 0; i < fusion->nb_map; i++) {
            section_map_t* map = &fusion->map_elf2[i];
            
            /* Trouver le nom de la section */
            const char* nom = "";
            if (map->nouvel_index >= 0 && map->nouvel_index < fusion->nb_sections) {
                Elf32_Shdr* sh = &fusion->sections[map->nouvel_index].h_section;
                if (sh->sh_name < strlen(fusion->section_str_table)) {
                    nom = fusion->section_str_table + sh->sh_name;
                }
            }
            
            printf("      %2d     →     %2d          0x%08x   (%s)\n",
                   map->ancien_index, map->nouvel_index, map->offset, nom);
        }
        printf("\n");
    }
}

void afficher_fusion_symboles(elf32_fusion_symboles* fusion) {
    if (!fusion) {
        printf("Erreur : fusion de symboles NULL\n");
        return;
    }
    
    printf("TABLE DES SYMBOLES FUSIONNÉS :\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("%-4s %-10s %-8s %-8s %-8s %-6s %-20s\n", 
           "Num", "Valeur", "Taille", "Type", "Bind", "Ndx", "Nom");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < fusion->nb_sym; i++) {
        Elf32_Sym* sym = &fusion->table_symbole[i];
        
        /* Nom du symbole */
        const char* nom = "";
        if (sym->st_name < (unsigned int)fusion->strtab_size) {
            nom = fusion->strtab + sym->st_name;
        }
        if (strlen(nom) == 0) {
            nom = "<empty>";
        }
        
        /* Type et bind */
        const char* type = get_type_string(sym->st_info);
        const char* bind = get_bind_string(sym->st_info);
        
        /* Index de section */
        const char* ndx = get_ndx_string(sym->st_shndx);
        
        printf("[%2d]  0x%08x %8u %-8s %-8s %-6s %s\n",
               i, sym->st_value, sym->st_size, type, bind, ndx, nom);
    }
    printf("\n");
}

