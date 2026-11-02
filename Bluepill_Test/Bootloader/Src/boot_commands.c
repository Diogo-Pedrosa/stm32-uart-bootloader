#include "boot_commands.h"
#include "boot_config.h"
#include "bootloader.h"
void BootCommands_GetInfo(volatile Boot_Info *info)
{
    if (info == 0)
    {
        return;
    }

    info->boot_version_major = BOOT_VERSION_MAJOR;
    info->boot_version_minor = BOOT_VERSION_MINOR;
    info->boot_version_patch = BOOT_VERSION_PATCH;
    info->protocol_version = BOOT_PROTOCOL_VERSION;
    info->target_id = BOOT_TARGET_ID;
    info->app_address = BOOT_APP_ADDRESS;
    info->app_capacity =
        BOOT_FLASH_END_ADDRESS - BOOT_APP_ADDRESS;
}


Boot_CommandStatus BootCommands_Execute(
    uint32_t command,
    volatile Boot_Info *info)
{
    switch (command)
    {
        case BOOT_CMD_GET_INFO:
            if (info == 0)
            {
                return BOOT_CMD_INVALID_ARGUMENT;
            }

            BootCommands_GetInfo(info);
            return BOOT_CMD_OK;

        case BOOT_CMD_RUN_APP:
            /* Se iniciar a aplicação, não retorna.
               Se retornar, os vetores foram rejeitados. */
            Bootloader_TryStartApplication();
            return BOOT_CMD_INVALID_APPLICATION;

        default:
            return BOOT_CMD_UNKNOWN;
    }
}