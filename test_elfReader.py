#!/usr/bin/env python3

"""
Script de validation du elfReader en comparant la sortie avec les fichiers .res
Teste les fichiers ELF 32-bit big-endian dans le dossier tests/readerTests
"""

import subprocess
import sys
from pathlib import Path
from typing import Tuple, List, Dict
import difflib
from datetime import datetime

# Configuration
PROJECT_ROOT = Path(__file__).parent
ELFreader_PATH = PROJECT_ROOT / "elfReader"
TESTS_DIR = PROJECT_ROOT / "tests" / "readerTests"
LOG_FILE = PROJECT_ROOT / "test_elfReader_results.log"

# Mapping entre options et fichiers .res
OPTION_TO_RES_FILE = {
    "-h": "header.res",
    "-S": "section.res",
    "-s": "symbol.res",
    "-r": "reloc.res",
}


def check_requirements() -> bool:
    """Vérifie que les outils nécessaires sont disponibles"""
    print("Vérification des prérequis...")
    
    # Vérifier elfReader
    if not ELFreader_PATH.exists():
        print(f"\033[91m[ERREUR]\033[0m {ELFreader_PATH} n'existe pas")
        print("   Veuillez compiler avec: make")
        return False
    
    # Vérifier que le répertoire de test existe
    if not TESTS_DIR.exists():
        print(f"\033[91m[ERREUR]\033[0m {TESTS_DIR} n'existe pas")
        return False
    
    print("\033[92m[OK]\033[0m Prérequis satisfaits\n")
    return True


def find_elf_file(test_dir: Path) -> Path:
    """Trouve le fichier ELF dans un répertoire de test"""
    for file in test_dir.iterdir():
        if file.suffix in ['.elf', '.o']:
            return file
    return None


def read_res_file(res_file: Path) -> str:
    """Lit le contenu d'un fichier .res en supprimant les blocs ```plaintext"""
    if not res_file.exists():
        return None
    
    with open(res_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Supprimer les blocs ```plaintext ... ```
    content = content.replace('```plaintext\n', '')
    content = content.replace('\n```', '')
    
    return content.strip()


def test_file_option(test_dir: Path, option: str) -> Tuple[bool, str]:
    """Teste un fichier avec une option spécifique"""
    # Trouver le fichier ELF
    elf_file = find_elf_file(test_dir)
    if not elf_file:
        return False, f"Aucun fichier ELF trouvé dans {test_dir}"
    
    # Trouver le fichier .res correspondant
    res_filename = OPTION_TO_RES_FILE.get(option)
    if not res_filename:
        return False, f"Option {option} non supportée"
    
    res_file = test_dir / res_filename
    if not res_file.exists():
        return False, f"Fichier {res_filename} non trouvé"
    
    # Lire le contenu du fichier .res
    expected_output = read_res_file(res_file)
    if expected_output is None:
        return False, f"Impossible de lire {res_filename}"
    
    # Exécuter notre elfReader
    our_cmd = [
        str(ELFreader_PATH),
        option,
        str(elf_file)
    ]
    try:
        result = subprocess.run(
            our_cmd,
            capture_output=True,
            text=True,
            timeout=5
        )
        our_output = (result.stdout + result.stderr).strip()
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT"
    except Exception as e:
        return False, str(e)
    
    # Comparer les sorties
    is_same, differences = compare_outputs(our_output, expected_output)
    
    if is_same:
        return True, "\033[92m[OK]\033[0m Pas de différences détectées"
    else:
        # Créer un résumé des différences
        diff_str = '\n'.join(differences[:20])  # Limiter à 20 lignes
        if len(differences) > 20:
            diff_str += f"\n... ({len(differences) - 20} lignes supplémentaires)"
        return False, diff_str


def compare_outputs(our_output: str, ref_output: str) -> Tuple[bool, List[str]]:
    """Compare deux sorties et retourne les différences"""
    our_lines = our_output.strip().split('\n')
    ref_lines = ref_output.strip().split('\n')
    
    # Utiliser difflib pour trouver les différences
    diff = list(difflib.unified_diff(
        ref_lines,
        our_lines,
        fromfile='.res file',
        tofile='elfReader',
        lineterm='',
        n=1
    ))
    
    return len(diff) == 0, diff


def main():
    """Fonction principale"""
    print("=" * 70)
    print("Test de validation du elfReader avec fichiers .res")
    print("=" * 70)
    print()
    
    if not check_requirements():
        sys.exit(1)
    
    # Trouver tous les répertoires de test
    test_dirs = sorted([d for d in TESTS_DIR.iterdir() if d.is_dir() and d.name.startswith('test') and d.name != 'test_RELA'])
    
    # Résultats globaux
    total_tests = 0
    passed_tests = 0
    failed_tests = []
    
    # Tester chaque répertoire avec chaque option
    for test_dir in test_dirs:
        print(f"\033[94m[DOSSIER]\033[0m {test_dir.name}")
        print("-" * 70)
        
        for option, description in OPTION_TO_RES_FILE.items():
            total_tests += 1
            
            # Afficher l'option
            print(f"  Option {option} ({description})...", end=" ", flush=True)
            
            success, message = test_file_option(test_dir, option)
            
            if success:
                print("\033[92m[OK]\033[0m")
                passed_tests += 1
            else:
                print("\033[91m[ERREUR]\033[0m")
                failed_tests.append((test_dir.name, option, message))
        
        print()
    
    # Résumé
    print("=" * 70)
    print("RÉSUMÉ DES TESTS")
    print("=" * 70)
    print(f"Total:  {total_tests} tests")
    print(f"\033[92m[OK]\033[0m Réussis:  {passed_tests} tests")
    print(f"\033[91m[ERREUR]\033[0m Échoués:  {len(failed_tests)} tests")
    print()

    with open(LOG_FILE, 'w', encoding='utf-8') as log:
        log.write("=" * 70 + "\n")
        log.write("Test de validation du elfReader avec fichiers .res\n")
        log.write("=" * 70 + "\n")
        log.write(f"Date: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
        
        log.write("RÉSUMÉ DES TESTS\n")
        log.write("=" * 70 + "\n")
        log.write(f"Total:  {total_tests} tests\n")
        log.write(f"[OK] Réussis:  {passed_tests} tests\n")
        log.write(f"[ERREUR] Échoués:  {len(failed_tests)} tests\n\n")
        
        # Afficher les détails des échecs
        if failed_tests:
            log.write("DÉTAILS DES ÉCHECS:\n")
            log.write("=" * 70 + "\n\n")
            for test_name, option, message in failed_tests:
                log.write(f"Dossier de test: {test_name}\n")
                log.write(f"Option: {option}\n")
                log.write("-" * 70 + "\n")
                log.write(message + "\n")
                log.write("\n" + "=" * 70 + "\n\n")
        else:
            log.write("Tous les tests sont passés avec succès !\n")
    
    print(f"\033[94m[FICHIER]\033[0m Résultats sauvegardés dans: {LOG_FILE}")
    print("=" * 70)
    
    # Code de sortie
    return 0 if len(failed_tests) == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
