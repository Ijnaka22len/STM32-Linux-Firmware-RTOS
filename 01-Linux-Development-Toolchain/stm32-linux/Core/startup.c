#include <stdint.h>

extern int main(void);
extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;

void Reset_Handler(void);
void Default_Handler(void);

#define WEAK_DEFAULT(name) void name(void) __attribute__((weak, alias("Default_Handler")))
WEAK_DEFAULT(NMI_Handler);
WEAK_DEFAULT(HardFault_Handler);
WEAK_DEFAULT(MemManage_Handler);
WEAK_DEFAULT(BusFault_Handler);
WEAK_DEFAULT(UsageFault_Handler);
WEAK_DEFAULT(SVC_Handler);
WEAK_DEFAULT(DebugMon_Handler);
WEAK_DEFAULT(PendSV_Handler);
WEAK_DEFAULT(SysTick_Handler);

/* Minimal core-vector example; external IRQs default to a trap.
 * This is not the official STM32F446RE vector table. For FreeRTOS,
 * interrupts or peripherals, use ST's complete startup vector table. */
__attribute__((section(".isr_vector"), used, aligned(256)))
const uintptr_t vector_table[16 + 128] = {
    [0] = (uintptr_t)&_estack,
    [1] = (uintptr_t)Reset_Handler,
    [2] = (uintptr_t)NMI_Handler,
    [3] = (uintptr_t)HardFault_Handler,
    [4] = (uintptr_t)MemManage_Handler,
    [5] = (uintptr_t)BusFault_Handler,
    [6] = (uintptr_t)UsageFault_Handler,
    [11] = (uintptr_t)SVC_Handler,
    [12] = (uintptr_t)DebugMon_Handler,
    [14] = (uintptr_t)PendSV_Handler,
    [15] = (uintptr_t)SysTick_Handler,
    [16 ... 143] = (uintptr_t)Default_Handler
};

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
    while (1) { __asm volatile("nop"); }
}

void Default_Handler(void)
{
    while (1) { __asm volatile("nop"); }
}
