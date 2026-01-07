#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "elf.h"

void lire_symbole(FILE* file, elf32_t* elf){
    if (file == NULL || elf == NULL) error("Erreur : fichier ou structure ELF NULL\n");


    if (elf->header.e_ident[EI_CLASS] != ELFCLASS32) error("Erreur : ELF64 non supporté\n");

    //section SHT_SYMTAB
    int index_symtab = -1;
    for (int i = 0; i < elf->header.e_shnum; i++){
        if (elf->sections[i].h_section.sh_type == SHT_SYMTAB){
            index_symtab = i;
            break;
        }
    }

    if (index_symtab == -1) error("Erreur : Aucune table des symboles trouvée\n");

    Elf32_Shdr symtab = elf->sections[index_symtab].h_section;

    //nombre de symboles 
    int nb_symboles = symtab.sh_size / symtab.sh_entsize;
    
    //Malloc
    elf->table_symbole = malloc(sizeof(Elf32_Sym) * nb_symboles);
    if (elf->table_symbole == NULL) error("Erreur malloc table des symboles\n");

    elf->nb_symboles = nb_symboles;
    
    //debut de la table
    fseek(file, symtab.sh_offset, SEEK_SET);

    //lecture
    for (int i = 0; i < nb_symboles; i++){
        get_32B(&elf->table_symbole[i].st_name, file);
        get_32B(&elf->table_symbole[i].st_value, file);
        get_32B(&elf->table_symbole[i].st_size, file);
        get_8B(&elf->table_symbole[i].st_info, file);
        get_8B(&elf->table_symbole[i].st_other, file);
        get_16B(&elf->table_symbole[i].st_shndx, file);
    }
    int index_strtab = symtab.sh_link;
    if (index_strtab < 0 || index_strtab >= elf->header.e_shnum) error("Index .strtab invalide\n");
    
    elf->symbol_str_table = (char*) elf->sections[index_strtab].contenu;
}


const char* get_type_string(unsigned char info){
    switch(ELF32_ST_TYPE(info)){
        case STT_NOTYPE:  return "NOTYPE";
        case STT_OBJECT:  return "OBJECT";
        case STT_FUNC:    return "FUNC";
        case STT_SECTION: return "SECTION";
        case STT_FILE:    return "FILE";
        default:          return "UNKNOWN";
    }
}


const char* get_bind_string(unsigned char info){
    switch(ELF32_ST_BIND(info)){
        case STB_LOCAL:  return "LOCAL";
        case STB_GLOBAL: return "GLOBAL";
        case STB_WEAK:   return "WEAK";
        default:         return "UNKNOWN";
    }
}


const char* get_ndx_string(Elf32_Half shndx){
    static char buffer[16]; 
    if (shndx == 0) return "UND";
    else if (shndx == 0xfff1) return "ABS"; 
    else {
        snprintf(buffer, sizeof(buffer), "%d", shndx);
        return buffer;
    }
}



void afficher_symboles(elf32_t* elf){
    printf("\nTable des symboles:\n");
    printf("Num  Value     Size Type    Bind   Ndx Name\n");
    for (uint32_t i = 0; i < elf->nb_symboles; i++){
        Elf32_Sym sym = elf->table_symbole[i];

        printf("%3d  %08x %5d %-7s %-6s %3s %s\n",
            i,
            sym.st_value,
            sym.st_size,
            get_type_string(sym.st_info),
            get_bind_string(sym.st_info),
            get_ndx_string(sym.st_shndx),
            &elf->section_str_table[sym.st_name]

        );
    }
}

