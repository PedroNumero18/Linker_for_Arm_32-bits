#!/bin/bash

echo "DEMO : LECTURE ELF"

ELF=../tests/readerTests/test_RELA/factoriel.o
ELF2=../tests/readerTests/test_RELA/filtre_even.o
ELF3=../tests/readerTests/test_RELA/find_max.o

echo
echo ">> Affichage des Reimplémentations"
../elfReader -r $ELF3
