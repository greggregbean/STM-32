//---------------------
// Modifying Registers
//---------------------
    #define REG_ADD_VALUE(REG, BIT, VALUE)  (*(REG) |= (VALUE) << (BIT))             // Adds VALUE in BIT of REG 
    #define REG_SET_VALUE(REG, VALUE)       (*(REG) = (VALUE))                       // Sets VALUE to REG
    #define REG_SET_ZERO(REG, BIT)          (*(REG) &= ~(0b1U << (BIT)))             // Sets 0 in BIT of REG
    #define REG_SET_ONE(REG, BIT)           (REG_ADD_VALUE(REG, BIT, 0b1U))          // Sets 1 in BIT of REG
    #define REG_SET_ZERO_ALL(REG)           (*(REG) = 0U)                            // Sets 0 in all bits of REG

//---------------
// RCC Registers
//---------------
    #define REG_RCC_CR     (volatile uint32_t*)(uintptr_t)0x40021000U // Clock Control Register
    #define REG_RCC_CFGR   (volatile uint32_t*)(uintptr_t)0x40021004U // PLL Configuration Register
    #define REG_RCC_AHBENR (volatile uint32_t*)(uintptr_t)0x40021014U // AHB1 Peripheral Clock Enable Register
    #define REG_RCC_CFGR2  (volatile uint32_t*)(uintptr_t)0x4002102CU // Clock configuration register 2

    // RCC_CR bits:
        #define HSERDY 17U // HSE clock ready flag
        //-------------------------------------------------
            #define N_RDY 0U  // HSE oscillator not ready
            #define RDY   1U  // HSE oscillator ready
        //-------------------------------------------------

        #define HSEON 16U // HSE clock enable
        //--------------------------------------
            #define OFF 0U // HSE oscillator OFF
            #define ON  1U // HSE oscillator ON
        //--------------------------------------

        #define PLLON 24U  // PLL Enable
        //----------------------------
            #define OFF 0U // PLL OFF
            #define ON  1U // PLL ON
        //----------------------------

        #define PLLRDY 25U // PLL clock ready flag
        //-------------------------------------
            #define PLL_UNL 0U // PLL unlocked
            #define PLL_L   1U // PLL locked
        //-------------------------------------

    // RCC_CFGR bits:
        #define PLL_MUL 18U // PLL multiplication factor
        //----------------------------------------------
            #define X_2  0b0000U // PLL input clock x 2
            #define X_3  0b0001U // PLL input clock x 3
            #define X_4  0b0010U // PLL input clock x 4
            #define X_5  0b0011U // PLL input clock x 5
            #define X_6  0b0100U // PLL input clock x 6
            #define X_7  0b0101U // PLL input clock x 7
            #define X_8  0b0110U // PLL input clock x 8
            #define X_9  0b0111U // PLL input clock x 9
            #define X_10 0b1000U // PLL input clock x 10
            #define X_11 0b1001U // PLL input clock x 11
            #define X_12 0b1010U // PLL input clock x 12
            #define X_13 0b1011U // PLL input clock x 13
            #define X_14 0b1100U // PLL input clock x 14
            #define X_15 0b1101U // PLL input clock x 15
            #define X_16 0b1110U // PLL input clock x 16
        //----------------------------------------------

        #define PLLSRC 16U // HSE divider for PLL input clock
        //----------------------------------------------------------------------------
            #define HSI_BY_2        0b00U // HSI/2 selected as PLL input clock 
            #define HSI_BY_PREDIV   0b01U // HSI/PREDIV selected as PLL input clock
            #define HSE_BY_PREDIV   0b10U // HSE/PREDIV selected as PLL input clock
            #define HSI48_BY_PREDIV 0b11U // HSI48/PREDIV selected as PLL input clock
        //----------------------------------------------------------------------------

        #define PPRE 8U // PCLK prescaler
        //------------------------------------------------------
            #define HCLK_NOT_DIV 0U << 2U // HCLK not divided
            #define HCLK_BY_2    0b100U   // HCLK divided by 2
            #define HCLK_BY_4    0b101U   // HCLK divided by 4
            #define HCLK_BY_8    0b110U   // HCLK divided by 8
            #define HCLK_BY_16   0b111U   // HCLK divided by 16
        //------------------------------------------------------------

        #define HPRE 4U // HCLK prescalar
        //------------------------------------------------------------
            #define SYS_CLK_NOT_DIV 0U << 3U // SYSCLK not divided    
            #define SYS_CLK_BY_2    0b1000U  // SYSCLK divided by 2   
            #define SYS_CLK_BY_4    0b1001U  // SYSCLK divided by 4   
            #define SYS_CLK_BY_8    0b1010U  // SYSCLK divided by 8     
            #define SYS_CLK_BY_16   0b1011U  // SYSCLK divided by 16    
            #define SYS_CLK_BY_64   0b1100U  // SYSCLK divided by 64   
            #define SYS_CLK_BY_128  0b1101U  // SYSCLK divided by 128 
            #define SYS_CLK_BY_256  0b1110U  // SYSCLK divided by 256 
            #define SYS_CLK_BY_512  0b1111U  // SYSCLK divided by 512 
        //------------------------------------------------------------

        #define SWS 2U // System clock switch status
        #define SW  0U // System clock switch
        //--------------------------------------------------------------------------------------
            #define HSI_SYS_CLK   0b00U // HSI selected/used as system clock
            #define HSE_SYS_CLK   0b01U // HSE selected/used as system clock
            #define PLL_SYS_CLK   0b10U // PLL selected/used as system clock
            #define HSI48_SYS_CLK 0b11U // HSI48 selected/used as system clock (when available)
        //--------------------------------------------------------------------------------------

    // RCC_AHBNER bits:
        #define IOPA_EN 17U
        #define IOPB_EN 18U
        #define IOPC_EN 19U
        #define IOPD_EN 20U
        #define IOPE_EN 21U
        #define IOPF_EN 22U
    
    
    // RCC_CFGR2 bits:
        #define PREDIV 0U // PREDIV division factor
        //-------------------------------------------------------------------------
            #define PREDIV_NOT_DIV     0b0000U // PREDIV input clock not divided
            #define PREDIV_BY_2        0b0001U // PREDIV input clock divided by 2
            #define PREDIV_BY_3        0b0010U // PREDIV input clock divided by 3
            #define PREDIV_BY_4        0b0011U // PREDIV input clock divided by 4
            #define PREDIV_BY_5        0b0100U // PREDIV input clock divided by 5
            #define PREDIV_BY_6        0b0101U // PREDIV input clock divided by 6
            #define PREDIV_BY_7        0b0110U // PREDIV input clock divided by 7
            #define PREDIV_BY_8        0b0111U // PREDIV input clock divided by 8
            #define PREDIV_BY_9        0b1000U // PREDIV input clock divided by 9
            #define PREDIV_BY_10       0b1001U // PREDIV input clock divided by 10
            #define PREDIV_BY_11       0b1010U // PREDIV input clock divided by 11
            #define PREDIV_BY_12       0b1011U // PREDIV input clock divided by 12
            #define PREDIV_BY_13       0b1100U // PREDIV input clock divided by 13
            #define PREDIV_BY_14       0b1101U // PREDIV input clock divided by 14
            #define PREDIV_BY_15       0b1110U // PREDIV input clock divided by 15
            #define PREDIV_BY_16       0b1111U // PREDIV input clock divided by 16
        //-------------------------------------------------------------------------

