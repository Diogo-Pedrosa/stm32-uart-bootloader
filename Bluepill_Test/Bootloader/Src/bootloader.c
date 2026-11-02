#include "bootloader.h"
#include "boot_config.h"
#include "boot_port.h"
volatile uint32_t app_stack = 0;
volatile uint32_t app_reset = 0;
volatile uint32_t app_vectors_ok = 0;

uint32_t Bootloader_CheckApplicationVectors(void)
{
    app_vectors_ok = 0U;

    app_stack =
        *(volatile const uint32_t *)BOOT_APP_ADDRESS;

    app_reset =
        *(volatile const uint32_t *)(BOOT_APP_ADDRESS + 4U);

    if ((app_stack <= BOOT_RAM_START_ADDRESS) ||
        (app_stack > BOOT_RAM_END_ADDRESS) ||
        ((app_stack & 0x7U) != 0U))
    {
        return 0U;
    }

    if ((app_reset & 1U) == 0U)
    {
        return 0U;
    }

    uint32_t reset_address = app_reset & ~1U;

    if ((reset_address < BOOT_APP_ADDRESS) ||
        (reset_address >= BOOT_FLASH_END_ADDRESS))
    {
        return 0U;
    }

    app_vectors_ok = 1U;
    return 1U;
}uint32_t Bootloader_TryStartApplication(void)
{
    if (Bootloader_CheckApplicationVectors() == 0U)
    {
        return 0U;
    }

    Boot_PortStartApplication(BOOT_APP_ADDRESS);
}