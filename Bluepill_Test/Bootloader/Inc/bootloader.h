#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>

/* Valores disponíveis para inspeção pelo depurador. */
extern volatile uint32_t app_stack;
extern volatile uint32_t app_reset;
extern volatile uint32_t app_vectors_ok;

/* Verifica apenas a plausibilidade dos vetores.
   Não verifica a integridade completa da imagem. */
uint32_t Bootloader_CheckApplicationVectors(void);
/* Retorna 0 se os vetores forem inválidos.
   Se forem aceitos, inicia a aplicação e não retorna. */
uint32_t Bootloader_TryStartApplication(void);
#endif