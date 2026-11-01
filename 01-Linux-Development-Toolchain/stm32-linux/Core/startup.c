/**
 * @file startup.c
 * @brief Minimal Cortex-M4 reset path for a polling-only demonstration.
 *
 * The vector table provides the core exception entries and directs all
 * external IRQs to Default_Handler. It is not the complete STM32F446RE
 * interrupt dispatch table and is not intended for FreeRTOS applications.
 */

#include <stdint.h>

extern int main(void);
extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;

void Reset_Handler(void);
void Default_Handler(void);

#define WEAK_DEFAULT(handler) \
    void handler(void) __attribute__((weak, alias("Default_Handler")))

WEAK_DEFAULT(NMI_Handler);
WEAK_DEFAULT(HardFault_Handler);
WEAK_DEFAULT(MemManage_Handler);
WEAK_DEFAULT(BusFault_Handler);
WEAK_DEFAULT(UsageFault_Handler);
WEAK_DEFAULT(SVC_Handler);
WEAK_DEFAULT(DebugMon_Handler);
WEAK_DEFAULT(PendSV_Handler);
WEAK_DEFAULT(SysTick_Handler);

/* Reserve core vectors and external IRQ slots for this minimal example. */
__attribute__((section(".isr_vector"), used, aligned(256)))
const uintptr_t vector_table[16 + 128] = {
    [0]  = (uintptr_t)&_estack,
    [1]  = (uintptr_t)Reset_Handler,
    [2]  = (uintptr_t)NMI_Handler,
    [3]  = (uintptr_t)HardFault_Handler,
    [4]  = (uintptr_t)MemManage_Handler,
    [5]  = (uintptr_t)BusFault_Handler,
    [6]  = (uintptr_t)UsageFault_Handler,
    [11] = (uintptr_t)SVC_Handler,
    [12] = (uintptr_t)DebugMon_Handler,
    [14] = (uintptr_t)PendSV_Handler,
    [15] = (uintptr_t)SysTick_Handler,
    [16 ... 143] = (uintptr_t)Default_Handler
};

/** Initialize the C data sections before entering the application. */
void Reset_Handler(void)
{
    uint32_t *src = &_sidata;

    for (uint32_t *dst = &_sdata; dst < &_edata;) {
        *dst++ = *src++;
    }

    for (uint32_t *dst = &_sbss; dst < &_ebss;) {
        *dst++ = 0U;
    }

    (void)main();

    /* An embedded application is not expected to return from main. */
    for (;;) {
        __asm volatile ("nop");
    }
}

/** Trap unimplemented exceptions and interrupts for debugger inspection. */
void Default_Handler(void)
{
    for (;;) {
        __asm volatile ("nop");
    }
}
