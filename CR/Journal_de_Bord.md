## Projet éditeur de lien ELF pour architecture ARM en Big endian

### Lundi 15 Décembre 2025:  
```{text}
présentation du code source  
configuration des accès git   
création du Makefile  
début de la lecture de l'entête du format ELF
```

### Mardi 16 Décembre 2025:  
```{text}
finalisation de la lecture et l'affichage de l'entête du fichier au format ELF passée en paramètre 
implémentation de fichiers de tests utilisant des compilateurs différents  
création de la fonction principale du projet
modification du fichier elf_header.h en elf.h et du type ELF et elf32_t
```

### Mercredi 17 Décembre 2025:  
```{text}
implémentation de la lecture des sections
ajout des test d'erreur pour les fonctions get32/16/8 
correction de la logique de lecture des headers ELF
implémentation de l'affichage du contenu d’une section
```

### Jeudi 18 Décembre 2025:  
```{text}
début de l'implémentation de la table des symboles 
formatage du code pour une meilleure lisibilité
```