#include <stdint.h>
#include <stdbool.h>
#include "reg_config.h"

#define P1 1
#define P2 2
#define NO_ONE 3

//-------------------
// 7-segment display
//-------------------
    // Pin Mapping:
        #define A  0x8000U
        #define B  0x0080U
        #define C  0x0010U
        #define D  0x0004U
        #define E  0x0002U
        #define F  0x0400U
        #define G  0x0020U
        #define DP 0x0008U

        #define POS0 0x0040U
        #define POS1 0x0100U
        #define POS2 0x0200U
        #define POS3 0x1000U

        static const uint32_t PINS_USED = A|B|C|D|E|F|G|DP|POS0|POS1|POS2|POS3;

    // Digit composition:
        #define P 10

        static const uint32_t DIGITS[11] =
        {
            A|B|C|D|E|F,   // 0
            B|C,           // 1
            A|B|G|E|D,     // 2
            A|B|C|D|G,     // 3
            B|C|F|G,       // 4
            A|F|G|C|D,     // 5
            A|C|D|E|F|G,   // 6
            A|B|C,         // 7
            A|B|C|D|E|F|G, // 8
            A|B|C|D|F|G,   // 9
            A|B|E|F|G      // P
        };

        static const uint32_t POSITIONS[4] =
        {
                POS1|POS2|POS3,  // 0
            POS0     |POS2|POS3, // 1
            POS0|POS1     |POS3, // 2
            POS0|POS1|POS2       // 3
        };

    // Display state:
        struct Seg7Display
        {
            uint32_t display;
            uint16_t number;
        };

        void SEG7_set_number_pos(struct Seg7Display* seg7, unsigned pos)
        {
            seg7->display = DIGITS[seg7->number] | POSITIONS[pos];
        }

    // Write changes to microcontroller:
        void SEG7_push_display_state_to_mc(struct Seg7Display* seg7)
        {
            uint32_t surrounding_state = ~PINS_USED & *GPIOA_ODR;
            uint32_t to_write = PINS_USED & seg7->display;

            *GPIOA_ODR = surrounding_state | to_write;
        }

//-------------------
// RCC configuration
//-------------------
    #define CPU_FREQENCY 48000000U                  // CPU frequency: 48 MHz
    #define ONE_MILLISECOND CPU_FREQENCY/1000U

    void board_clocking_init()
    {
        // (1) Clock HSE and wait for oscillations to setup:
            REG_ADD_VALUE(REG_RCC_CR, HSEON, ON);
            while((*REG_RCC_CR & (RDY << HSERDY)) != (RDY << HSERDY));

        // (2) Configure PLL: Nice
            REG_ADD_VALUE(REG_RCC_CFGR2, PREDIV, PREDIV_BY_2);

        // (3) Select PREDIV output as PLL input (4 MHz):
            REG_ADD_VALUE(REG_RCC_CFGR, PLLSRC, HSI_BY_PREDIV);

        // (4) Set PLLMUL to 12 (SYSCLK frequency = 48 MHz):
            REG_ADD_VALUE(REG_RCC_CFGR, PLL_MUL, X_12);

        // (5) Enable PLL:
            REG_ADD_VALUE(REG_RCC_CR, PLLON, ON);
            while ((*REG_RCC_CR & (PLL_L << PLLRDY)) != (PLL_L << PLLRDY));

        // (6) Configure AHB frequency to 48 MHz:
            REG_ADD_VALUE(REG_RCC_CFGR, HPRE, SYS_CLK_NOT_DIV);

        // (7) Select PLL as SYSCLK source:
            REG_ADD_VALUE(REG_RCC_CFGR, SW, PLL_SYS_CLK);
            while ((*REG_RCC_CFGR & (0b11U << SWS)) != (PLL_SYS_CLK << SWS));
            
        // (8) Set APB frequency to 24 MHz:
            REG_ADD_VALUE(REG_RCC_CFGR, PPRE, HCLK_NOT_DIV);
    }

    void to_get_more_accuracy_pay_2202_2013_2410_3805_1ms()
    {
        for (uint32_t i = 0; i < ONE_MILLISECOND/3U; ++i)
        {
            // Insert NOP for power consumption:
            __asm__ volatile("nop");
        }
    }

