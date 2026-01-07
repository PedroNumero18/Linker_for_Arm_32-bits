#include <stdlib.h> 
#include <stdio.h>
#include <getopt.h>

#include "utils.h"
#include "elf.h"

void print_flags(const char* nom){
    fprintf(stderr,"Usage:\n" 
           "./%s <option> <result> <filename1> <filename2>\n"
           "DEFAULT result: a.out\n"
           "fusionne 1 à 2 fichier au format elf pour en faire un executable\n"
           "voici les options:\n"
           " -h \t\t\t montre comment utilisée ce programme\n"
           " -o \t\t\t permet de choisir le nom du fichier final\n"
    , nom);
    return;
}


int main(int argc, char** argv){
    int opt;
    char *filename1 = NULL, *filename2 = NULL; //*result = "a.out";
    FILE *file1, *file2;// *output;


    while(  (   opt = getopt(argc, argv, "ho:")   ) != -1){
        switch (opt){
            case 'h': print_flags(argv[0]); return 0;                               break;
            case 'o': /*result = optarg; */                                            break; 
            default: print_flags(argv[0]); error("cette option n'existe pas !!!");  break;
        }
    }
    if ((argc - optind) != 2){
        print_flags(argv[0]); error("Erreur:attendu 2 fichiers en entrée \n");
    }
    filename1 = argv[optind];
    filename2 = argv[optind + 1];

    file1 = fopen(filename1, "rb");
    file2 = fopen(filename2, "rb");

    if (!file1 || !file2) error("Error opening files\n");

    elf32_t* elf1 = elf_init();
    elf32_t* elf2 = elf_init();

    lire_header(file1, elf1);   lire_header(file2, elf2);
    lire_sections(file1, elf1); lire_sections(file2, elf2);
    lire_symbole(file1, elf1);  lire_symbole(file2, elf2);
    lire_Reimple(file1, elf1);  lire_Reimple(file2, elf2);


    elf32_fusion_sections* fusionSec = fusion_sections(elf1, elf2);
    if (!fusionSec) error("Erreur fusion des sections\n");
    //reconstruire_shstrtab(fusionSec);
    
    printf("\n===== FUSION DES SECTIONS =====\n");
    printf("Nombre de sections après fusion : %d\n\n", fusionSec->nb_sections);

    elf32_t elf_fusion_fake;
    elf_fusion_fake.header.e_shnum = fusionSec->nb_sections;
    elf_fusion_fake.header.e_shoff = 0;
    elf_fusion_fake.sections = fusionSec->sections;

    elf_fusion_fake.section_str_table =(char*)fusionSec->sections[fusionSec->nb_sections - 1].contenu;
    afficher_sections(&elf_fusion_fake);

    printf("\n===== CONTENU DE LA SECTION 6 APRÈS FUSION =====\n");
    afficher_contenu_section(&elf_fusion_fake, "6");
    elf32_fusion_symboles* fusionSymb = fusion_symboles(elf1, elf2);
    if (!fusionSymb) error("erreur fusion symbole");

    printf("\n===== FUSION DES SYMBOLE =====\n");
    /* ==== FUSION ==== */
    elf32_fusion_symboles *fusion = fusion_symboles(elf1, elf2);
    if (!fusion) error("Erreur fusion_symboles\n");

        /* Faux ELF pour réutiliser afficher_symboles */
    elf32_t elf_fusion = {0};
    elf_fusion.header.e_shnum      = elf1->header.e_shnum;      // pour STT_SECTION
    elf_fusion.sections            = elf1->sections;            // réutilise les mêmes sections
    elf_fusion.section_str_table   = elf1->section_str_table;   // noms de sections
    elf_fusion.table_symbole       = fusion->table_symbole;    // table fusionnée
    elf_fusion.nb_symboles         = fusion->nb_sym;
    elf_fusion.symbol_str_table    = fusion->strtab;           // strtab fusionnée

    printf("\n===== SYMBOLES FUSIONNES =====\n");
    afficher_symboles(&elf_fusion);

    elf_free(elf1);
    elf_free(elf2);
    fclose(file1);
    fclose(file2);
    return 0;
}