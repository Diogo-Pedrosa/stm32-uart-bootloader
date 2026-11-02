#ifndef BOOT_PORT_H
#define BOOT_PORT_H

#include <stdint.h>

/* Chamar pelo main, em modo privilegiado e usando MSP.
   A função transfere a execução e não retorna. */
__attribute__((noreturn))
void Boot_PortStartApplication(uint32_t app_address);

#endif