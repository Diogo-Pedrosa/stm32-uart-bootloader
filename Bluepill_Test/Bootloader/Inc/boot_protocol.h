#ifndef BOOT_PROTOCOL_H
#define BOOT_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#define BOOT_PROTOCOL_SOF_1              0xAAU
#define BOOT_PROTOCOL_SOF_2              0x55U

#define BOOT_PROTOCOL_RESPONSE_FLAG      0x80U

#define BOOT_PROTOCOL_MAX_PAYLOAD        256U

/* Marker + version + command + sequence + payload length. */
#define BOOT_PROTOCOL_HEADER_SIZE        8U
#define BOOT_PROTOCOL_CRC_SIZE           4U

#define BOOT_PROTOCOL_MIN_FRAME_SIZE \
    (BOOT_PROTOCOL_HEADER_SIZE + BOOT_PROTOCOL_CRC_SIZE)

#define BOOT_PROTOCOL_MAX_FRAME_SIZE \
    (BOOT_PROTOCOL_MIN_FRAME_SIZE + BOOT_PROTOCOL_MAX_PAYLOAD)

/* Returns the frame size, or zero for invalid arguments. */
size_t BootProtocol_BuildFrame(
    uint8_t command,
    uint16_t sequence,
    const uint8_t *payload,
    uint16_t payload_length,
    uint8_t *output,
    size_t output_capacity);
typedef enum
{
    BOOT_FRAME_OK = 0,
    BOOT_FRAME_INVALID_ARGUMENT,
    BOOT_FRAME_INVALID_SIZE,
    BOOT_FRAME_INVALID_MARKER,
    BOOT_FRAME_INVALID_VERSION,
    BOOT_FRAME_INVALID_CRC
} Boot_FrameStatus;

typedef struct
{
    uint8_t command;
    uint16_t sequence;
    uint16_t payload_length;
    const uint8_t *payload;
} Boot_FrameView;
size_t BootProtocol_BuildInfoResponse(
    const Boot_FrameView *request,
    uint8_t *output,
    size_t output_capacity);
Boot_FrameStatus BootProtocol_ParseFrame(
    const uint8_t *frame,
    size_t frame_size,
    Boot_FrameView *view);

#endif