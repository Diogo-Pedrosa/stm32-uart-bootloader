#include "boot_port.h"
#include "stm32f1xx_hal.h"

/* GCC ARM: r0 recebe o endereço da aplicação.
   Somente Assembly após trocar a pilha. */
__attribute__((naked, noreturn))
static void Boot_EnterApplication(
    uint32_t app_address __attribute__((unused)))
{
    __asm volatile(
        "ldr r1, [r0, #4] \n"
        "ldr r0, [r0]     \n"
        "msr msp, r0      \n"
        "cpsie i          \n"
        "bx r1            \n"
    );
}

__attribute__((noreturn))
void Boot_PortStartApplication(uint32_t app_address)
{
    HAL_RCC_DeInit();
    HAL_DeInit();

    __disable_irq();

    SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL = 0U;

    for (uint32_t i = 0U; i < 2U; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFFU;
        NVIC->ICPR[i] = 0xFFFFFFFFU;
    }

    SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk |
                SCB_ICSR_PENDSVCLR_Msk;

    SCB->VTOR = app_address;

    __DSB();
    __ISB();

    Boot_EnterApplication(app_address);
}