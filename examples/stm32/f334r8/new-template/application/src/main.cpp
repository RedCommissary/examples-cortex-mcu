/********************************************************************************
 * project     Example for multi-phase PWM on the STM32F334R8 with HRPWM        *
 *                                                                              *
 * file        main.h                                                           *
 * author      @RedCommissary                                                   *
 * date        02.01.2021                                                       *
 *                                                                              *
 ********************************************************************************/

/********************************************************************************
 * Include 
 ********************************************************************************/

#include "main.h"

/********************************************************************************
 * Main program body
 ********************************************************************************/

int main (void) {

/***********************************************
 * Setting system clock for 72 MHz
 ***********************************************/
    Clock::Init();

/***********************************************
 * Setting GPIO for LED indicator on board
 * 
 * PA12 - Set high level (log.1)
 ***********************************************/
    Gpio::Init<12>(GPIOA, Gpio::Mode::output);
    Gpio::Set<12>(GPIOA);

    while(1) {}
}