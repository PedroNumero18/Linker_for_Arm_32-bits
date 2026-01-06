#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "elf.h"


const char *get_type(Elf32_Word t) {
    switch (t) {
    case SHT_NULL:     return "NULL";
    case SHT_PROGBITS: return "PROGBITS";
    case SHT_SYMTAB:   return "SYMTAB";
    case SHT_STRTAB:   return "STRTAB";
    case SHT_RELA:     return "RELA";
    case SHT_HASH:     return "HASH";
    case SHT_DYNAMIC:  return "DYNAMIC";
    case SHT_NOTE:     return "NOTE";
    case SHT_NOBITS:   return "NOBITS";
    case SHT_REL:      return "REL";
    case SHT_SHLIB:    return "SHLIB";
    case SHT_DYNSYM:   return "DYNSYM";
    case SHT_LOPROC:   return "LOPROC";
    case SHT_HIPROC:   return "HIPROC";
    case SHT_LOUSER:   return "LOUSER";
    case SHT_HIUSER:   return "HIUSER";
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

int is_numerical(char *str) {
    if (!str || *str == '\0') return 0;
    while (*str) {
        if (!isdigit((unsigned char) *str)) return 0;
        str++;
    }
    return 1;
}

void lire_sections(FILE* file, elf32_t* elf) {
    if (file == NULL || elf == NULL) error("Erreur fichier ou d'initialisation d'elf\n");  

    fseek(file,elf->header.e_shoff,SEEK_SET);
    elf->sections = malloc(sizeof(elf32_sections)*(elf->header.e_shnum));
    
    if (elf->sections == NULL) error("Erreur malloc sections\n");  
    
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

void afficher_sections(const elf32_t* elf) {
    if (elf == NULL || (elf->sections) == NULL) error( "Sections non chargées\n");

    printf("Il y a %d en-têtes de section, débutant à l'adresse de décalage 0x%x:\n\n",elf->header.e_shnum, elf->header.e_shoff);
    printf("Section Header:\n");
    printf("  [Nr] Nom                  Type               Adr      Décala Taille ES Fan LN Inf Al\n");

    for (int i = 0; i < elf->header.e_shnum; i++) {
        const Elf32_Shdr* section = &elf->sections[i].h_section;
        char buff[32];
        printf("  [%2d] %-20s %-18s %08x %06x %06x %02x %3s %2u %2u %2x\n", // ce sont les décalages et les nombres de caractère que je prends pour l'instant
            i,
            elf->section_str_table + section->sh_name, // nom (plus tard) (une fonction ? / lire dans la string table ?)
            get_type(section->sh_type),              // type   (ici faudra une fonction pour remplacer le nombre par le bon nom) changer aussi le format dans le print ducoup %-18u par %18-s
            section->sh_addr,                        // adresse
            section->sh_offset,                      // décalage
            section->sh_size,                        // taille
            section->sh_entsize,                     // taille entrée
            get_flags(section->sh_flags,buff),       // flags (ici il faudra une fonction pour remplacer le nombre par le bon nom) changer aussi le format dans le print ducoup %3u par %3s
            section->sh_link,                        // lien
            section->sh_info,                        // info
            section->sh_addralign                    // alignement
        );
    }
    printf("\n");
}

void lire_contenu_sect( FILE* f, elf32_t *elf, int index) {
    // Récupération du pointeur vers la section spécifique dans votre tableau
    elf32_sections *section = &elf->sections[index];

    // On verifie si la section est vide ou virtuelle (comme .bss)
    // SHT_NOBITS = 8. On n'a rien à lire pour ces sections.
    if (section->h_section.sh_type == SHT_NOBITS || section->h_section.sh_size == 0) {
        section->contenu = NULL;
        return;
    }

    // Allocation de la mémoire pour le contenu
    section->contenu = malloc(sizeof(uint8_t) * section->h_section.sh_size);
    if (section->contenu == NULL) {
        error("Erreur d'allocation mémoire");
    }

    // On se positionne au bon endroit dans le fichier
    // On utilise 'h_section.sh_offset'
    if (fseek(f, section->h_section.sh_offset, SEEK_SET) != 0) {
        error("Erreur de fseek");
    }

    // On fait la lecture des données brutes
    fread(section->contenu, 1, section->h_section.sh_size, f);
}

int get_section_ind_par_nom(const elf32_t *elf, const char *name) {
    if (elf == NULL || name == NULL) return -1;
    for (int i = 0; i < elf->header.e_shnum; i++) {
        const char *section_name =
            elf->section_str_table + elf->sections[i].h_section.sh_name;

        if (strcmp(section_name, name) == 0) {
            return i;
        }
    }
    return -1; //pas de section
}


void afficher_contenu_section(elf32_t *elf, char *param){
    int index_section = -1;

    //Trouver l'index de la section 
    if (is_numerical(param)) index_section = atoi(param);
    else {
        for (int i = 0; i < elf->header.e_shnum; i++) {
            //Calcul du nom : table des strings + offset du nom de la section
            char *nom_courant = elf->section_str_table + elf->sections[i].h_section.sh_name;
            if (strcmp(nom_courant, param) == 0) {
                index_section = i;
                break;
            }
        }
    }

    if (index_section < 0 || index_section >= elf->header.e_shnum) {
        printf("Erreur : Section '%s' introuvable.\n", param);
        return;
    }

    // Affichage
    elf32_sections *sect = &elf->sections[index_section];
    
    // Si c'est une section vide (.bss)
    if (sect->contenu == NULL) {
        printf("Section %d sans contenu.\n", index_section);
        return;
    }
    printf("Contenu de la section '%s' (Index %d) :\n", param, index_section);
    
    for (Elf32_Word i = 0; i < sect->h_section.sh_size; i += 16) {

        /* Adresse */
        printf("  0x%08x ", i);

        /* Partie HEX */
        for (int j = 0; j < 16; j++) {
            if (i + j < sect->h_section.sh_size)
                printf("%02x", sect->contenu[i + j]);
            else
                printf("  ");

            if ((j + 1) % 4 == 0) printf(" ");
        }

        /* Partie ASCII */
        printf(" ");

        for (int j = 0; j < 16 && i + j < sect->h_section.sh_size; j++) {
            unsigned char c = sect->contenu[i + j];
            if (isprint(c))
                printf("%c", c);
            else
                printf(".");
        }

        printf("\n");
    }
}


elf32_fusion_sections* fusion_sections(elf32_t* elf1, elf32_t* elf2) {
    elf32_fusion_sections* fusion = malloc(sizeof(elf32_fusion_sections));
    if (!fusion) error("Erreur malloc fusion");
    int max_sections = elf1->header.e_shnum + elf2->header.e_shnum;
    fusion->sections = malloc(sizeof(elf32_sections) * max_sections);
    if (!fusion->sections) error("Erreur malloc sections fusion");
    int idx_fusion = 0;
    //section elf1
    for (int i = 0; i < elf1->header.e_shnum; i++) {
        fusion->sections[idx_fusion].h_section =
            elf1->sections[i].h_section;

        if (elf1->sections[i].h_section.sh_size > 0 &&
            elf1->sections[i].contenu != NULL) {

            fusion->sections[idx_fusion].contenu =malloc(elf1->sections[i].h_section.sh_size);
            memcpy(
                fusion->sections[idx_fusion].contenu,
                elf1->sections[i].contenu,
                elf1->sections[i].h_section.sh_size
            );
        } else {
            fusion->sections[idx_fusion].contenu = NULL;
        }
        idx_fusion++;
    }
    //section elf2
    for (int j = 0; j < elf2->header.e_shnum; j++) {
        int fusionnee = 0;
        const char *name2 = elf2->section_str_table + elf2->sections[j].h_section.sh_name;

        for (int i = 0; i < elf1->header.e_shnum; i++) {
            const char *name1 = elf1->section_str_table + elf1->sections[i].h_section.sh_name;
            if (strcmp(name1, name2) == 0 &&
                elf1->sections[i].h_section.sh_type == SHT_PROGBITS &&
                elf2->sections[j].h_section.sh_type == SHT_PROGBITS) {
                size_t old_size = fusion->sections[i].h_section.sh_size;
                size_t add_size = elf2->sections[j].h_section.sh_size;

                fusion->sections[i].contenu = realloc(
                    fusion->sections[i].contenu,
                    old_size + add_size
                );
                memcpy(
                    fusion->sections[i].contenu + old_size,
                    elf2->sections[j].contenu,
                    add_size
                );
                fusion->sections[i].h_section.sh_size = old_size + add_size;
                fusionnee = 1;
                break;
            }
        }


        //ajout

        if (!fusionnee) {
            fusion->sections[idx_fusion].h_section =
                elf2->sections[j].h_section;

            if (elf2->sections[j].h_section.sh_size > 0 &&
                elf2->sections[j].contenu != NULL) {

                fusion->sections[idx_fusion].contenu =
                    malloc(elf2->sections[j].h_section.sh_size);

                memcpy(
                    fusion->sections[idx_fusion].contenu,
                    elf2->sections[j].contenu,
                    elf2->sections[j].h_section.sh_size
                );
            } else {
                fusion->sections[idx_fusion].contenu = NULL;
            }

            idx_fusion++;
        }
    }

    fusion->nb_sections = idx_fusion;
    return fusion;
}
/*
elf32_fusion_sections* fusion_sections(elf32_t* elf1, elf32_t* elf2){
    elf32_fusion_sections* fusion = malloc(sizeof(elf32_fusion_sections));
    if(!fusion) error("fusion malloc error");
    fusion->nb_sections = (int)((elf1->header).e_shnum + (elf2->header).e_shnum);
    int nbre_potalas= 0;
    fusion->sections = malloc(sizeof(elf32_sections)*fusion->nb_sections);
    for(int i=0;i<(elf1->header).e_shnum;i++){
        (fusion->sections)[i]=(elf1->sections)[i];
    }
    for(int i=0;i<(elf1->header).e_shnum;i++){
        for(int j=0;j<(elf2->header).e_shnum;j++){
            if(((elf1->sections)[i].h_section.sh_type == SHT_PROGBITS && (elf2->sections)[j].h_section.sh_type == SHT_PROGBITS) 
            && (elf1->sections)[i].h_section.sh_name == (elf2->sections)[j].h_section.sh_name){
                //on modifie la taille du contenu de notre fichier fusion
                (fusion->sections)[i].h_section.sh_size+=(elf2->sections)[j].h_section.sh_size;
                (fusion->sections)[i].contenu = realloc((fusion->sections)[i].contenu, sizeof(uint8_t) * 
                (elf2->sections)[j].h_section.sh_size + (elf1->sections)[i].h_section.sh_size);
                strcat((char *)(fusion->sections)[i].contenu,(char *)(elf2->sections)[j].contenu);
                nbre_potalas++;
                }
        }
    }
    fusion->nb_sections -= nbre_potalas;
    for(int i=(elf1->header).e_shnum;i<fusion->nb_sections;i++){
        (fusion->sections)[i]=(elf2->sections)[fusion->nb_sections - i];
    }
    return fusion;
}
*/