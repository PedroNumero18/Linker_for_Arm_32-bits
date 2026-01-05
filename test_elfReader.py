#!/usr/bin/env python3
"""
Script de validation du elfReader en le comparant avec arm-none-eabi-readelf
Teste les fichiers ELF 32-bit big-endian dans le dossier tests/
"""

import subprocess
import sys
from pathlib import Path
from typing import Tuple, List
import difflib
from datetime import datetime

# Configuration
PROJECT_ROOT = Path(__file__).parent
ELFreader_PATH = PROJECT_ROOT / "elfReader"
TESTS_DIR = PROJECT_ROOT / "tests" / "testsSup"
LOG_FILE = PROJECT_ROOT / "test_elfReader_results.log"

# Fichiers de test ELF (32-bit big-endian)
TEST_FILES = [
    "Test1.elf",
    "Test2.elf", 
    "Test4.elf",
]

# Options à tester
TEST_OPTIONS = {
    "-h": "Headers du fichier",
    "-S": "Sections du fichier",
    "-s": "Table des symboles",
}


def check_requirements() -> bool:
    """Vérifie que les outils nécessaires sont disponibles"""
    print("Vérification des prérequis...")
    
    # Vérifier elfReader
    if not ELFreader_PATH.exists():
        print(f"❌ {ELFreader_PATH} n'existe pas")
        print("   Veuillez compiler avec: make")
        return False
    
    # Vérifier arm-none-eabi-readelf
    try:
        subprocess.run(
            ["arm-none-eabi-readelf", "--version"],
            capture_output=True,
            check=True
        )
        print("✓ arm-none-eabi-readelf disponible")
    except (FileNotFoundError, subprocess.CalledProcessError):
        print("❌ arm-none-eabi-readelf n'est pas installé")
        print("   Installez-le avec: sudo apt-get install binutils-arm-none-eabi")
        return False
    
    # Vérifier les fichiers de test
    for test_file in TEST_FILES:
        test_path = TESTS_DIR / test_file
        if not test_path.exists():
            print(f"⚠ Fichier de test manquant: {test_path}")
    
    print("✓ Prérequis satisfaits\n")
    return True


def run_command(cmd: List[str]) -> Tuple[str, int]:
    """Exécute une commande et retourne sa sortie et le code de retour"""
    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=5
        )
        return result.stdout + result.stderr, result.returncode
    except subprocess.TimeoutExpired:
        return "TIMEOUT", -1
    except Exception as e:
        return str(e), -1


def compare_outputs(our_output: str, ref_output: str) -> Tuple[bool, List[str]]:
    """Compare deux sorties et retourne les différences"""
    our_lines = our_output.strip().split('\n')
    ref_lines = ref_output.strip().split('\n')
    
    # Utiliser difflib pour trouver les différences
    diff = list(difflib.unified_diff(
        ref_lines,
        our_lines,
        fromfile='arm-none-eabi-readelf',
        tofile='elfReader',
        lineterm='',
        n=1
    ))
    
    return len(diff) == 0, diff


def test_file_option(test_file: str, option: str) -> Tuple[bool, str]:
    """Teste un fichier avec une option spécifique"""
    test_path = TESTS_DIR / test_file
    
    if not test_path.exists():
        return False, f"Fichier non trouvé: {test_path}"
    
    # Exécuter arm-none-eabi-readelf
    ref_cmd = [
        "arm-none-eabi-readelf",
        option,
        str(test_path)
    ]
    ref_output, ref_code = run_command(ref_cmd)
    
    # Exécuter notre elfReader
    our_cmd = [
        str(ELFreader_PATH),
        option,
        str(test_path)
    ]
    our_output, our_code = run_command(our_cmd)
    
    # Comparer
    is_same, differences = compare_outputs(our_output, ref_output)
    
    if is_same:
        return True, "✓ Pas de différences détectées"
    else:
        # Créer un résumé des différences
        diff_str = '\n'.join(differences[:20])  # Limiter à 20 lignes
        if len(differences) > 20:
            diff_str += f"\n... ({len(differences) - 20} lignes supplémentaires)"
        return False, diff_str


def main():
    """Fonction principale"""
    print("=" * 70)
    print("Test de validation du elfReader ARM 32-bit Big-Endian")
    print("=" * 70)
    print()
    
    if not check_requirements():
        sys.exit(1)
    
    # Résultats globaux
    total_tests = 0
    passed_tests = 0
    failed_tests = []
    
    # Tester chaque fichier avec chaque option
    for test_file in TEST_FILES:
        test_path = TESTS_DIR / test_file
        if not test_path.exists():
            continue
        
        print(f"📁 Fichier: {test_file}")
        print("-" * 70)
        
        for option, description in TEST_OPTIONS.items():
            total_tests += 1
            
            # Afficher l'option
            print(f"  Option {option} ({description})...", end=" ", flush=True)
            
            success, message = test_file_option(test_file, option)
            
            if success:
                print("✓")
                passed_tests += 1
            else:
                print("✗")
                failed_tests.append((test_file, option, message))
        
        print()
    
    # Résumé
    print("=" * 70)
    print("RÉSUMÉ DES TESTS")
    print("=" * 70)
    print(f"Total:  {total_tests} tests")
    print(f"✓ Réussis:  {passed_tests} tests")
    print(f"✗ Échoués:  {len(failed_tests)} tests")
    print()

    with open(LOG_FILE, 'w', encoding='utf-8') as log:
        log.write("=" * 70 + "\n")
        log.write("Test de validation du elfReader ARM 32-bit Big-Endian\n")
        log.write("=" * 70 + "\n")
        log.write(f"Date: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
        
        log.write("RÉSUMÉ DES TESTS\n")
        log.write("=" * 70 + "\n")
        log.write(f"Total:  {total_tests} tests\n")
        log.write(f"✓ Réussis:  {passed_tests} tests\n")
        log.write(f"✗ Échoués:  {len(failed_tests)} tests\n\n")
        
        # Afficher les détails des échecs
        if failed_tests:
            log.write("DÉTAILS DES ÉCHECS:\n")
            log.write("=" * 70 + "\n\n")
            for test_file, option, message in failed_tests:
                log.write(f"Fichier: {test_file}\n")
                log.write(f"Option: {option}\n")
                log.write("-" * 70 + "\n")
                log.write(message + "\n")
                log.write("\n" + "=" * 70 + "\n\n")
        else:
            log.write("Tous les tests sont passés avec succès ! ✓\n")
    
    print(f"📄 Résultats sauvegardés dans: {LOG_FILE}")
    print("=" * 70)
    
    # Code de sortie
    return 0 if len(failed_tests) == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
