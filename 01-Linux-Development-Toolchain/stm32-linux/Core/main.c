#include <stdint.h>

/* NUCLEO-F446RE: onboard green LD2 is wired to PA5. */
#define RCC_AHB1ENR  (*(volatile uint32_t *)0x40023830UL)
#define GPIOA_MODER  (*(volatile uint32_t *)0x40020000UL)
#define GPIOA_OTYPER (*(volatile uint32_t *)0x40020004UL)
#define GPIOA_PUPDR  (*(volatile uint32_t *)0x4002000CUL)
#define GPIOA_BSRR   (*(volatile uint32_t *)0x40020018UL)

#define LD2_PIN 5U
#define LD2_MASK (1UL << LD2_PIN)

static void delay(volatile uint32_t count)
{
    while (count-- != 0U) {
        __asm volatile("nop");
    }
}

int main(void)
{
    RCC_AHB1ENR |= 1UL;            /* Enable GPIOA bus clock. */
    (void)RCC_AHB1ENR;

    GPIOA_MODER &= ~(3UL << (LD2_PIN * 2U));
    GPIOA_MODER |=  (1UL << (LD2_PIN * 2U));
    GPIOA_OTYPER &= ~LD2_MASK;
    GPIOA_PUPDR &= ~(3UL << (LD2_PIN * 2U));
    GPIOA_BSRR = LD2_MASK << 16U;  /* Start LOW. */

    for (;;) {
        GPIOA_BSRR = LD2_MASK;          /* LED ON */
        delay(500000U);
        GPIOA_BSRR = LD2_MASK << 16U;   /* LED OFF */
        delay(500000U);
    }
}
