#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
        elf->sections[i].contenu = NULL;
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
    if (elf == NULL || (elf->sections) == NULL) 
        error("Sections non chargées\n");
    
    printf("There are %d section headers, starting at offset 0x%x:\n\n", 
           elf->header.e_shnum, elf->header.e_shoff);
    
    printf("Section Headers:\n");
    printf("  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al\n");
    
    for (int i = 0; i < elf->header.e_shnum; i++) {
        const Elf32_Shdr* section = &elf->sections[i].h_section;
        char flags_buff[4] = {0};  // Pour les flags (max 3 caractères + '\0')
        
        printf("  [%2d] %-17s %-15s %08x %06x %06x %02x %3s %2u %3u %2u\n",
               i,
               elf->section_str_table + section->sh_name,  // Nom
               get_type(section->sh_type),                 // Type
               section->sh_addr,                           // Addr
               section->sh_offset,                         // Off
               section->sh_size,                           // Size
               section->sh_entsize,                        // ES (Entry Size)
               get_flags(section->sh_flags, flags_buff),   // Flg
               section->sh_link,                           // Lk
               section->sh_info,                           // Inf
               section->sh_addralign                       // Al
        );
    }
    
    printf( "Key to Flags:\n"
            "  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n"
            "  L (link order), O (extra OS processing required), G (group), T (TLS),\n"
            "  C (compressed), x (unknown), o (OS specific), E (exclude),\n"
            "  D (mbind), y (purecode), p (processor specific)\n"
        );
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
    
    // Trouver l'index de la section 
    if (is_numerical(param)) {
        index_section = atoi(param);
    } else {
        for (int i = 0; i < elf->header.e_shnum; i++) {
            // Calcul du nom : table des strings + offset du nom de la section
            char *nom_courant = elf->section_str_table + elf->sections[i].h_section.sh_name;
            if (strcmp(nom_courant, param) == 0) {
                index_section = i;
                break;
            }
        }
    }
    
    if (index_section < 0 || index_section >= elf->header.e_shnum) {
        printf("\nreadelf: Warning: Section '%s' was not dumped because it does not exist!\n", param);
        return;
    }
    
    elf32_sections *sect = &elf->sections[index_section];
    char *nom_section = elf->section_str_table + sect->h_section.sh_name;
    
    // Si c'est une section vide ou sans contenu
    if (sect->contenu == NULL || sect->h_section.sh_size == 0) {
        printf("\nSection '%s' has no data to dump.\n", nom_section);
        return;
    }
    
    // Affichage de l'en-tête
    printf("\nHex dump of section '%s':\n", nom_section);
    
    // Affichage du contenu
    for (Elf32_Word i = 0; i < sect->h_section.sh_size; i += 16) {
        // Adresse (avec l'adresse de base de la section si elle est chargée en mémoire)
        printf("  0x%08x ", sect->h_section.sh_addr + i);
        
        // Partie HEX (4 groupes de 4 octets)
        for (int j = 0; j < 16; j++) {
            if (i + j < sect->h_section.sh_size) {
                printf("%02x", sect->contenu[i + j]);
            } else {
                printf("  ");
            }
            
            // Espacement tous les 4 octets
            if ((j + 1) % 4 == 0) {
                printf(" ");
            }
        }

        //ASCII
        printf(" ");

        for (int j = 0; j < 16 && i + j < sect->h_section.sh_size; j++) {
            unsigned char c = sect->contenu[i + j];
            if (isprint(c)) {
                printf("%c", c);
            } else {
                printf(".");
            }
        }
        
        printf("\n");
    }
    
    printf("\n");
}

uint32_t calculer_e_shoff(const elf32_fusion_sections* fusion) {
    uint32_t offset = sizeof(Elf32_Ehdr);

    for (int i = 0; i < fusion->nb_sections; i++) {
        Elf32_Shdr *sh = &fusion->sections[i].h_section;

        if (sh->sh_type != SHT_NOBITS) {
            offset += sh->sh_size;
        }
    }

    return offset;
}

int est_fusionnable(const Elf32_Shdr *s) {
    return (s->sh_type == SHT_PROGBITS) &&
           (s->sh_flags & SHF_ALLOC);
}

