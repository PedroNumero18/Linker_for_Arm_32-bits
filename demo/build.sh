#!/bin/bash

echo "COMPILATION DU LINKER"

cd ..

make clean
if make release; then
    echo " Compilation réussie"
else
    echo " Erreur de compilation"
    exit 1
fi

cd demo
