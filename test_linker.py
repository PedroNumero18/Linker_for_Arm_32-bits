#!/usr/bin/env python3

"""
Script de validation de l'édition de lien en le comparant avec arm-none-eabi-ld
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
ELFreader_PATH = PROJECT_ROOT / "linker"
TESTS_DIR = PROJECT_ROOT / "tests" / "testsSup"
LOG_FILE = PROJECT_ROOT / "test_linker_results.log"