/********************************************************************************
 * class        Settings clock system MCU                                              *
 *                                                                              *
 * file         Clock.h                                                         *
 * author       @RedCommissary                                                  *
 * date         10.02.2021                                                      *
 *                                                                              *
 ********************************************************************************/

#pragma once

/********************************************************************************
 * Include 
 ********************************************************************************/

#include "stm32f4xx.h"

/********************************************************************************
 * Class Clock
 ********************************************************************************/

class Clock {
    public:
        static void Init() {
            RCC->CR |= RCC_CR_HSION;                                    // Enable system for internal clock
            while (!(RCC->CR & RCC_CR_HSIRDY));                         // Waiting flag about enable

            FLASH->ACR |= FLASH_ACR_LATENCY_3WS;                        // Latency for internal flash memory

            RCC->CFGR |= RCC_PLLCFGR_PLLM_4;                            // HSI divider 16
            RCC->CFGR |= RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_7;       // Mult x192

            RCC->CR |= RCC_CR_PLLON;                                    // Enable PLL system
            while((RCC->CR & RCC_CR_PLLRDY) == 0){}                     // Waiting flag about enable
            RCC->CFGR |= RCC_CFGR_SW_PLL;                               // Select source SYSCLK = PLL
            while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1) {}      // Waiting flag about enable
        }
};