//FUSSSSIONN AHHHHHHHHH
/*Fonction pour avoir la taille des strtab*/
int get_symtab_strtab_size(elf32_t *elf, int symtab_index) {
    if (!elf || symtab_index < 0 || symtab_index >= elf->header.e_shnum) {
        return -1;
    }

    Elf32_Shdr *symtab_sh = &elf->sections[symtab_index].h_section;

    /* sh_link de .symtab donne l’index de la section .strtab utilisée par cette symtab */
    int strtab_index = symtab_sh->sh_link;
    if (strtab_index < 0 || strtab_index >= elf->header.e_shnum) {
        return -1;
    }

    Elf32_Shdr *strtab_sh = &elf->sections[strtab_index].h_section;

    /* La taille de la strtab = sh_size */
    return (int)strtab_sh->sh_size;
}


elf32_fusion_symboles* fusion_symboles(elf32_t* elf1, elf32_t* elf2){
    elf32_fusion_symboles *fusion = malloc(sizeof(elf32_fusion_symboles));
    fusion->table_symbole = malloc(sizeof(Elf32_Sym)* (int)(elf1->nb_symboles + elf2->nb_symboles));
    fusion->sym_map_elf1 = malloc(sizeof(int) * elf1->nb_symboles);
    fusion->sym_map_elf2 = malloc(sizeof(int) * elf2->nb_symboles);
    int index_symtab_elf1 = -1;
    int index_symtab_elf2 = -1;

    //on recherche les indices des deux symtab(table de symboles)
    for (int i = 0; i < elf1->header.e_shnum; i++){
        if (elf1->sections[i].h_section.sh_type == SHT_SYMTAB){
            index_symtab_elf1 = i;
            i=elf1->header.e_shnum;
        }
    }
    for (int i = 0; i < elf2->header.e_shnum; i++){
        if (elf2->sections[i].h_section.sh_type == SHT_SYMTAB){
            index_symtab_elf2 = i;
            i=elf2->header.e_shnum;
        }
    }
    int taille_strtab_elf1 = get_symtab_strtab_size(elf1, index_symtab_elf1);
    int taille_strtab_elf2 = get_symtab_strtab_size(elf2, index_symtab_elf2);
    fusion->strtab = malloc(taille_strtab_elf1+taille_strtab_elf2);
    fusion->nb_sym       = 0;              
    fusion->strtab_size  = 0;  

    /*Section elf1 symbole locall*/
    for (int i = 0; i < (int)elf1->nb_symboles; i++) {
        if (ELF32_ST_BIND( elf1->table_symbole[i].st_info) == STB_LOCAL){
            fusion->table_symbole[i] = elf1->table_symbole[i];
            fusion->sym_map_elf1[i] = i;
            fusion->nb_sym++;
            //On ajoute notre symbole dans la strtab resultat
            char *nom_symbole = &elf1->section_str_table[(elf1->table_symbole[i]).st_name];
            int longueur = strlen(nom_symbole) + 1;
            memcpy(fusion->strtab + fusion->strtab_size, nom_symbole, longueur);
            fusion->strtab_size += longueur;
        }
    }
    /*section 2 symbole locall*/
    for (int i = 0; i < (int)elf2->nb_symboles; i++) {
        if (ELF32_ST_BIND(elf2->table_symbole[i].st_info) == STB_LOCAL){
            fusion->table_symbole[fusion->nb_sym] =  elf2->table_symbole[i];
            fusion->sym_map_elf2[i] = fusion->nb_sym;
            fusion->nb_sym++;
            (elf2->table_symbole[i]).st_name += taille_strtab_elf1 ;
            char *nom_symbole = &elf2->section_str_table[(elf2->table_symbole[i]).st_name];
            int longueur = strlen(nom_symbole) + 1;
            memcpy(fusion->strtab + fusion->strtab_size, nom_symbole, longueur);
            fusion->strtab_size += longueur;
        }
    }

    /*La fusion des Globaux*/
    Symboles_globaux* globaux = NULL;
    int nb_globaux = 0;
    for (int i = 0; i < (int)elf1->nb_symboles; i++) {
        if (ELF32_ST_BIND(elf1->table_symbole[i].st_info) == STB_GLOBAL) {
            char *nom_symbole = &elf1->section_str_table[(elf1->table_symbole[i]).st_name];
            int longueur = strlen(nom_symbole) + 1;
            memcpy(fusion->strtab + fusion->strtab_size, nom_symbole, longueur);
            fusion->strtab_size += longueur;
            globaux = realloc(globaux, (nb_globaux+1)*sizeof(Symboles_globaux));
            globaux[nb_globaux].nom = &fusion->strtab[fusion->strtab_size];
            globaux[nb_globaux].sym1 = &elf1->table_symbole[i];
            globaux[nb_globaux].sym2 = NULL;
            nb_globaux++;
        }    
    }

    //ajout/completion des symboles de elf2
    for (int i = 0; i < (int)elf2->nb_symboles; i++) {
        if (ELF32_ST_BIND(elf2->table_symbole[i].st_info) == STB_GLOBAL) {
            char *nom_symbole = &elf2->section_str_table[(elf2->table_symbole[i]).st_name];

            // Chercher si déjà dans elf1
            int est_present = 0;
            int j = 0;
            while(j < nb_globaux && est_present==0){
                if (strcmp(nom_symbole, globaux[j].nom) == 0) {
                globaux[j].sym2 = &elf2->table_symbole[i];
                est_present = 1;
                }
                j++;
            }

            //cas ou il est absent
            if(!est_present){
                char *nom_symbole = &elf2->section_str_table[(elf2->table_symbole[i]).st_name];
                int longueur = strlen(nom_symbole) + 1;
                memcpy(fusion->strtab + fusion->strtab_size, nom_symbole, longueur);
                fusion->strtab_size += longueur;
                globaux = realloc(globaux, (nb_globaux+1)*sizeof(Symboles_globaux));
                globaux[nb_globaux].nom = &fusion->strtab[fusion->strtab_size];
                globaux[nb_globaux].sym2 = &elf2->table_symbole[i];
                globaux[nb_globaux].sym1 = NULL;
                nb_globaux++;
            }
        }
    }

    //on fait le tri dans notre fusion
    for (int i = 0; i < nb_globaux; i++) {
        Elf32_Sym* symbole_elf1 = globaux[i].sym1;
        Elf32_Sym* symbole_elf2 = globaux[i].sym2;    
        Elf32_Sym* a_copier = NULL;
        
        // RÈGLE 1 : 2 définis → ERREUR
        if ((symbole_elf1 && symbole_elf1->st_shndx != STN_UNDEF) && 
        (symbole_elf2 && symbole_elf2->st_shndx != STN_UNDEF)) {
            printf("%s", globaux[i].nom);
            error("symbole défini dans les 2 fichiers\n");
        }
        // RÈGLE 2 : 1 défini → le prendre
        else if ((symbole_elf1 && symbole_elf1->st_shndx != STN_UNDEF)) {
            a_copier = symbole_elf1;
        }
        else if ((symbole_elf2 && symbole_elf2->st_shndx != STN_UNDEF)) {
            a_copier = symbole_elf2;
        }
        // RÈGLE 3 : 2 indéfinis → 1 seul
        else if (!(symbole_elf2 && symbole_elf2->st_shndx != STN_UNDEF) && 
            !(symbole_elf1 && symbole_elf1->st_shndx != STN_UNDEF) && 
            symbole_elf1 && 
            symbole_elf2) {
            a_copier = symbole_elf1;  // arbitraire : prendre elf1
        }
        // RÈGLE 4 : 1 seul → le prendre
        else if (symbole_elf1) {
            a_copier = symbole_elf1;
        }
        else if (symbole_elf2) {
            a_copier = symbole_elf2;
        }
        
        // Copier avec st_name correct
        if (a_copier) {
            fusion->table_symbole[fusion->nb_sym] =  *a_copier;;
            globaux[i].index_fusion = fusion->nb_sym;
            fusion->nb_sym++;
        }
    }
    free(globaux);
    return fusion;
}
 