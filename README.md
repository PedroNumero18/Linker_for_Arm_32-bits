## Projet éditeur de lien ELF pour architecture ARM en Big endian

### Auteurs:
    -Roualy Mehdi
    -Dudek  Milan
    -Kutlu  Akin
    -Neveux Luckas

### Mode d'emploi
- Pour compiler le projet :
```bash
make
```
- Pour utiliser le elfReader :
```bash
./elfReader <option(s)> <filename>
```
Une liste de toutes les commandes possibles est disponible en tapant :
```{text}
    -a  montre l'intégralité du fichier
    -h  montre les headers du fichier
    -S  montre les section du fichier
    -s  montre la table des symboles du fichier
    -r  montre les tables de réimplantation
```

### Descriptif de la structure du code (pour le elfReader):  
```{text}
- elfReader.c : programme principal, permet de lancer les différentes phases  
- elf_header.c : contient le code de l'étape 1, permet de lire et afficher l'entête du fichier ELF
- elf_section.c : contient le code de l'étape 2 et de l'étape 3, permet de lire et afficher la table des sections du fichier ELF, ainsi que le contenu d'une section précise
- elf_tableSymboles.c : contient le code de l'étape 4, permet de lire et afficher la table des symboles du fichier ELF
- elf_reimplantation.c : contient le code de l'étape 5, permet de lire et afficher les tables de réimplantation du fichier ELF
``` 


