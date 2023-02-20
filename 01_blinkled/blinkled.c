#include <stdint.h>

//---------------
// RCC Registers
//---------------

#define REG_RCC_CR     (volatile uint32_t*)(uintptr_t)0x40021000U // Clock Control Register
#define REG_RCC_CFGR   (volatile uint32_t*)(uintptr_t)0x40021004U // PLL Configuration Register
#define REG_RCC_AHBENR (volatile uint32_t*)(uintptr_t)0x40021014U // AHB1 Peripheral Clock Enable Register
#define REG_RCC_CFGR2  (volatile uint32_t*)(uintptr_t)0x4002102CU // Clock configuration register 2

//----------------
// GPIO Registers
//----------------

#define GPIOC_MODER (volatile uint32_t*)(uintptr_t)0x48000800U // GPIO port mode register
#define GPIOC_TYPER (volatile uint32_t*)(uintptr_t)0x48000804U // GPIO port output type register
#define GPIOС_ODR   (volatile uint32_t*)(uintptr_t)0x48000814U // GPIO port output data register

//---------------------
// Modifying Registers
//---------------------

#define GPIOC_REG_ADD_VALUE(REG, BIT, VALUE)  (*REG |= VALUE << (BIT))               // Adds VALUE in BIT of REG           
#define GPIOC_REG_SET_ZERO(REG, BIT)          (*REG &= ~(0b1U << (BIT)))             // Sets 0 in BIT of REG
#define GPIOC_REG_SET_ONE(REG, BIT)           (GPIOC_REG_ADD_VALUE(REG, BIT, 0b1U))  // Sets 1 in BIT of REG

//-----------
// GPIO LEDS
//-----------
#define LED_3_GREEN 9U
#define LED_4_BLUE  8U 

//---------------------
// GPIO Initialization
//---------------------
    // Modes:
        #define INPUT_M 0b00U
        #define GEN_PURP_OUT_M 0b01U
        #define ALT_FUNC_M 0b10U
        #define ANALOG_M 0b11U
    // Output types:
        #define PUSH_PULL_T 0b0U
        #define OPEN_DRAIN_T 0b1U


//------
// Main
//------

#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
#define ONE_MILLISECOND CPU_FREQENCY/1000U

void board_clocking_init()
{
    // (1) Clock HSE and wait for oscillations to setup.
    *REG_RCC_CR = 0x00010000U;
    while ((*REG_RCC_CR & 0x00020000U) != 0x00020000U);

    // (2) Configure PLL:
    // PREDIV output: HSE/2 = 4 MHz
    *REG_RCC_CFGR2 |= 1U;

    // (3) Select PREDIV output as PLL input (4 MHz):
    *REG_RCC_CFGR |= 0x00010000U;

    // (4) Set PLLMUL to 12:
    // SYSCLK frequency = 48 MHz
    *REG_RCC_CFGR |= (12U-2U) << 18U;

    // (5) Enable PLL:
    *REG_RCC_CR |= 0x01000000U;
    while ((*REG_RCC_CR & 0x02000000U) != 0x02000000U);

    // (6) Configure AHB frequency to 48 MHz:
    *REG_RCC_CFGR |= 0b000U << 4U;

    // (7) Select PLL as SYSCLK source:
    *REG_RCC_CFGR |= 0b10U;
    while ((*REG_RCC_CFGR & 0xCU) != 0x8U);

    // (8) Set APB frequency to 24 MHz
    *REG_RCC_CFGR |= 0b001U << 8U;
}

void board_gpio_init()
{
    // (1) Enable GPIOC clocking:
    GPIOC_REG_SET_ONE(REG_RCC_AHBENR, 19);

    // (2) Configure PC8 mode:
    GPIOC_REG_ADD_VALUE(GPIOC_MODER, 2*LED_4_BLUE, GEN_PURP_OUT_M);

    // (3) Configure PC8 type:
    GPIOC_REG_ADD_VALUE(GPIOC_TYPER, 2*LED_4_BLUE, PUSH_PULL_T);

    // (4) Configure PC9 mode:
    GPIOC_REG_ADD_VALUE(GPIOC_MODER, 2*LED_3_GREEN, GEN_PURP_OUT_M);

    // (5) Configure PC9 type:
    GPIOC_REG_ADD_VALUE(GPIOC_TYPER, 2*LED_3_GREEN, PUSH_PULL_T);
}

void totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms()
{
    for (uint32_t i = 0; i < 1000U * ONE_MILLISECOND; ++i)
    {
        // Insert NOP for power consumption:
        __asm__ volatile("nop");
    }
}

int main()
{
#ifndef INSIDE_QEMU
    board_clocking_init();
#endif

    board_gpio_init();

    while (1)
    {
        GPIOC_REG_SET_ONE(GPIOС_ODR, LED_4_BLUE);

        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();

        GPIOC_REG_SET_ZERO(GPIOС_ODR, LED_4_BLUE);

        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();

        GPIOC_REG_SET_ONE(GPIOС_ODR, LED_3_GREEN);

        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();

        GPIOC_REG_SET_ZERO(GPIOС_ODR, LED_3_GREEN);
    }
}
