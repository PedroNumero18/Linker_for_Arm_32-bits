#include <stdlib.h> 
#include <stdio.h>

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
    
    elf->section_str_table = (char*) elf->sections[index_strtab].contenu;
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