elf32_fusion_sections* fusion_sections(elf32_t* elf1, elf32_t* elf2) {
    if (!elf1 || !elf2) error("ELF non initialisé");

    elf32_fusion_sections* fusion = fusion_sect_init();
    if (!fusion) error("Erreur malloc fusion section");

    int max_sections = elf1->header.e_shnum + elf2->header.e_shnum;
    fusion->sections = malloc(sizeof(elf32_sections) * max_sections);
    if (!fusion->sections) error("Erreur malloc sections fusion");

    fusion->map_elf2 = malloc(sizeof(section_map_t) * elf2->header.e_shnum);
    if (!fusion->map_elf2) error("Erreur malloc map elf2");
    fusion->nb_map = 0;

    int idx_fusion = 0;

    for (int i = 0; i < elf1->header.e_shnum; i++) {
        fusion->sections[idx_fusion].h_section = elf1->sections[i].h_section;

        if (elf1->sections[i].h_section.sh_size > 0 &&
            elf1->sections[i].contenu != NULL) {
            fusion->sections[idx_fusion].contenu =
                malloc(elf1->sections[i].h_section.sh_size);
            if (!fusion->sections[idx_fusion].contenu)
                error("Erreur malloc section ELF1");
            memcpy(fusion->sections[idx_fusion].contenu,
                   elf1->sections[i].contenu,
                   elf1->sections[i].h_section.sh_size);
        } else {
            fusion->sections[idx_fusion].contenu = NULL;
        }
        idx_fusion++;
    }

    for (int j = 0; j < elf2->header.e_shnum; j++) {
        int fusionnee = 0;
        const char *name2 = elf2->section_str_table + elf2->sections[j].h_section.sh_name;

        for (int i = 0; i < elf1->header.e_shnum; i++) {
            const char *name1 = elf1->section_str_table + elf1->sections[i].h_section.sh_name;

            if (strcmp(name1, name2) == 0 &&
                est_fusionnable(&elf1->sections[i].h_section) &&
                est_fusionnable(&elf2->sections[j].h_section)) {

                size_t old_size = fusion->sections[i].h_section.sh_size;
                size_t add_size = elf2->sections[j].h_section.sh_size;

                uint8_t* new_buf = realloc(fusion->sections[i].contenu, old_size + add_size);
                if (!new_buf) error("Erreur realloc fusion PROGBITS");

                memcpy(new_buf + old_size, elf2->sections[j].contenu, add_size);

                fusion->sections[i].contenu = new_buf;
                fusion->sections[i].h_section.sh_size = old_size + add_size;

                fusion->map_elf2[fusion->nb_map].ancien_index = j;
                fusion->map_elf2[fusion->nb_map].nouvel_index = i;
                fusion->map_elf2[fusion->nb_map].offset = old_size;
                fusion->nb_map++;

                fusionnee = 1;
                break;
            }
        }

        if (fusionnee) continue;
        if (elf2->sections[j].h_section.sh_type == SHT_STRTAB) {
            int found = -1;
            for (int i = 0; i < idx_fusion; i++) {
                const char* name1 = elf1->section_str_table + elf1->sections[i].h_section.sh_name;
                if (strcmp(name1, name2) == 0) { found = i; break; }
            }

            if (found >= 0) {
                size_t old_size = fusion->sections[found].h_section.sh_size;
                size_t add_size = elf2->sections[j].h_section.sh_size;
                uint8_t* new_buf = malloc(old_size + add_size);
                if (!new_buf) error("Erreur malloc fusion STRTAB");

                memcpy(new_buf, fusion->sections[found].contenu, old_size);
                memcpy(new_buf + old_size, elf2->sections[j].contenu, add_size);

                free(fusion->sections[found].contenu);
                fusion->sections[found].contenu = new_buf;
                fusion->sections[found].h_section.sh_size = old_size + add_size;

                for (int s = 0; s < elf2->header.e_shnum; s++) {
                    if (elf2->sections[s].h_section.sh_type == SHT_SYMTAB) {
                        Elf32_Sym* symtab = (Elf32_Sym*)elf2->sections[s].contenu;
                        int nb_sym = elf2->sections[s].h_section.sh_size / sizeof(Elf32_Sym);
                        for (int k = 0; k < nb_sym; k++) {
                            symtab[k].st_name += old_size;
                        }
                    }
                }

                fusion->map_elf2[fusion->nb_map].ancien_index = j;
                fusion->map_elf2[fusion->nb_map].nouvel_index = found;
                fusion->map_elf2[fusion->nb_map].offset = old_size;
                fusion->nb_map++;
                continue;
            }
        }

        fusion->sections[idx_fusion].h_section = elf2->sections[j].h_section;
        if (elf2->sections[j].h_section.sh_size > 0 &&
            elf2->sections[j].contenu != NULL) {
            fusion->sections[idx_fusion].contenu =
                malloc(elf2->sections[j].h_section.sh_size);
            if (!fusion->sections[idx_fusion].contenu)
                error("Erreur malloc section ELF2");
            memcpy(fusion->sections[idx_fusion].contenu,
                   elf2->sections[j].contenu,
                   elf2->sections[j].h_section.sh_size);
        } else {
            fusion->sections[idx_fusion].contenu = NULL;
        }

        fusion->map_elf2[fusion->nb_map].ancien_index = j;
        fusion->map_elf2[fusion->nb_map].nouvel_index = idx_fusion;
        fusion->map_elf2[fusion->nb_map].offset = 0;
        fusion->nb_map++;

        idx_fusion++;
    }

    fusion->nb_sections = idx_fusion;

    fusion->section_str_table = (char*)fusion->sections[elf1->header.e_shstrndx].contenu;

    return fusion;
}