//----------------
// GPIO Registers
//----------------
    #define GPIOA_MODER  (volatile uint32_t*)(uintptr_t)0x48000000U // GPIO port mode register
    #define GPIOA_OTYPER (volatile uint32_t*)(uintptr_t)0x48000004U // GPIO port output type register
    #define GPIOA_PUPDR  (volatile uint32_t*)(uintptr_t)0x4800000CU // GPIO port pull-up/pull-down register
    #define GPIOA_IDR    (volatile uint32_t*)(uintptr_t)0x48000010U // GPIO port input  data register
    #define GPIOA_ODR    (volatile uint32_t*)(uintptr_t)0x48000014U // GPIO port output data register

    #define GPIOC_MODER  (volatile uint32_t*)(uintptr_t)0x48000800U // GPIO port mode register
    #define GPIOC_OTYPER (volatile uint32_t*)(uintptr_t)0x48000804U // GPIO port output type register
    #define GPIOС_PUPDR  (volatile uint32_t*)(uintptr_t)0x4800080CU // GPIO port pull-up/pull-down register
    #define GPIOC_IDR    (volatile uint32_t*)(uintptr_t)0x48000810U // GPIO port input  data register
    #define GPIOС_ODR    (volatile uint32_t*)(uintptr_t)0x48000814U // GPIO port output data register

    // GPIOC_ODR bits:
        #define LED_3_GREEN 9U
        #define LED_4_BLUE  8U 

    // GPIO Initialization:
        // Modes:
            #define INPUT_M        0b00U
            #define GEN_PURP_OUT_M 0b01U
            #define ALT_FUNC_M     0b10U
            #define ANALOG_M       0b11U
        // Output types:
            #define PUSH_PULL_T    0b0U
            #define OPEN_DRAIN_T   0b1U

    // GPIO pull-up/pull-down:
        #define NO_PULL_UP_NO_PULL_DOWN 0b00U
        #define PULL_UP                 0b01U
        #define PULL_DOWN               0b10U
        #define RESERVED                0b11U 
     


