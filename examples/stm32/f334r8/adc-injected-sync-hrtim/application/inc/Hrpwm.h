/********************************************************************************
 * project     Class for initialization HRPWM                                   *
 *                                                                              *
 * file        Hrpwm.h                                                          *
 * author      @RedCommissary                                                   *
 * date        25.01.2020                                                       *
 *                                                                              *
 ********************************************************************************/

#pragma once

/********************************************************************************
 * Include 
 ********************************************************************************/

#include "stm32f3xx.h"
#include "Gpio.h"

/***********************************************
* Class HRPWM
* 
* PA8  - HRPWM channel A output 1
* PA9  - HRPWM channel A output 2
* 
***********************************************/
class Hrpwm {
    public:
        static void Init();
        static void SetDuty(uint16_t duty);
};

    