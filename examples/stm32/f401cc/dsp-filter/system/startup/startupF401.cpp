/********************************************************************************
 * Include
 ********************************************************************************/

#include "startupF401.h"

/********************************************************************************
 * Default reset
 ********************************************************************************/

static INLINE void InitializeData() {

    uint32_t *pSrc;
    uint32_t *pDest;

    // Load initialized data from FLASH to RAM
    pSrc = &__etext;
    pDest = &__data_start__;

    while (pDest < &__data_end__)
    *pDest++ = *pSrc++;

    // Clear uninitialized data
    pDest = &__bss_start__;

    while (pDest < &__bss_end__)
    *pDest++ = 0;

    #ifdef _FPU_ENABLE
        __asm(
            "  LDR.W R0, =0xE000ED88\n"
            "  LDR R1, [R0]\n"
            "  ORR R1, R1, #(0xF << 20)\n"
            "  STR R1, [R0]"
        );
    #endif
}

extern "C" void ResetHandler() {
    InitializeData();
    main();
}

/********************************************************************************
 * Vector table
 ********************************************************************************/

__attribute__ ((used,section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {

/************* Core exceptions ******************/

    pvf(&__StackTop),
    ResetHandler,
    sNMI::handler,
    sHardFault::handler,
    sMemManage::handler,
    sBusFault::handler,
    sUsageFault::handler,
    0,0,0,0,
    sSVC::handler,
    sDebugMon::handler,
    0,
    sPendSV::handler,
    sSysTick::handler,

/********** External exceptions ***************/

    sWWDG::handler,
    sPVD::handler,
    sTampStamp::handler,
    sRtc::weakeUp,
    sFlash::handler,
    sRcc::handler,
    sExti::line0Handler,
    sExti::line1Handler,
    sExti::line2Handler,
    sExti::line3Handler,
    sExti::line4Handler,
    sDma1::channel0Handler,
    sDma1::channel1Handler,
    sDma1::channel2Handler,
    sDma1::channel3Handler,
    sDma1::channel4Handler,
    sDma1::channel5Handler,
    sDma1::channel6Handler,
    sAdc::handler,
    0,0,0,0,
    sExti::line5Handler,
    sTim1::breakeTim9Handler,
    sTim1::upTim10Handler,
    sTim1::triggerTim11Handler,
    sTim1::ccHandler,
    sTim2::handler,
    sTim3::handler,
    sTim4::handler,
    sI2C1::eventHandler,
    sI2C1::errorHandler,
    sI2C2::eventHandler,
    sI2C2::errorHandler,
    sSpi1::handler,
    sSpi2::handler,
    sUsart1::handler,
    sUsart2::handler,
    0,
    sExti::line10Handler,
    sRtc::alarmHandler,
    sUSB::wkupOtgHandler,
    0,0,0,0,
    sDma1::channel7Handler,
    0,
    sSdio::handler,
    sTim5::handler,
    sSpi3::handler,
    0,0,0,0,
    sDma2::channel0Handler,
    sDma2::channel1Handler,
    sDma2::channel2Handler,
    sDma2::channel3Handler,
    sDma2::channel4Handler,
    0,0,0,0,0,0,
    sUSB::OtgHandler,
    sDma2::channel5Handler,
    sDma2::channel6Handler,
    sDma2::channel7Handler,
    sUsart6::handler,
    sI2C3::eventHandler,
    sI2C3::errorHandler,
    0,0,0,0,0,0,0,
    sFpu::handler,
    0,0,
    sSpi4::handler
};

/********************************************************************************
 * Default interrupt
 ********************************************************************************/

WEAK void sNMI::handler()               { while (1) {} }
WEAK void sHardFault::handler()         { while (1) {} }
WEAK void sMemManage::handler()         { while (1) {} }
WEAK void sBusFault::handler()          { while (1) {} }
WEAK void sUsageFault::handler()        { while (1) {} }
WEAK void sSVC::handler()               { while (1) {} }
WEAK void sDebugMon::handler()          { while (1) {} }
WEAK void sPendSV::handler()            { while (1) {} }
WEAK void sSysTick::handler()           { while (1) {} }

WEAK void sWWDG::handler()              { while (1) {} }
WEAK void sPVD::handler()               { while (1) {} }
WEAK void sTampStamp::handler()         { while (1) {} }
WEAK void sRtc::weakeUp()               { while (1) {} }
WEAK void sFlash::handler()             { while (1) {} }
WEAK void sRcc::handler()               { while (1) {} }
WEAK void sExti::line0Handler()         { while (1) {} }
WEAK void sExti::line1Handler()         { while (1) {} }
WEAK void sExti::line2Handler()         { while (1) {} }
WEAK void sExti::line3Handler()         { while (1) {} }
WEAK void sExti::line4Handler()         { while (1) {} }
WEAK void sDma1::channel0Handler()      { while (1) {} }
WEAK void sDma1::channel1Handler()      { while (1) {} }
WEAK void sDma1::channel2Handler()      { while (1) {} }
WEAK void sDma1::channel3Handler()      { while (1) {} }
WEAK void sDma1::channel4Handler()      { while (1) {} }
WEAK void sDma1::channel5Handler()      { while (1) {} }
WEAK void sDma1::channel6Handler()      { while (1) {} }
WEAK void sAdc::handler()               { while (1) {} }
WEAK void sExti::line5Handler()         { while (1) {} }
WEAK void sTim1::breakeTim9Handler()    { while (1) {} }
WEAK void sTim1::upTim10Handler()       { while (1) {} }
WEAK void sTim1::triggerTim11Handler()  { while (1) {} }
WEAK void sTim1::ccHandler()            { while (1) {} }
WEAK void sTim2::handler()              { while (1) {} }
WEAK void sTim3::handler()              { while (1) {} }
WEAK void sTim4::handler()              { while (1) {} }
WEAK void sI2C1::eventHandler()         { while (1) {} }
WEAK void sI2C1::errorHandler()         { while (1) {} }
WEAK void sI2C2::eventHandler()         { while (1) {} }
WEAK void sI2C2::errorHandler()         { while (1) {} }
WEAK void sSpi1::handler()              { while (1) {} }
WEAK void sSpi2::handler()              { while (1) {} }
WEAK void sUsart1::handler()            { while (1) {} }
WEAK void sUsart2::handler()            { while (1) {} }
WEAK void sExti::line10Handler()        { while (1) {} }
WEAK void sRtc::alarmHandler()          { while (1) {} }
WEAK void sUSB::wkupOtgHandler()        { while (1) {} }
WEAK void sDma1::channel7Handler()      { while (1) {} }
WEAK void sSdio::handler()              { while (1) {} }
WEAK void sTim5::handler()              { while (1) {} }
WEAK void sSpi3::handler()              { while (1) {} }
WEAK void sDma2::channel0Handler()      { while (1) {} }
WEAK void sDma2::channel1Handler()      { while (1) {} }
WEAK void sDma2::channel2Handler()      { while (1) {} }
WEAK void sDma2::channel3Handler()      { while (1) {} }
WEAK void sDma2::channel4Handler()      { while (1) {} }
WEAK void sUSB::OtgHandler()            { while (1) {} }
WEAK void sDma2::channel5Handler()      { while (1) {} }
WEAK void sDma2::channel6Handler()      { while (1) {} }
WEAK void sDma2::channel7Handler()      { while (1) {} }
WEAK void sUsart6::handler()            { while (1) {} }
WEAK void sI2C3::eventHandler()         { while (1) {} }
WEAK void sI2C3::errorHandler()         { while (1) {} }
WEAK void sFpu::handler()               { while (1) {} }
WEAK void sSpi4::handler()              { while (1) {} }