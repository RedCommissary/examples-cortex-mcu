/********************************************************************************
 * project     Example for test injected channel ADC STM32F334                  *
 *                                                                              *
 * file        main.h                                                           *
 * author      @RedCommissary                                                   *
 * date        04.11.2020                                                       *
 * copyright   The MIT License (MIT)                                            *
 *                                                                              *
 ********************************************************************************/

/********************************************************************************
 * ADC
 * 
 * Pin voltage input    - PA0   - ADC1 IN1
 * Pin current input    - PA1   - ADC1 IN2
 * Pin current output   - PA2   - ADC1 IN3
 * Pin voltage output   - PA3   - ADC1 IN4
 * Pin temperature      - PA5   - ADC2 IN2
 * 
 ********************************************************************************/

/********************************************************************************
 * Include 
 ********************************************************************************/

#include "main.h"

/********************************************************************************
 * Variable
 ********************************************************************************/

uint16_t resultAdcInputVoltage = 0;
uint16_t resultAdcOutputVoltage = 0;
float inputVoltage = 0.0f;
float outputVoltage = 0.0f;
uint16_t dutyHrpwm = 22500;

constexpr float adcStep = 0.000807;
constexpr float dividerInputVoltage = 20.6078f;
constexpr float dividerOutputVoltage = 4.9f;

/********************************************************************************
 * Used function
 ********************************************************************************/

void InitLed() {
    Gpio::Init<15>(GPIOA, Gpio::Mode::output);
    Gpio::Set<15>(GPIOA);
}

void InitAdc() {
    RCC->AHBENR |= RCC_AHBENR_ADC12EN;
    
    /* Init GPIO for input */
    Gpio::Init<0,3>(GPIOA, Gpio::Mode::input);

    /* Calibration ADC */
    ADC1->CR &= ~ADC_CR_ADVREGEN;
    ADC1->CR |= ADC_CR_ADVREGEN_0;          // Enable Vref
    ADC1->CR &= ~ADC_CR_ADCALDIF;

    ADC1->CR |= ADC_CR_ADCAL;               // Start calibration
    while (ADC1->CR & ADC_CR_ADCAL);        // Wait end calibration

    /* Timer setting to the trigger for start ADC */
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 1-1;
    TIM6->ARR = 36;
    TIM6->CR2 |= TIM_CR2_MMS_1;             // Enable generation TRGO for ADC
    TIM6->CR1 |= TIM_CR1_CEN;

    /* ADC setting to the injected channel */
    /* 0 00000 0 00000 0 00100 0 00001 10 1110 01 = 0x1B8 */
    ADC1->JSQR |= 0x101B9;                  // Lenght = 2, Trigger = event 14, Type trigger = rising edge, Channel = IN1, IN4

    ADC1->IER |= ADC_IER_JEOSIE;            // Interrupt enable
    NVIC_EnableIRQ(ADC1_2_IRQn);            // Enable interrupt ADC1 and ADC2

    ADC1->CR |= ADC_CR_ADEN;                // Enable ADC1
    while(!(ADC1->ISR & ADC_ISR_ADRDY));    // Wait ready ADC1

    ADC1->CR |= ADC_CR_JADSTART;            // Enable injector conversion
}

void InitHrpwm() {
    RCC->CFGR3   |= RCC_CFGR3_HRTIM1SW_PLL;                                                 // Enable multiplier x2 for PLL frequency
    RCC->APB2ENR |= RCC_APB2ENR_HRTIM1EN;  

    Gpio::Init<8,9>(GPIOA, Gpio::Mode::outputAF, Gpio::Type::PP, Gpio::Speed::veryHigh, Gpio::Pupd::pullDown, Gpio::AF::af13);

    HRTIM1->sCommonRegs.DLLCR |= HRTIM_DLLCR_CAL | HRTIM_DLLCR_CALEN;                       // Start timer's calibration
    while ((HRTIM1->sCommonRegs.ISR & HRTIM_ISR_DLLRDY) == RESET);                          // Waiting for the end fo calibration

    HRTIM1->sTimerxRegs[0].PERxR = 45000;                                                   // Set period for timer
    HRTIM1->sTimerxRegs[0].CMP1xR = 0;                                                      // Set starting duty

    HRTIM1->sTimerxRegs[0].OUTxR |= HRTIM_OUTR_DTEN;                                        // Enable dead-time
    HRTIM1->sTimerxRegs[0].DTxR  |= (3 << HRTIM_DTR_DTPRSC_Pos);                            // Set Tdtg = (2^3) * 868 ps = 6.94 ns
    HRTIM1->sTimerxRegs[0].DTxR  |= (15 << HRTIM_DTR_DTR_Pos) | (15 << HRTIM_DTR_DTF_Pos);  // Set dead-time rising and falling = 15 * Ttg = 104 ns
    HRTIM1->sTimerxRegs[0].DTxR  |= HRTIM_DTR_DTFSLK | HRTIM_DTR_DTRSLK;                    // Lock value dead-time

    HRTIM1->sTimerxRegs[0].SETx1R |= HRTIM_SET1R_PER;	                                    // Event forces the output to active state
    HRTIM1->sTimerxRegs[0].RSTx1R |= HRTIM_RST1R_CMP1;                                      // Event forces the output to inactive state

    HRTIM1->sTimerxRegs[0].TIMxCR |= HRTIM_TIMCR_CONT;                                      // Continuous mode

    HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TA1OEN | HRTIM_OENR_TA2OEN;                      // Enable output PWM channel A

    HRTIM1->sMasterRegs.MPER = 45000;                                                       // Period for master timer
    HRTIM1->sMasterRegs.MCR |= HRTIM_MCR_MCEN | HRTIM_MCR_TACEN;                            // Enable counter for Master and timer A
}

/********************************************************************************
 * Main program body
 ********************************************************************************/

int main (void) {

    Clock::Init();
    InitAdc();
    InitHrpwm();
    InitLed();

    while(1) {
        HRTIM1->sTimerxRegs[0].CMP1xR = dutyHrpwm;
    }
}

/********************************************************************************
 * ADC handler
 ********************************************************************************/

void sAdc::handler() {
    ADC1->ISR |= ADC_ISR_JEOS;
    resultAdcInputVoltage = ADC1->JDR1;
    resultAdcOutputVoltage = ADC1->JDR2;
    inputVoltage = dividerInputVoltage * adcStep * (float)resultAdcInputVoltage;
    outputVoltage = dividerOutputVoltage * adcStep * (float)resultAdcOutputVoltage;
}