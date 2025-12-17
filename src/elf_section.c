#include <stdlib.h> 
#include <stdio.h>
#include "utils.h"
#include "elf.h"



void lire_sections(FILE* file, elf32_t* elf) {

   if (file == NULL || elf == NULL){
        fprintf(stderr,"Erreur fichier ou d'initialisation d'elf\n");
        exit(1);
    }
    fseek(file,elf->header.e_shoff,SEEK_SET);
    elf->sections = malloc(sizeof(elf32_sections)*(elf->header.e_shnum));
    if (elf->sections == NULL){
        fprintf(stderr,"Erreur malloc sections\n");
        exit(1);
    }
    for(int i= 0; i <elf->header.e_shnum; i++){
        get_32B(&(elf->sections)[i].h_section.sh_name, file);
        get_32B(&(elf->sections)[i].h_section.sh_type, file);
        get_32B(&(elf->sections)[i].h_section.sh_flags, file);
        get_32B(&(elf->sections)[i].h_section.sh_addr, file);
        get_32B(&(elf->sections)[i].h_section.sh_offset, file);
        get_32B(&(elf->sections)[i].h_section.sh_size, file);
        get_32B(&(elf->sections)[i].h_section.sh_link, file);
        get_32B(&(elf->sections)[i].h_section.sh_info, file);
        get_32B(&(elf->sections)[i].h_section.sh_addralign, file);
        get_32B(&(elf->sections)[i].h_section.sh_entsize, file);
    }
}


void afficher_sections(const elf32_t* elf) {
    if (elf == NULL || (elf->sections) == NULL) {
        fprintf(stderr, "Sections non chargées\n");
        return;
    }

    printf("Il y a %d en-têtes de section, débutant à l'adresse de décalage 0x%x:\n\n",elf->header.e_shnum, elf->header.e_shoff);
    printf("En-têtes de section :\n");
    printf("  [Nr] Nom                  Type               Adr      Décala Taille ES Fan LN Inf Al\n");

    for (int i = 0; i < elf->header.e_shnum; i++) {
        const Elf32_Shdr* section = &elf->sections[i].h_section;

        printf("  [%2d] %-20s %-18u %08x %06x %06x %02x %3u %2u %2u %2x\n", // c les décalage et les nombre de caractere que je prends pour l'instant
            i,
            /* section_nom */ "",                   // nom (plus tard) (une fonction ? / lire dans la string table ?)
            section->sh_type,                       // type   (ici faudra une fonction pour remplacer le nombre par le bon nom) changer aussi le format dans le print ducoup %-18u par %18-s
            section->sh_addr,                        // adresse
            section->sh_offset,                      // décalage
            section->sh_size,                        // taille
            section->sh_entsize,                     // taille entrée
            section->sh_flags,                       // flags (ici faudra une fonction pour remplacer le nombre par le bon nom) changer aussi le format dans le print ducoup %3u par %3s
            section->sh_link,                        // lien
            section->sh_info,                        // info
            section->sh_addralign                    // alignement
        );
    }
}

