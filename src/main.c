#include <stdlib.h> 
#include <stdio.h>
#include <getopt.h>

#include "utils.h"
#include "elf.h"

void print_flags(const char* nom){
    printf("Usage:\n" 
           "%s <option(s)> <filename>\n"
           "affiche toutes les informations d'un fichier au format ELF\n"
           "voici les options:\n"
           " -h \t\t\t montre les headers du fichier\n"
           " -S \t\t\t montre les section du fichier\n"
           " -s \t\t\t montre la table des symboles du fichier\n"
    , nom);
    return;
}

int main(int argc,char *argv[]){
    int opt, h_flag, S_flag, s_flag;
    struct option longOpts[]={
        {"header", required_argument, 0, 'h'},
        {"section", required_argument, 0 ,'S'},
        {"symbol", required_argument, 0, 's'},
        {NULL, no_argument, NULL, 0},
    };
    
    while( ( opt = getopt_long(argc, argv, "hSs", longOpts, &optind) ) != -1 ){
        switch(opt){
            case 'h': h_flag = 1; break;
            case 'S': S_flag = 1; break;
            case 's': s_flag = 1; break;
            case '?':
            default: print_flags(argv[0]); error("mauvais flag"); break;
        }
    }
    if (!h_flag && !S_flag && !s_flag) {print_flags(argv[0]); error("aucune option fournie");}

    if (optind != argc-1) error("le nombre de fichier est incorrrect");

    FILE* inputFile = fopen(argv[optind],"rb");
    if (!inputFile)error("Error opening file\n");

    elf32_t* elf = malloc(sizeof(elf32_t));
    if(!elf)error("Error allocating memory to pointer\n");
    
    filename = argv[1];

    lire_header(inputFile, elf);
    affichage_entete(&elf->header);
    lire_sections(inputFile, elf);
    afficher_sections(elf);
    lire_contenu_sect(inputFile, elf, 5);
    afficher_contenu_section(elf, "5");
    lire_symbole(inputFile, elf);
    afficher_symboles(elf);
    
    free(elf);
    return 0;
}
