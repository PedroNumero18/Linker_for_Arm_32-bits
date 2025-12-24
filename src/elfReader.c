#include <stdlib.h> 
#include <stdio.h>
#include <getopt.h>

#include "utils.h"
#include "elf.h"

void print_flags(const char* nom){
    fprintf(stderr,"Usage:\n" 
           "%s <option(s)> <filename>\n"
           "affiche toutes les informations d'un fichier au format ELF\n"
           "voici les options:\n"
           " -a \t\t\t montre l'intégralité du fichier\n"
           " -h \t\t\t montre les headers du fichier\n"
           " -S \t\t\t montre les section du fichier\n"
           " -s \t\t\t montre la table des symboles du fichier\n"
    , nom);
    return;
}

int main(int argc,char *argv[]){
    int opt, a_flag, h_flag, S_flag, g_flag, s_flag, r_flag;
    
    while( ( opt = getopt(argc, argv, "ahSg:sr") ) != -1 ){
        switch(opt){
            case 'a': a_flag = 1; break;
            case 'h': h_flag = 1; break;
            case 'S': S_flag = 1; break;
            case 'g': g_flag = 1; break;
            case 's': s_flag = 1; break;
            case 'r': r_flag = 1; break;
            default: print_flags(argv[0]); error("cette option n'existe pas !!!"); break;
        }
    }
    if (!h_flag && !S_flag && !s_flag && !a_flag && !g_flag) {print_flags(argv[0]); error("aucune option fournie");}
    if (optind != argc-1) error("le nombre de fichier est incorrrect");

    FILE* inputFile = fopen(argv[optind],"rb");
    if (!inputFile)error("Error opening file\n");

    elf32_t* elf = malloc(sizeof(elf32_t));
    if(!elf)error("Error allocating memory to pointer\n");
    
    filename = argv[1];
    if(h_flag || a_flag){
        lire_header(inputFile, elf);
        affichage_entete(&elf->header);
    }
    if(S_flag || a_flag){
        lire_header(inputFile, elf);
        lire_sections(inputFile, elf);
        afficher_sections(elf);
        lire_contenu_sect(inputFile, elf, 5);
        afficher_contenu_section(elf, "5");
    }
    if(s_flag || a_flag){
        lire_header(inputFile, elf);
        lire_sections(inputFile, elf);
        lire_symbole(inputFile, elf);
        afficher_symboles(elf);
    }
    
    if(r_flag || a_flag){
        lire_header(inputFile, elf);
        lire_sections(inputFile, elf);
        lire_symbole(inputFile, elf);
        lire_Reimple(inputFile, elf);
        afficher_Reimple(elf);
    }

    free(elf);
    return 0;
}
