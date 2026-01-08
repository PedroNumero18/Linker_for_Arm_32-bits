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
           " -x \t\t\t montre le contenue d'une section choisie\n"
           " -s \t\t\t montre la table des symboles du fichier\n"
           " -r \t\t\t montre les tables de réimplantation\n"
    , nom);
    return;
}

int main(int argc,char *argv[]){
    int opt;
    char* wSection;
    int a_flag = 0, h_flag = 0, S_flag = 0, s_flag = 0, x_flag = 0, r_flag = 0;
    
    while( ( opt = getopt(argc, argv, "ahSsx:r") ) != -1 ){
        switch(opt){
            case 'a': a_flag = 1; break;
            case 'h': h_flag = 1; break;
            case 'S': S_flag = 1; break;
            case 's': s_flag = 1; break;
            case 'r': r_flag = 1; break;
            case 'x': x_flag = 1; wSection = optarg; break; 
            default: print_flags(argv[0]); error("cette option n'existe pas !!!"); break;
        }
    }
    if (!h_flag && !S_flag && !s_flag && !a_flag && !x_flag && !r_flag) {print_flags(argv[0]); error("aucune option fournie");}
    if (optind != argc-1) error("le nombre de fichier est incorrrect");
    filename = argv[optind];
    FILE* inputFile = fopen(filename,"rb");
    if (!inputFile)error("Error opening file\n");

    elf32_t* elf = elf_init();
    if(!elf)error("Error allocating memory to pointer\n");
    
    lire_header(inputFile, elf);
    lire_sections(inputFile, elf);
    //lire_contenu_sect(inputFile, elf, 6);
    lire_symbole(inputFile, elf);
    lire_Reimple(inputFile, elf);
    

    if(h_flag || a_flag) affichage_entete(&elf->header);
    if(S_flag || a_flag) afficher_sections(elf); 
    if (x_flag) afficher_contenu_section(elf, wSection);
    if(s_flag || a_flag) afficher_symboles(elf);
    if(r_flag || a_flag) afficher_Reimple(elf);
    

    elf_free(elf);
    return 0;
}
