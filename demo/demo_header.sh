#!/bin/bash

echo "DEMO : LECTURE ELF"

ELF=../tests/readerTests/test1/test1.elf

echo
echo ">> Affichage de l’en-tête"
../elfReader -h $ELF

