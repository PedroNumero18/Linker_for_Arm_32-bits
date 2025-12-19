#include <stdlib.h> 
#include <stdio.h>
#include "utils.h"
#include "elf.h"


void lire_symbole(FILE* file, elf32_t* elf){
    uint8_t info=0;/*pour mieux gérer le cast plus tard*/
    if (file == NULL || elf == NULL){
        fprintf(stderr,"Erreur sur le fichier contenant les symbole\n");
        exit(1);
    }
    int taille_Tsym = (elf->header.e_shnum+elf->header.e_phnum)*2;
    elf->table_symbole = malloc(sizeof(Elf32_Sym)*(taille_Tsym));
    if (elf->table_symbole == NULL){
        fprintf(stderr,"Erreur malloc table des symboles \n");
        exit(1);
    }
    for(int i= 0; i <taille_Tsym; i++){
        get_32B(&(elf->table_symbole)[i].st_name, file);
        get_32B(&(elf->table_symbole)[i].st_value, file);
        get_32B(&(elf->table_symbole)[i].st_size, file);
        get_8B(&info,file);
        (elf->table_symbole)[i].st_info = (unsigned char) info;
        get_8B(&info,file);/*on stocke les 8 bits de Others ici*/
        (elf->table_symbole)[i].st_other = (unsigned char) info;
        get_16B(&(elf->table_symbole)[i].st_shndx, file);
    } 
}

/*
Mon compilateur a bug donc je sais pas si y'a des erreur donc c en comme pour le moment

#include <stdlib.h>
#include <stdio.h>
#include "elf.h"
#include "utils.h"

void lire_symbole(FILE* file, elf32_t* elf){
    if (file == NULL || elf == NULL){
        fprintf(stderr,"Erreur : fichier ou structure ELF NULL\n");
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
    if (elf->table_symbole == NULL){
        fprintf(stderr,"Erreur malloc table des symboles\n");
        exit(1);
    }
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
}


void afficher_symboles(elf32_t* elf){
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



*/