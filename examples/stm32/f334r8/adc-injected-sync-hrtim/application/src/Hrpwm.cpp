/********************************************************************************
 * Include 
 ********************************************************************************/

#include "Hrpwm.h"

/********************************************************************************
 * Methods Hrpwm
 ********************************************************************************/
void Hrpwm::Init() {
    /*
    * Setting I/O for PWM1A and PWM2A
    */ 
    Gpio::Init<8,9>(GPIOA, Gpio::Mode::outputAF, Gpio::Type::PP, Gpio::Speed::veryHigh, Gpio::Pupd::pullDown, Gpio::AF::af13);

    /*
    * Setting system clock for HRTIM
    */ 
    RCC->CFGR3   |= RCC_CFGR3_HRTIM1SW_PLL;     // Enable multiplier x2 for PLL frequency                                                
    RCC->APB2ENR |= RCC_APB2ENR_HRTIM1EN;       // Enable clock for HRTIM1

    HRTIM1->sCommonRegs.DLLCR |= HRTIM_DLLCR_CAL | HRTIM_DLLCR_CALEN;   // Start timer's calibration
    while ((HRTIM1->sCommonRegs.ISR & HRTIM_ISR_DLLRDY) == RESET);      // Waiting for the end fo calibration

    /*
    * Setting to period (frequency) and starting duty
    */ 
    HRTIM1->sTimerxRegs[0].PERxR = 45000;   // Set period for timer A
    HRTIM1->sTimerxRegs[0].CMP1xR = 100;    // Set starting duty A

    /*
    * Setting to dead time for complementary output
    */ 
    HRTIM1->sTimerxRegs[0].OUTxR |= HRTIM_OUTR_DTEN;                                        // Enable dead-time
    HRTIM1->sTimerxRegs[0].DTxR  |= (3 << HRTIM_DTR_DTPRSC_Pos);                            // Set Tdtg = (2^3) * 868 ps = 6.94 ns
    HRTIM1->sTimerxRegs[0].DTxR  |= (30 << HRTIM_DTR_DTR_Pos) | (30 << HRTIM_DTR_DTF_Pos);  // Set dead-time rising and falling = 30 * Ttg = 208 ns
    HRTIM1->sTimerxRegs[0].DTxR  |= HRTIM_DTR_DTFSLK | HRTIM_DTR_DTRSLK;                    // Lock value dead-time

    /*
    * Select event for start ADC1
    */
    HRTIM1->sTimerxRegs[0].CMP2xR = 4500;               // Samples in 10% of ON time 
    HRTIM1->sCommonRegs.CR1 |= HRTIM_CR1_ADC2USRC_0;    // ADC trigger 2 update: Timer A 
    HRTIM1->sCommonRegs.ADC2R |= HRTIM_ADC2R_AD2TAC2;   // ADC trigger 2 event: Timer A compare 2  

    /*
    * Start and stop event/comparator
    */ 
    HRTIM1->sTimerxRegs[0].SETx1R |= HRTIM_SET1R_PER;   // Event forces the output to active state
    HRTIM1->sTimerxRegs[0].RSTx1R |= HRTIM_RST1R_CMP1;  // Event forces the output to inactive state

    /*
    * Select to Continuous mode + update Master timer
    */
    HRTIM1->sTimerxRegs[0].TIMxCR |= HRTIM_TIMCR_CONT;

    /*
    * Enable output HRPWM channel
    */
    HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TA1OEN | HRTIM_OENR_TA2OEN;  // Enable output PWM channel A
    
    /*
    * Setting Master timer for period (frequency) and comparator for phase shift
    */
    HRTIM1->sMasterRegs.MPER = 45000;           // Period for master timer
    
    /*
    * Start Master timer and PWM signal to channel A,B,E
    */
    HRTIM1->sMasterRegs.MCR |= HRTIM_MCR_MCEN | HRTIM_MCR_TACEN;
}

void Hrpwm::SetDuty (uint16_t duty) {
    HRTIM1->sTimerxRegs[0].CMP1xR = duty;  // Set starting duty A
}