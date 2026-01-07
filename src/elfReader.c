#include <stdlib.h> 
#include <stdio.h>
#include <getopt.h>

#include "utils.h"
#include "elf.h"

void print_flags(const char* nom){
    fprintf(stderr,"Usage:\n" 
           "./%s <option(s)> <filename>\n"
           "./%s -f <file1> <file2>\n"
           "affiche toutes les informations d'un fichier au format ELF\n"
           "voici les options:\n"
           " -a \t\t\t montre l'intégralité du fichier\n"
           " -h \t\t\t montre les headers du fichier\n"
           " -S \t\t\t montre les section du fichier\n"
           " -s \t\t\t montre la table des symboles du fichier\n"
           " -r \t\t\t montre les tables de réimplantation\n"
           " -f \t\t fusionne deux fichiers ELF (étape 6)\n"
    , nom, nom);
    return;
}

void mode_fusion(const char* file1, const char* file2) {
    FILE* f1 = fopen(file1, "rb");
    FILE* f2 = fopen(file2, "rb");
    if (!f1 || !f2) error("Erreur ouverture fichier pour fusion\n");

    elf32_t* elf1 = elf_init();
    elf32_t* elf2 = elf_init();
    if (!elf1 || !elf2) error("Erreur allocation ELF\n");

    lire_header(f1, elf1);
    lire_sections(f1, elf1);
    lire_symbole(f1, elf1);
    lire_Reimple(f1, elf1);

    lire_header(f2, elf2);
    lire_sections(f2, elf2);
    lire_symbole(f2, elf2);
    lire_Reimple(f2, elf2);

    //fusion
    elf32_fusion_sections* fusion = fusion_sections(elf1, elf2);
    if (!fusion) error("Erreur fusion des sections\n");

    printf("\n===== FUSION DES SECTIONS =====\n");
    printf("Nombre de sections après fusion : %d\n\n", fusion->nb_sections);

   elf32_t elf_fusion_fake;
    elf_fusion_fake.header.e_shnum = fusion->nb_sections;
    elf_fusion_fake.header.e_shoff = 0;
    elf_fusion_fake.sections = fusion->sections;

    elf_fusion_fake.section_str_table = elf1->section_str_table;
    afficher_sections(&elf_fusion_fake);

    printf("\n===== CONTENU DE LA SECTION 1 APRÈS FUSION =====\n");
    afficher_contenu_section(&elf_fusion_fake, "1");

    elf_free(elf1);
    elf_free(elf2);
    fclose(f1);
    fclose(f2);
}


int main(int argc,char *argv[]){
    int opt;
    int a_flag = 0, h_flag = 0, S_flag = 0, s_flag = 0, r_flag = 0 , f_flag = 0;
    
    while( ( opt = getopt(argc, argv, "ahSsr f") ) != -1 ){
        switch(opt){
            case 'a': a_flag = 1; break;
            case 'h': h_flag = 1; break;
            case 'S': S_flag = 1; break;
            case 's': s_flag = 1; break;
            case 'r': r_flag = 1; break;
            case 'f': f_flag = 1; break;
            default: print_flags(argv[0]); error("cette option n'existe pas !!! \n"); break;
        }
    }
    if (f_flag) {
        if (argc - optind != 2) {
            print_flags(argv[0]);
            error("L'option -f attend exactement 2 fichiers ELF\n");
        }
        mode_fusion(argv[optind], argv[optind + 1]);
        return 0;
    }

    if (!h_flag && !S_flag && !s_flag && !a_flag && !r_flag) {print_flags(argv[0]); error("aucune option fournie");}
    if (optind != argc-1) error("le nombre de fichier est incorrect \n");
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
    

    if(h_flag || a_flag){affichage_entete(&elf->header);}
    if(S_flag || a_flag){afficher_sections(elf); afficher_contenu_section(elf, "5");}
    if(s_flag || a_flag) afficher_symboles(elf);
    if(r_flag || a_flag) afficher_Reimple(elf);
    

    elf_free(elf);
    return 0;
}