//--------------------
// GPIO configuration
//--------------------
    void board_gpio_init()
    {
        // Configure Port A:
            REG_SET_ONE(REG_RCC_AHBENR, IOPA_EN);

            // Configure mode register:
                // Modes for the first 12 pins 
                    for (int i = 1; i <= 12; i++) {
                        REG_ADD_VALUE(GPIOA_MODER, 2*i, GEN_PURP_OUT_M);
                    }
                // Mode for the 15 pins
                    REG_ADD_VALUE(GPIOA_MODER, 2*15, GEN_PURP_OUT_M);

            // Configure type register:
                REG_SET_ZERO_ALL(GPIOA_OTYPER);

            // Configure PA0 as button:
                REG_ADD_VALUE(GPIOA_MODER, 0, INPUT_M);
                REG_ADD_VALUE(GPIOA_PUPDR, 0, PULL_DOWN);
        
        // Configure Port C:
            REG_SET_ONE(REG_RCC_AHBENR, IOPC_EN);

            // Configure PC8:
                REG_ADD_VALUE(GPIOC_MODER, 2*LED_4_BLUE, GEN_PURP_OUT_M);
                REG_ADD_VALUE(GPIOC_OTYPER, 2*LED_4_BLUE, PUSH_PULL_T);

            // Configure PC9:
                REG_ADD_VALUE(GPIOC_MODER, 2*LED_3_GREEN, GEN_PURP_OUT_M);
                REG_ADD_VALUE(GPIOC_OTYPER, 2*LED_3_GREEN, PUSH_PULL_T);

            // Configure PC5 as button:
                REG_ADD_VALUE(GPIOC_MODER, 2*5, INPUT_M);
                REG_ADD_VALUE(GPIOС_PUPDR, 2*5, PULL_DOWN);
            
            // Configure PC5 as button:
                REG_ADD_VALUE(GPIOC_MODER, 2*4, INPUT_M);
                REG_ADD_VALUE(GPIOС_PUPDR, 2*4, PULL_DOWN);
            
    }

void end(int winner) {
    struct Seg7Display p = 
    {
        .number = P
    };

    int tick = 0;
    
    if(winner == 1) {
        struct Seg7Display num_1 =
        {
            .number = 1
        };

        while(1) {
            SEG7_set_number_pos(&num_1, 2);
            SEG7_push_display_state_to_mc(&num_1);

            SEG7_set_number_pos(&p, 3);
            SEG7_push_display_state_to_mc(&p);

            if (tick%20000 == 0) {
                REG_SET_ONE(GPIOС_ODR, LED_3_GREEN);
            }

            if (tick%20000 == 10000) {
                REG_SET_ZERO(GPIOС_ODR, LED_3_GREEN);
            }
            if (tick%100000 == 0) {
                REG_SET_ONE(GPIOС_ODR, LED_4_BLUE);
            }

            if (tick%100000 == 50000) {
                REG_SET_ZERO(GPIOС_ODR, LED_4_BLUE);
            }

            tick++;
        }
    }

    else {
        struct Seg7Display num_2 =
        {
            .number = 2
        };
        
        while(1) {
            SEG7_set_number_pos(&num_2, 0);
            SEG7_push_display_state_to_mc(&num_2);

            SEG7_set_number_pos(&p, 1);
            SEG7_push_display_state_to_mc(&p);

            if (tick%20000 == 0) {
                REG_SET_ONE(GPIOС_ODR, LED_4_BLUE);
            }

            if (tick%20000 == 10000) {
                REG_SET_ZERO(GPIOС_ODR, LED_4_BLUE);
            }
            if (tick%100000 == 0) {
                REG_SET_ONE(GPIOС_ODR, LED_3_GREEN);
            }

            if (tick%100000 == 50000) {
                REG_SET_ZERO(GPIOС_ODR, LED_3_GREEN);
            }
            
            tick++;
        }
    }
    

}
    

//------
// Main
//------
    int main()
    {
        board_clocking_init();

        board_gpio_init();

        // Init display rendering:
        struct Seg7Display p1 =
        {
            .number = 0
        };

        struct Seg7Display p2 =
        {
            .number = 0
        };

        uint32_t saturation1 = 0U;
        uint32_t saturation2 = 0U;

        int ownership = NO_ONE; 

        while (1)
        {
            bool active1 = *GPIOC_IDR & (1U << 4U);
            bool active2 = *GPIOC_IDR & (1U << 5U);

            if (active1)
            {
                if (saturation1 < 5U)
                {
                    saturation1 += 1U;
                }
                else
                {
                    p1.number = 1;
                    if(ownership == P2) {
                        end(P2);
                    }
                    else ownership = P1;
                }
            }
            else
            {
                saturation1 = 0U;
                ownership = NO_ONE;
            }

            if (active2)
            {
                if (saturation2 < 5U)
                {
                    saturation2 += 1U;
                }
                else
                {
                    p2.number = 1;
                    if(ownership == P1) {
                        end(P1);
                    }
                    else ownership = P2;
                }
            }
            else
            {
                saturation2 = 0U;
                ownership = NO_ONE;
            }

            SEG7_set_number_pos(&p1, 3);
            SEG7_push_display_state_to_mc(&p1);

            SEG7_set_number_pos(&p2, 0);
            SEG7_push_display_state_to_mc(&p2);

            p1.number = 0;
            p2.number = 0;
        }
    }
