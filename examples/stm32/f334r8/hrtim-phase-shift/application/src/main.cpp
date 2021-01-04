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

/***********************************************
 * Setting HRTIM
 ***********************************************/
void InitHrpwm() {

    /*
    * Setting I/O for output high resolution PWM:
    * 
    * PA8  - HRPWM channel A output 1
    * PA9  - HRPWM channel A output 2
    * PA10 - HRPWM channel B output 1
    * PA11 - HRPWM channel B output 2
    * PC8  - HRPWM channel E output 1
    * PC9  - HRPWM channel E output 2
    * 
    */
    Gpio::Init<8,9>(GPIOA, Gpio::Mode::outputAF, Gpio::Type::PP, Gpio::Speed::veryHigh, Gpio::Pupd::pullDown, Gpio::AF::af13);      
    Gpio::Init<10,11>(GPIOA, Gpio::Mode::outputAF, Gpio::Type::PP, Gpio::Speed::veryHigh, Gpio::Pupd::pullDown, Gpio::AF::af13);
    Gpio::Init<8,9>(GPIOC, Gpio::Mode::outputAF, Gpio::Type::PP, Gpio::Speed::veryHigh, Gpio::Pupd::pullDown, Gpio::AF::af3);
 
    RCC->CFGR3   |= RCC_CFGR3_HRTIM1SW_PLL;     // Enable multiplier x2 for PLL frequency                                                
    RCC->APB2ENR |= RCC_APB2ENR_HRTIM1EN;       // Enable clock for HRTIM1

    HRTIM1->sCommonRegs.DLLCR |= HRTIM_DLLCR_CAL | HRTIM_DLLCR_CALEN;   // Start timer's calibration
    while ((HRTIM1->sCommonRegs.ISR & HRTIM_ISR_DLLRDY) == RESET);      // Waiting for the end fo calibration

    /*
    * Setting to period (frequency) and starting duty
    */ 
    HRTIM1->sTimerxRegs[0].PERxR = 45000;   // Set period for timer A
    HRTIM1->sTimerxRegs[0].CMP1xR = 10000;  // Set starting duty A
    HRTIM1->sTimerxRegs[1].PERxR = 45000;   // Set period for timer B
    HRTIM1->sTimerxRegs[1].CMP1xR = 10000;  // Set starting duty B
    HRTIM1->sTimerxRegs[4].PERxR = 45000;   // Set period for timer E
    HRTIM1->sTimerxRegs[4].CMP1xR = 10000;  // Set starting duty E

    /*
    * Setting to dead time for complementary output
    */ 
    HRTIM1->sTimerxRegs[0].OUTxR |= HRTIM_OUTR_DTEN;                                        // Enable dead-time
    HRTIM1->sTimerxRegs[0].DTxR  |= (3 << HRTIM_DTR_DTPRSC_Pos);                            // Set Tdtg = (2^3) * 868 ps = 6.94 ns
    HRTIM1->sTimerxRegs[0].DTxR  |= (15 << HRTIM_DTR_DTR_Pos) | (15 << HRTIM_DTR_DTF_Pos);  // Set dead-time rising and falling = 15 * Ttg = 104 ns
    HRTIM1->sTimerxRegs[0].DTxR  |= HRTIM_DTR_DTFSLK | HRTIM_DTR_DTRSLK;                    // Lock value dead-time
        
    HRTIM1->sTimerxRegs[1].OUTxR |= HRTIM_OUTR_DTEN;                                        
    HRTIM1->sTimerxRegs[1].DTxR  |= (3 << HRTIM_DTR_DTPRSC_Pos);                            
    HRTIM1->sTimerxRegs[1].DTxR  |= (15 << HRTIM_DTR_DTR_Pos) | (15 << HRTIM_DTR_DTF_Pos);  
    HRTIM1->sTimerxRegs[1].DTxR  |= HRTIM_DTR_DTFSLK | HRTIM_DTR_DTRSLK;                    

    HRTIM1->sTimerxRegs[4].OUTxR |= HRTIM_OUTR_DTEN;                                        
    HRTIM1->sTimerxRegs[4].DTxR  |= (3 << HRTIM_DTR_DTPRSC_Pos);                            
    HRTIM1->sTimerxRegs[4].DTxR  |= (15 << HRTIM_DTR_DTR_Pos) | (15 << HRTIM_DTR_DTF_Pos);  
    HRTIM1->sTimerxRegs[4].DTxR  |= HRTIM_DTR_DTFSLK | HRTIM_DTR_DTRSLK; 

    /*
    * Start and stop event/comparator
    */ 
    HRTIM1->sTimerxRegs[0].RSTxR  |= HRTIM_RSTR_MSTPER;     // Event reload timer for channel A
    HRTIM1->sTimerxRegs[0].SETx1R |= HRTIM_SET1R_MSTPER;    // Event forces the output to active state for channel A
    HRTIM1->sTimerxRegs[0].RSTx1R |= HRTIM_RST1R_CMP1;      // Event forces the output to inactive state for channel A

    HRTIM1->sTimerxRegs[1].RSTxR  |= HRTIM_RSTR_MSTCMP1;    // Event reload timer for channel B
    HRTIM1->sTimerxRegs[1].SETx1R |= HRTIM_SET1R_MSTCMP1;   // Event forces the output to active state for channel B
    HRTIM1->sTimerxRegs[1].RSTx1R |= HRTIM_RST1R_CMP1;      // Event forces the output to inactive state for channel B

    HRTIM1->sTimerxRegs[4].RSTxR  |= HRTIM_RSTR_MSTCMP2;    // Event reload timer for channel E
    HRTIM1->sTimerxRegs[4].SETx1R |= HRTIM_SET1R_MSTCMP2;   // Event forces the output to active state for channel E
    HRTIM1->sTimerxRegs[4].RSTx1R |= HRTIM_RST1R_CMP1;      // Event forces the output to inactive state for channel E

    /*
    * Select to Continuous mode + update Master timer
    */
    HRTIM1->sTimerxRegs[0].TIMxCR |= HRTIM_TIMCR_CONT | HRTIM_TIMCR_MSTU;
    HRTIM1->sTimerxRegs[1].TIMxCR |= HRTIM_TIMCR_CONT | HRTIM_TIMCR_MSTU;
    HRTIM1->sTimerxRegs[4].TIMxCR |= HRTIM_TIMCR_CONT | HRTIM_TIMCR_MSTU;

    /*
    * Enable output HRPWM channel
    */
    HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TA1OEN | HRTIM_OENR_TA2OEN;  // Enable output PWM channel A
    HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TB1OEN | HRTIM_OENR_TB2OEN;  // Enable output PWM channel B
    HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN;  // Enable output PWM channel E

    /*
    * Setting Master timer for period (frequency) and comparator for phase shift
    */
    HRTIM1->sMasterRegs.MPER = 45000;           // Period for master timer
    HRTIM1->sMasterRegs.MCMP1R = 15000;         // Phase shift 1/3 period (120 deg) for channel B
    HRTIM1->sMasterRegs.MCMP2R = 30000;         // Phase shift 2/3 period (240 deg) for channel E

    HRTIM1->sMasterRegs.MCR |= HRTIM_MCR_CONT | HRTIM_MCR_PREEN | HRTIM_MCR_MREPU;
    
    /*
    * Start Master timer and PWM signal to channel A,B,E
    */
    HRTIM1->sMasterRegs.MCR |= HRTIM_MCR_MCEN | HRTIM_MCR_TACEN | HRTIM_MCR_TBCEN | HRTIM_MCR_TECEN; 
}

/********************************************************************************
 * Main program body
 ********************************************************************************/

int main (void) {

/***********************************************
 * Setting system clock
 ***********************************************/
    Clock::Init();

/***********************************************
 * Setting HRTIM
 ***********************************************/
    InitHrpwm();

/***********************************************
 * Setting GPIO for LED indicator on board
 * 
 * PA12 - LED_INFO
 ***********************************************/
    Gpio::Init<12>(GPIOA, Gpio::Mode::output);
    Gpio::Set<12>(GPIOA);

    while(1) {}
}