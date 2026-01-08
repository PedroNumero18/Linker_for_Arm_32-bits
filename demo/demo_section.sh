#!/bin/bash

echo "DEMO : LECTURE ELF"

ELF=../tests/readerTests/test1/test1.elf

echo
echo ">> Affichage des sections"
../elfReader -S $ELF
