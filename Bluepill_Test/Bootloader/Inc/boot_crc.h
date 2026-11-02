#ifndef BOOT_CRC_H
#define BOOT_CRC_H

#include <stddef.h>
#include <stdint.h>

/* data must point to at least length readable bytes.
   A null pointer is allowed only when length is zero. */
uint32_t BootCRC_Calculate(const uint8_t *data, size_t length);

#endif