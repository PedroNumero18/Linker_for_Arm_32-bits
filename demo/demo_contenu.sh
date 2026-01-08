#!/bin/bash

# Vérification des arguments
if [ $# -lt 1 ]; then
    echo "Usage : $0 <nom_section|index>"
    echo "Exemple : $0 .text"
    echo "Exemple : $0 6"
    exit 1
fi

ELF_FILE=../tests/readerTests/test1/test1.elf
SECTION=$1

echo "======================================"
echo " FICHIER : $ELF_FILE"
echo " SECTION : $SECTION"
echo "======================================"
echo

../elfReader -x "$SECTION" "$ELF_FILE"
