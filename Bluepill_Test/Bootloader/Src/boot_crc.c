#include "boot_crc.h"

/* CRC-32/ISO-HDLC:
   reflected polynomial: 0xEDB88320
   initial value:        0xFFFFFFFF
   final XOR:            0xFFFFFFFF */
uint32_t BootCRC_Calculate(const uint8_t *data, size_t length)
{
    uint32_t crc = 0xFFFFFFFFU;

    for (size_t i = 0U; i < length; i++)
    {
        crc ^= data[i];

        for (uint32_t bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 1U) != 0U)
            {
                crc = (crc >> 1U) ^ 0xEDB88320U;
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    return crc ^ 0xFFFFFFFFU;
}