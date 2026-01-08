#!/bin/bash

echo "=== DEMO : FUSION ==="

ELF1=../tests/linkerTests/test3/testMain.o
ELF2=../tests/linkerTests/test3/test3.o

echo
echo ">> Sections du fichier 1"
../elfReader -S $ELF1

echo
echo ">> Sections du fichier 2"
../elfReader -S $ELF2
echo
echo ">> Fusion des deux fichiers"
../linker -o fusion $ELF1 $ELF2
