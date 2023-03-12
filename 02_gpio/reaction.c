#include "reg_config.h"

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
        static const uint32_t DIGITS[10] =
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
            A|B|C|D|F|G    // 9
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

        void SEG7_set_number_quarter(struct Seg7Display* seg7, unsigned tick)
        {
            uint32_t divisors[4] = {1, 10, 100, 1000};

            unsigned quarter = tick % 4;
            unsigned divisor = divisors[quarter];

            seg7->display = DIGITS[(seg7->number / divisor) % 10] | POSITIONS[quarter];
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
        // (1) Configure PA1-PA12 as output:
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

        // (2) Configure PA0 as button:
            REG_ADD_VALUE(GPIOA_MODER, 0, INPUT_M);

        // Configure PA0 as pull-down pin:
            REG_ADD_VALUE(GPIOA_PUPDR, 0, PULL_DOWN);
    }

//------
// Main
//------
    int main()
    {
        board_clocking_init();

        board_gpio_init();

        // Init display rendering:
        struct Seg7Display seg7 =
        {
            .number = 0
        };

        uint32_t tick = 0;
        bool button_was_pressed = 0U;
        uint32_t saturation = 0U;

        while (1)
        {
            // Update button state:
            bool active = *GPIOA_IDR & (1U << 0U);

            if (active)
            {
                if (saturation < 5U)
                {
                    saturation += 1U;
                }
                else
                {
                    button_was_pressed = 1U;
                }
            }
            else
            {
                saturation = 0U;
            }

            // Update display state:
            if (!button_was_pressed && (tick % 10U) == 0U)
            {
                if (seg7.number < 9999U)
                {
                    seg7.number = seg7.number + 1U;
                }
            }

            // Render display state:
            SEG7_set_number_quarter(&seg7, tick);

            SEG7_push_display_state_to_mc(&seg7);

            // Adjust ticks every ms:
            to_get_more_accuracy_pay_2202_2013_2410_3805_1ms();
            tick += 1;
        }
    }
