#include "boot_receiver.h"

void BootReceiver_Init(Boot_Receiver *receiver)
{
    if (receiver == NULL)
    {
        return;
    }

    receiver->count = 0U;
    receiver->expected_size = 0U;
    receiver->last_status = BOOT_FRAME_OK;
}

Boot_RxResult BootReceiver_PushByte(
    Boot_Receiver *receiver,
    uint8_t byte,
    Boot_FrameView *view)
{
    if ((receiver == NULL) || (view == NULL))
    {
        return BOOT_RX_FRAME_REJECTED;
    }

    if (receiver->count == 0U)
    {
        if (byte == BOOT_PROTOCOL_SOF_1)
        {
            receiver->buffer[0] = byte;
            receiver->count = 1U;
        }

        return BOOT_RX_WAITING;
    }

    if (receiver->count == 1U)
    {
        if (byte == BOOT_PROTOCOL_SOF_2)
        {
            receiver->buffer[1] = byte;
            receiver->count = 2U;
        }
        else if (byte != BOOT_PROTOCOL_SOF_1)
        {
            receiver->count = 0U;
        }

        /* A repeated first marker keeps us waiting
           for the second marker. */
        return BOOT_RX_WAITING;
    }

    if (receiver->count >= sizeof(receiver->buffer))
    {
        receiver->count = 0U;
        receiver->expected_size = 0U;
        receiver->last_status = BOOT_FRAME_INVALID_SIZE;
        return BOOT_RX_FRAME_REJECTED;
    }

    receiver->buffer[receiver->count++] = byte;

    if (receiver->count == BOOT_PROTOCOL_HEADER_SIZE)
    {
        uint16_t payload_length =
            (uint16_t)(
                (uint16_t)receiver->buffer[6] |
                ((uint16_t)receiver->buffer[7] << 8U)
            );

        if (payload_length > BOOT_PROTOCOL_MAX_PAYLOAD)
        {
            receiver->count = 0U;
            receiver->expected_size = 0U;
            receiver->last_status = BOOT_FRAME_INVALID_SIZE;
            return BOOT_RX_FRAME_REJECTED;
        }

        receiver->expected_size =
            BOOT_PROTOCOL_MIN_FRAME_SIZE + (size_t)payload_length;
    }

    if ((receiver->expected_size != 0U) &&
        (receiver->count == receiver->expected_size))
    {
        receiver->last_status = BootProtocol_ParseFrame(
            receiver->buffer,
            receiver->count,
            view
        );

        receiver->count = 0U;
        receiver->expected_size = 0U;

        return (receiver->last_status == BOOT_FRAME_OK)
            ? BOOT_RX_FRAME_READY
            : BOOT_RX_FRAME_REJECTED;
    }

    return BOOT_RX_WAITING;
}