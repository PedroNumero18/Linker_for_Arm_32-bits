#include <stdio.h>
#include <stdint.h>

int main() {
    // Structure minimale représentant le début de l'e_ident d'un ELF
    // Nous allons écrire exactement 19 octets.
    uint8_t elf_header_start[19] = {
        0x7F, 'E', 'L', 'F', // 0-3: Magic Number
        1,                   // 4: EI_CLASS (1 = 32-bit)
        1,                   // 5: EI_DATA (1 = Big Endian)
        1,                   // 6: EI_VERSION (Toujours 1)
        0,                   // 7: EI_OSABI (0 = System V)
        0,                   // 8: EI_ABIVERSION
        0, 0, 0, 0, 0, 0, 0, // 9-15: Padding (zéros)
        0x02, 0x00,          // 16-17: e_type (2 = Executable file)
        0x3E                 // 18: Début de e_machine (0x3E = x86-64)
    };

    const char *filename = "mon_header.elf";
    FILE *file = fopen(filename, "wb");

    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
    }

    // Écriture des 19 octets
    size_t written = fwrite(elf_header_start, sizeof(uint8_t), 19, file);

    if (written == 19) {
        printf("Fichier '%s' généré avec succès (%zu octets).\n", filename, written);
        printf("Note : 19 n'est pas un multiple de 16 ou 32.\n");
    }

    fclose(file);
    return 0;
}
