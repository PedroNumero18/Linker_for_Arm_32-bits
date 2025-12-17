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
    for(int i= 0; i < elf->header.e_shnum; i++){
       elf->sections[i].contenu = malloc(sizeof(uint8_t) * (elf->sections)[i].h_section.sh_size);
       fseek(file,(elf->sections)[i].h_section.sh_offset,SEEK_SET);
       for (size_t j = 0; j < (elf->sections[i].h_section.sh_size); j++) {
            get_8B(&elf->sections[i].contenu[j],file);
       }
    } 

    elf->section_str_table = (char *)elf->sections[elf->header.e_shstrndx].contenu;
}

const char *get_type(Elf32_Word t) {
    switch (t) {
    case SHT_NULL: return "NULL";
    case SHT_PROGBITS: return "PROGBITS";
    case SHT_SYMTAB: return "SYMTAB";
    case SHT_STRTAB: return "STRTAB";
    case SHT_RELA: return "RELA";
    case SHT_HASH: return "HASH";
    case SHT_DYNAMIC: return "DYNAMIC";
    case SHT_NOTE: return "NOTE";
    case SHT_NOBITS: return "NOBITS";
    case SHT_REL: return "REL";
    case SHT_SHLIB: return "SHLIB";
    case SHT_DYNSYM: return "DYNSYM";
    case SHT_LOPROC: return "LOPROC";
    case SHT_HIPROC: return "HIPROC";
    case SHT_LOUSER: return "LOUSER";
    case SHT_HIUSER: return "HIUSER";
    case SHT_ARM_ATTRIBUTES: return "ARM_ATTRIBUTES";
    default:
        return "OTHER";
    }
}

char *get_flags(uint32_t flags, char buf[32]) {
    int k = 0;

    if (flags & SHF_WRITE)           buf[k++] = 'W';
    if (flags & SHF_ALLOC)           buf[k++] = 'A';
    if (flags & SHF_EXECINSTR)       buf[k++] = 'X';
    if (flags & SHF_MERGE)           buf[k++] = 'M';
    if (flags & SHF_STRINGS)         buf[k++] = 'S';
    if (flags & SHF_INFO_LINK)       buf[k++] = 'I';
    if (flags & SHF_LINK_ORDER)      buf[k++] = 'L';
    if (flags & SHF_OS_NONCONFORMING)buf[k++] = 'O';
    if (flags & SHF_GROUP)           buf[k++] = 'G';
    if (flags & SHF_TLS)             buf[k++] = 'T';
    if (flags & SHF_MASKOS)          buf[k++] = 'o';
    if (flags & SHF_MASKPROC)        buf[k++] = 'p';
    if (flags & SHF_EXCLUDE)         buf[k++] = 'E';
    buf[k] = '\0';      // pas de '0' -> comme readelf

    return buf;
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
        char buff[32];
        printf("  [%2d] %-20s %-18s %08x %06x %06x %02x %3s %2u %2u %2x\n", // c les décalage et les nombre de caractere que je prends pour l'instant
            i,
            elf->section_str_table + section->sh_name,                   // nom (plus tard) (une fonction ? / lire dans la string table ?)
            get_type(section->sh_type),                       // type   (ici faudra une fonction pour remplacer le nombre par le bon nom) changer aussi le format dans le print ducoup %-18u par %18-s
            section->sh_addr,                        // adresse
            section->sh_offset,                      // décalage
            section->sh_size,                        // taille
            section->sh_entsize,                     // taille entrée
            get_flags(section->sh_flags,buff),                       // flags (ici faudra une fonction pour remplacer le nombre par le bon nom) changer aussi le format dans le print ducoup %3u par %3s
            section->sh_link,                        // lien
            section->sh_info,                        // info
            section->sh_addralign                    // alignement
        );
    }
}

