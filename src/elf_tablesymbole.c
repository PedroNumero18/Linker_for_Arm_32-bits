#include <stdlib.h> 
#include <stdio.h>

#include "utils.h"
#include "elf.h"

void lire_symbole(FILE* file, elf32_t* elf){
    if (file == NULL || elf == NULL) error("Erreur : fichier ou structure ELF NULL\n");

    //et la en +
    if (elf->header.e_ident[EI_CLASS] != ELFCLASS32){
        fprintf(stderr,"Erreur : ELF64 non supporté\n");
        exit(1);
    }

    //section SHT_SYMTAB
    int index_symtab = -1;
    for (int i = 0; i < elf->header.e_shnum; i++){
        if (elf->sections[i].h_section.sh_type == SHT_SYMTAB){
            index_symtab = i;
            break;
        }
    }

    if (index_symtab == -1){
        fprintf(stderr,"Aucune table des symboles trouvée\n");
        return;
    }

    Elf32_Shdr symtab = elf->sections[index_symtab].h_section;

    //nombre de symboles 
    int nb_symboles = symtab.sh_size / symtab.sh_entsize;
    
    //Malloc
    elf->table_symbole = malloc(sizeof(Elf32_Sym) * nb_symboles);
    if (elf->table_symbole == NULL) error("Erreur malloc table des symboles\n");
    
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
    //ici le changement
    int index_strtab = symtab.sh_link;
    elf->section_str_table = (char*) elf->sections[index_strtab].contenu;
}

void afficher_symboles(elf32_t* elf){
    printf("\nTable des symboles:\n");
    printf("Num  Value     Size Type    Bind   Ndx Name\n");
    for (int i = 0; elf->table_symbole[i].st_name != 0 || i == 0; i++){
        Elf32_Sym sym = elf->table_symbole[i];

        printf("%3d  %08x %5d %-7d %-6d %3d %s\n",
            i,
            sym.st_value,
            sym.st_size,
            ELF32_ST_TYPE(sym.st_info),
            ELF32_ST_BIND(sym.st_info),
            sym.st_shndx,
            &elf->section_str_table[sym.st_name]
            //ici faudra faire comme pour section et remplacer les nombre par les bon string
        );
    }
}
