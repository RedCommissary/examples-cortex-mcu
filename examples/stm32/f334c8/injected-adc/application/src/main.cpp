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

uint16_t resultAdc = 0;
float inputVoltage = 0.0f;

constexpr float adcStep = 0.000807;
constexpr float dividerInputVoltage = 20.6078f;

/********************************************************************************
 * Used function
 ********************************************************************************/
void InitAdc() {
    RCC->AHBENR |= RCC_AHBENR_ADC12EN;
    
    /* Init GPIO for input */
    Gpio::Init<0>(GPIOA, Gpio::Mode::input);

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
    /* 0 00000 0 00000 0 00000 0 00001 10 1110 00 = 0x1B8 */
    ADC1->JSQR |= 0x1B8;                    // Lenght = 1, Trigger = event 14, Type trigger = rising edge, Channel = IN1

    ADC1->IER |= ADC_IER_JEOSIE;            // Interrupt enable
    NVIC_EnableIRQ(ADC1_2_IRQn);            // Enable interrupt ADC1 and ADC2

    ADC1->CR |= ADC_CR_ADEN;                // Enable ADC1
    while(!(ADC1->ISR & ADC_ISR_ADRDY));    // Wait ready ADC1

    ADC1->CR |= ADC_CR_JADSTART;            // Enable injector conversion
}

/********************************************************************************
 * Main program body
 ********************************************************************************/

int main (void) {

    Clock::Init();
    InitAdc();
    
    Gpio::Init<15>(GPIOA, Gpio::Mode::output);
    Gpio::Set<15>(GPIOA);

    while(1) {}
}

/********************************************************************************
 * ADC handler
 ********************************************************************************/

void sAdc::handler() {
    ADC1->ISR |= ADC_ISR_JEOS;
    resultAdc = ADC1->JDR1;
    inputVoltage = dividerInputVoltage * adcStep * (float)resultAdc;
}