## Projet éditeur de lien ELF pour architecture ARM en Big endian

### Auteurs:
    -Roualy Mehdi
    -Dudek  Milan
    -Kutlu  Akin
    -Neveux Luckas
    -RAVELONAHINA Miranto
    -Mbuy'Ibutsi Aunouviet Mathys

### Mode d'emploi
- Pour compiler le projet :
```bash
make release
```
- pour lancer gdb/ valgrind
```bash
make debug
```
- Pour utiliser le elfReader :
```bash
./elfReader <option(s)> <filename>
```

Une liste de toutes les options possibles est disponible en tapant :
```{text}
    -a  montre l'intégralité du fichier
    -h  montre les headers du fichier
    -S  montre les section du fichier
    -s  montre la table des symboles du fichier
    -x  montre le contenue d'une section choisie
    -r  montre les tables de réimplantation
```

- Pour utiliser le linker :
```bash
./linker <option> resultname filename1 filename2
```
une liste des toutes les options possibles est disponible en tapant:
```{text}
    -h  montre comment utilisée ce programme
    -o  permet de choisir le nom du fichier final
```

### Descriptif de la structure du code (pour le elfReader):  
```{text}
- elfReader.c : programme principal, permet de lancer les différentes phases  
- elf_header.c : contient le code de l'étape 1, permet de lire et afficher l'entête du fichier ELF
- elf_section.c : contient le code de l'étape 2 et de l'étape 3, permet de lire et afficher la table des sections du fichier ELF, ainsi que le contenu d'une section précise
- elf_tableSymboles.c : contient le code de l'étape 4, permet de lire et afficher la table des symboles du fichier ELF
- elf_reimplantation.c : contient le code de l'étape 5, permet de lire et afficher les tables de réimplantation du fichier ELF
``` 

## Liste des fonctionnalités implémentées et manquantes
### Fontionnalités implémentées :
- Affichage de l'entête du fichier ELF
- Affichage de la table des sections du fichier ELF
- Affichage du contenu d'une section précise
- Affichage de la table des symboles du fichier ELF
- Affichage des tables de réimplantation du fichier ELF

### Fonctionnalités implémentées non tester:
- Fusion des sections de deux fichier ELF
- Fusion des tables de symboles de deux ficher ELF
- Fusion des table de reimplantaion de deux fichier ELF

### Fonctionnalités manquantes :
- la Phase 2 non terminer
- les parties 6, 7 et 8 non tester
- la partie 9.
- les fichiers en litte endian non pris en compte

## Liste des éventuels bugs connus mais non résolus:
- les parties 6 à 8 sont non tester donc des fois y a des segfaults
- tout les bugs présent (connue) sont du au fait que linker.c n'est pas terminer 

## Liste et description des tests effectués
Liste de fichiers à tester :
Pour tester les programmes, nous avons créé 7 fichiers ARM 32 bits big endian supplémentaires :
- test 1: 1 fichier .elf compiler en type EXEC contenant juste un main() (tout dois être valide)
- test 2: 1 fichier .o compiler en type REL qui ne contient pas de main() (tout dois être valide sauf les reloc car problème d'affichage)
- test 3: 1 fichier .elf compiler via 1 fichier et un shared library le tout en type EXEC contenant un main() (tout dois être valide sauf les symboles volontaires)

Lancer les tests :
- Pour lancer tous les tests :
```bash
./test_elfReader.py
```
ou 
```bash
make test
```

