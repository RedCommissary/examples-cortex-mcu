/********************************************************************************
 * Include 
 ********************************************************************************/

#include "Adc.h"

/********************************************************************************
 * Variables 
 ********************************************************************************/

uint16_t Adc::Result::inputSignalPA0;

/********************************************************************************
 * Methods Adc
 ********************************************************************************/
void Adc::Callibration() {
    ADC1->CR &= ~ADC_CR_ADVREGEN;
    ADC1->CR |= ADC_CR_ADVREGEN_0;      // Enable Vref
    ADC1->CR &= ~ADC_CR_ADCALDIF;

    ADC1->CR |= ADC_CR_ADCAL;           // Start calibration
    while (ADC1->CR & ADC_CR_ADCAL);    // Wait end calibration
}

void Adc::Init() {
    RCC->AHBENR |= RCC_AHBENR_ADC12EN;

    Callibration();

    ADC1->JSQR |= 0x165;                    // 0 00000 0 00000 0 00000 0 00001 01 1001 01 = 0x165
                                            // Lenght = 1, Trigger = event 9, Type trigger = rising edge, Channel = IN1

    ADC1->IER |= ADC_IER_JEOSIE;            // Interrupt enable
    NVIC_EnableIRQ(ADC1_2_IRQn);            // Enable interrupt ADC1 and ADC2

    ADC1->CR |= ADC_CR_ADEN;                // Enable ADC1
    while(!(ADC1->ISR & ADC_ISR_ADRDY));    // Wait ready ADC1

    ADC1->CR |= ADC_CR_JADSTART;            // Enable injector conversion
}

void sAdc::handler() {
    ADC1->ISR |= ADC_ISR_JEOS;
    Adc::Result::inputSignalPA0 = ADC1->JDR1;
}