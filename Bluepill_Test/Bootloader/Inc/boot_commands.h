#ifndef BOOT_COMMANDS_H
#define BOOT_COMMANDS_H

#include <stdint.h>

typedef struct
{
    uint32_t boot_version_major;
    uint32_t boot_version_minor;
    uint32_t boot_version_patch;
    uint32_t protocol_version;
    uint32_t target_id;
    uint32_t app_address;
    uint32_t app_capacity;
} Boot_Info;

void BootCommands_GetInfo(volatile Boot_Info *info);
typedef enum
{
    BOOT_CMD_GET_INFO = 0x01U,
    BOOT_CMD_RUN_APP  = 0x02U
} Boot_Command;

typedef enum
{
    BOOT_CMD_OK = 0,
    BOOT_CMD_UNKNOWN,
    BOOT_CMD_INVALID_ARGUMENT,
    BOOT_CMD_INVALID_APPLICATION
} Boot_CommandStatus;

Boot_CommandStatus BootCommands_Execute(
    uint32_t command,
    volatile Boot_Info *info);
#endif