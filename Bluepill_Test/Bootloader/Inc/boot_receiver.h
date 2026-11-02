#ifndef BOOT_RECEIVER_H
#define BOOT_RECEIVER_H

#include "boot_protocol.h"

typedef enum
{
    BOOT_RX_WAITING = 0,
    BOOT_RX_FRAME_READY,
    BOOT_RX_FRAME_REJECTED
} Boot_RxResult;

typedef struct
{
    uint8_t buffer[BOOT_PROTOCOL_MAX_FRAME_SIZE];
    size_t count;
    size_t expected_size;
    Boot_FrameStatus last_status;
} Boot_Receiver;

void BootReceiver_Init(Boot_Receiver *receiver);

/* Use view only after BOOT_RX_FRAME_READY.
   Its payload remains valid until the next received byte
   or until the receiver is initialized again. */
Boot_RxResult BootReceiver_PushByte(
    Boot_Receiver *receiver,
    uint8_t byte,
    Boot_FrameView *view);

#endif