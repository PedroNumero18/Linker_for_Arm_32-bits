#include <stdlib.h>
#include <string.h>
#include "elf.h"

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
        perror("Erreur d'allocation mémoire");
        exit(1);
    }

    // On se positionne au bon endroit dans le fichier
    // On utilise 'h_section.sh_offset'
    if (fseek(f, section->h_section.sh_offset, SEEK_SET) != 0) {
        perror("Erreur de fseek");
        exit(1);
    }

    // On fait la lecture des données brutes
    fread(section->contenu, 1, section->h_section.sh_size, f);
}


// Permet de savoir si c'est un nombre
int est_numerique(char *str) {
    if (str == NULL || *str == '\0') return 0;
    while (*str) {
        if (*str < '0' || *str > '9') return 0;
        str++;
    }
    return 1;
}


void afficher_contenu_section(elf32_t *elf, char *param) {
    int index_section = -1;

    // Trouver l'index de la section 
    if (est_numerique(param)) {
        // Numéro
        index_section = atoi(param);
    } 
    else {
        // Nom
        // On parcourt toutes les sections pour trouver celle qui a ce nom
        for (int i = 0; i < elf->header.e_shnum; i++) {
            // Calcul du nom : table des strings + offset du nom de la section
            char *nom_courant = elf->section_str_table + elf->sections[i].h_section.sh_name;
            if (strcmp(nom_courant, param) == 0) {
                index_section = i;
                break;
            }
        }
    }

    // On regarde si ça existe ou pas 
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

    for (Elf32_Word i = 0; i < sect->h_section.sh_size; i++) {

        // On passe à la ligne tous les 16 octets et on affiche l'adresse
        if (i % 16 == 0) {
            printf("\n  0x%08x ", i);
        }
        printf("%02x", sect->contenu[i]); // Affiche l'octet en hex

        // Espace tous les 4 octets
        if ((i + 1) % 4 == 0) printf(" "); 
    }
    printf("\n");
}