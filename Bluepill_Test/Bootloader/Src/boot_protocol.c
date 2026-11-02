#include "boot_protocol.h"
#include "boot_config.h"
#include "boot_crc.h"
#include "boot_commands.h"
size_t BootProtocol_BuildFrame(
    uint8_t command,
    uint16_t sequence,
    const uint8_t *payload,
    uint16_t payload_length,
    uint8_t *output,
    size_t output_capacity)
{
    if ((output == NULL) ||
        (payload_length > BOOT_PROTOCOL_MAX_PAYLOAD) ||
        ((payload_length > 0U) && (payload == NULL)))
    {
        return 0U;
    }

    size_t frame_size =
        BOOT_PROTOCOL_MIN_FRAME_SIZE + (size_t)payload_length;

    if (output_capacity < frame_size)
    {
        return 0U;
    }

    output[0] = BOOT_PROTOCOL_SOF_1;
    output[1] = BOOT_PROTOCOL_SOF_2;
    output[2] = (uint8_t)BOOT_PROTOCOL_VERSION;
    output[3] = command;

    output[4] = (uint8_t)(sequence & 0xFFU);
    output[5] = (uint8_t)(sequence >> 8U);

    output[6] = (uint8_t)(payload_length & 0xFFU);
    output[7] = (uint8_t)(payload_length >> 8U);

    for (size_t i = 0U; i < payload_length; i++)
    {
        output[BOOT_PROTOCOL_HEADER_SIZE + i] = payload[i];
    }

    /* CRC covers version through the last payload byte. */
    uint32_t crc = BootCRC_Calculate(
        &output[2],
        (BOOT_PROTOCOL_HEADER_SIZE - 2U) + payload_length
    );

    size_t crc_offset =
        BOOT_PROTOCOL_HEADER_SIZE + (size_t)payload_length;

    output[crc_offset]      = (uint8_t)(crc & 0xFFU);
    output[crc_offset + 1U] = (uint8_t)((crc >> 8U) & 0xFFU);
    output[crc_offset + 2U] = (uint8_t)((crc >> 16U) & 0xFFU);
    output[crc_offset + 3U] = (uint8_t)(crc >> 24U);

    return frame_size;
}
Boot_FrameStatus BootProtocol_ParseFrame(
    const uint8_t *frame,
    size_t frame_size,
    Boot_FrameView *view)
{
    if ((frame == NULL) || (view == NULL))
    {
        return BOOT_FRAME_INVALID_ARGUMENT;
    }

    *view = (Boot_FrameView){0};

    if ((frame_size < BOOT_PROTOCOL_MIN_FRAME_SIZE) ||
        (frame_size > BOOT_PROTOCOL_MAX_FRAME_SIZE))
    {
        return BOOT_FRAME_INVALID_SIZE;
    }

    if ((frame[0] != BOOT_PROTOCOL_SOF_1) ||
        (frame[1] != BOOT_PROTOCOL_SOF_2))
    {
        return BOOT_FRAME_INVALID_MARKER;
    }

    if (frame[2] != BOOT_PROTOCOL_VERSION)
    {
        return BOOT_FRAME_INVALID_VERSION;
    }

    uint16_t payload_length =
        (uint16_t)((uint16_t)frame[6] |
                   ((uint16_t)frame[7] << 8U));

    if ((payload_length > BOOT_PROTOCOL_MAX_PAYLOAD) ||
        (frame_size !=
            BOOT_PROTOCOL_MIN_FRAME_SIZE + (size_t)payload_length))
    {
        return BOOT_FRAME_INVALID_SIZE;
    }

    size_t crc_offset =
        BOOT_PROTOCOL_HEADER_SIZE + (size_t)payload_length;

    uint32_t received_crc =
        (uint32_t)frame[crc_offset] |
        ((uint32_t)frame[crc_offset + 1U] << 8U) |
        ((uint32_t)frame[crc_offset + 2U] << 16U) |
        ((uint32_t)frame[crc_offset + 3U] << 24U);

    uint32_t calculated_crc = BootCRC_Calculate(
        &frame[2],
        (BOOT_PROTOCOL_HEADER_SIZE - 2U) + payload_length
    );

    if (received_crc != calculated_crc)
    {
        return BOOT_FRAME_INVALID_CRC;
    }

    view->command = frame[3];

    view->sequence =
        (uint16_t)((uint16_t)frame[4] |
                   ((uint16_t)frame[5] << 8U));

    view->payload_length = payload_length;
    view->payload = &frame[BOOT_PROTOCOL_HEADER_SIZE];

    return BOOT_FRAME_OK;
}

/* Builds a GET_INFO response from a validated request.
   Returns zero for unsupported or invalid requests.
   Request payload and output must not overlap. */
size_t BootProtocol_BuildInfoResponse(
    const Boot_FrameView *request,
    uint8_t *output,
    size_t output_capacity);

    static void BootProtocol_WriteU32LE(
    uint8_t *output,
    uint32_t value)
{
    output[0] = (uint8_t)(value & 0xFFU);
    output[1] = (uint8_t)((value >> 8U) & 0xFFU);
    output[2] = (uint8_t)((value >> 16U) & 0xFFU);
    output[3] = (uint8_t)(value >> 24U);
}

size_t BootProtocol_BuildInfoResponse(
    const Boot_FrameView *request,
    uint8_t *output,
    size_t output_capacity)
{
    if ((request == NULL) ||
        (request->command != BOOT_CMD_GET_INFO) ||
        (request->payload_length != 0U))
    {
        return 0U;
    }

    Boot_Info info = {0};

    Boot_CommandStatus status = BootCommands_Execute(
        BOOT_CMD_GET_INFO,
        &info
    );

    if (status != BOOT_CMD_OK)
    {
        return 0U;
    }

    uint8_t payload[29] = {0};

    /* Wire status: 0 means success. */
    payload[0] = 0U;

    BootProtocol_WriteU32LE(&payload[1],  info.boot_version_major);
    BootProtocol_WriteU32LE(&payload[5],  info.boot_version_minor);
    BootProtocol_WriteU32LE(&payload[9],  info.boot_version_patch);
    BootProtocol_WriteU32LE(&payload[13], info.protocol_version);
    BootProtocol_WriteU32LE(&payload[17], info.target_id);
    BootProtocol_WriteU32LE(&payload[21], info.app_address);
    BootProtocol_WriteU32LE(&payload[25], info.app_capacity);

    return BootProtocol_BuildFrame(
        (uint8_t)(request->command | BOOT_PROTOCOL_RESPONSE_FLAG),
        request->sequence,
        payload,
        (uint16_t)sizeof(payload),
        output,
        output_capacity
    );
}