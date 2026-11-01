/**
 * @file main.c
 * @brief Register-level GPIO output demonstration on NUCLEO-F446RE.
 *
 * The onboard LD2 LED is connected to GPIOA pin 5. This application
 * configures PA5 without HAL and alternates the output using GPIOx_BSRR.
 * The busy-wait delay is not a calibrated timebase.
 */

#include <stdint.h>

/* STM32F446RE peripheral register addresses. */
#define RCC_AHB1ENR   (*(volatile uint32_t *)0x40023830UL)
#define GPIOA_MODER   (*(volatile uint32_t *)0x40020000UL)
#define GPIOA_OTYPER  (*(volatile uint32_t *)0x40020004UL)
#define GPIOA_PUPDR   (*(volatile uint32_t *)0x4002000CUL)
#define GPIOA_BSRR    (*(volatile uint32_t *)0x40020018UL)

#define LD2_PIN       5U
#define LD2_MASK      (1UL << LD2_PIN)
#define DELAY_COUNT   500000U

/** Execute an approximate delay using a volatile decrement loop. */
static void delay(volatile uint32_t count)
{
    while (count-- != 0U) {
        __asm volatile ("nop");
    }
}

/** Configure GPIOA pin 5 as a push-pull output, initially low. */
static void ld2_init(void)
{
    RCC_AHB1ENR |= 1UL;                    /* Enable GPIOA clock. */
    (void)RCC_AHB1ENR;                     /* Complete peripheral clock write. */

    GPIOA_MODER &= ~(3UL << (LD2_PIN * 2U));
    GPIOA_MODER |=  (1UL << (LD2_PIN * 2U));
    GPIOA_OTYPER &= ~LD2_MASK;            /* Push-pull output. */
    GPIOA_PUPDR &= ~(3UL << (LD2_PIN * 2U)); /* No pull resistors. */
    GPIOA_BSRR = (LD2_MASK << 16U);       /* Reset PA5 output. */
}

int main(void)
{
    ld2_init();

    for (;;) {
        GPIOA_BSRR = LD2_MASK;            /* Set PA5: LED on. */
        delay(DELAY_COUNT);
        GPIOA_BSRR = (LD2_MASK << 16U);   /* Reset PA5: LED off. */
        delay(DELAY_COUNT);
    }
}
