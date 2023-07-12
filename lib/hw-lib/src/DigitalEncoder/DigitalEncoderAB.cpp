#include <hw/DigitalEncoder/DigitalEncoderAB.h>

DigitalEncoderAB::DigitalEncoderAB(float cpr, float gearRatio) {
    this->cpr = cpr;
    setGearRatio(gearRatio);
#ifdef IC2D_SETUP
    setAngleFormat(AngleFormat::LINEAR); 
#elif
    setAngleFormat(AngleFormat::CONTINOUS);
#endif

#ifdef TARGET_STM32F4
    continuous_counter = 0;
    prec_counter_value = 0;
#endif

}

void DigitalEncoderAB::reset(int resetOffset) {
#ifdef TARGET_STM32F4
    if (this->PositionTIM != NULL) this->PositionTIM->CNT = resetOffset;

    continuous_counter = 0;
    prec_counter_value = 0;
#endif
}

int DigitalEncoderAB::getAngle() {
#ifdef TARGET_STM32F4
    int act_counter_value = 0;

    if (this->PositionTIM != NULL) 
        act_counter_value = this->PositionTIM->CNT;
    else 
        return 0;

    if(act_counter_value >= prec_counter_value) {
        if(act_counter_value-prec_counter_value > offset) {
            continuous_counter--;
        }
    } else {
        if(prec_counter_value-act_counter_value > offset) {
            continuous_counter++;
        }
    }
    prec_counter_value = act_counter_value;
    return act_counter_value + continuous_counter*0xffff;
#endif
#ifdef TARGET_STM32L4
    float cnt = LPTIM1->CNT;
    float var = getVariation();
    if ((LPTIM1->ISR & 0x00000002) != 0) {
        LPTIM1->ICR |= 0x00000002;      /* Clear the bit */
        if(var > 0.0f)
            angle += 65536;
        else
            angle -= 65536;
    }
    return angle + cnt;
#endif
}

int DigitalEncoderAB::getMaxAngle() {
    return cpr * getGearRatio();
}

#ifdef TARGET_STM32F4
void DigitalEncoderAB::setTIM1() {
    this->PositionTIM = TIM1;
    EncoderInitialise_TIM1();
}

void DigitalEncoderAB::setTIM2() {
    this->PositionTIM = TIM2;
    EncoderInitialise_TIM2();
}

void DigitalEncoderAB::setTIM3() {
    this->VelocityTIM = TIM3;
    InterPeriodInitialise_TIM3();
    this->period = MAX_VELOCITY_PERIOD;
}

void DigitalEncoderAB::setTIM4() {
    this->VelocityTIM = TIM4;
    // EncoderInitialise_TIM4();
    InterPeriodInitialise_TIM4();
    this->period = MAX_VELOCITY_PERIOD;
}

void DigitalEncoderAB::setTIM8() {
    this->PositionTIM = TIM8;
    EncoderInitialise_TIM8();
}
#endif

void DigitalEncoderAB::setCountPerRevolution(float cpr) {
    this->cpr = cpr;
}

float DigitalEncoderAB::getCountPerRevolution() {
    return cpr;
}

/*
 *  Initialize TIM1 
 *  int bitnum;
 *  if(pin >= 8){
 *      bitnum = (pin - 8) * 4;
 *      GIPIOx -> AFR[1] &= ~(0xF<<bitnum); //CLEAR AF REGISTER 
 *      GIPIOx -> AFR[1] |= (AF<<bitnum);   // SETTING THE REGISTER
 *  } 
 *  
 * Nel registro AFR scelgo di utilizzare la parte LOW AFR[0] O HIGH AFR[1] a seconda del pin che scelgo
 * Da 0 - 7 -> parte bassa  
 * Da 8 - 15 -> parte alta
 * Dentro il registro scrivo il modulo AFR (da 0 a 7) che voglio usare (lo trovo sul manuale, anche il rispettivo TIM supportato
 * dai PIN scelti)
 */
void DigitalEncoderAB::EncoderInitialise_TIM1() // 32 bit
{
    #ifdef TARGET_STM32F4
    // configure GPIO PA8 & PA9 aka D7 & D8 as inputs for Encoder
    // Enable clock for GPIOA
    __GPIOA_CLK_ENABLE(); //equivalent from _hal_rcc.h

    //stm32f4xx.h
    GPIOA->MODER   |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1 ;           //PA8 & PA9 as Alternate Function   /*!< GPIO port mode register,               Address offset: 0x00      */
    GPIOA->OTYPER  |= GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9 ;                 //PA8 & PA9 as Inputs               /*!< GPIO port output type register,        Address offset: 0x04      */
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9 ;     // Low speed                        /*!< GPIO port output speed register,       Address offset: 0x08      */
    GPIOA->PUPDR   |= GPIO_PUPDR_PUPDR8_1 | GPIO_PUPDR_PUPDR9_1 ;           // Pull Down                        /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    GPIOA->AFR[0]  |= 0x00000000 ;                                          //  AF01 for PA8 & PA9              /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
    GPIOA->AFR[1]  |= 0x00000011 ;                                          //  bits here refer to gpio8..15    /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */

// configure TIM1 as Encoder input
    __TIM1_CLK_ENABLE();

    TIM1->CR2 = 0x0030; //MMS = '011' Master Mode - Compare Pulse
    TIM1->SMCR  = 0x0003;     // SMS='011' (Encoder mode 3)  < TIM slave mode control register
    // TIM1->CCMR1 = 0xF1F1;     // CC1S='01' CC2S='01'         < TIM capture/compare mode register 1
    TIM1->CCMR1 = 0x0001;   //CC1S = '01' < Only T1 for capture without filter
    TIM1->CCMR2 = 0x0000;     //                             < TIM capture/compare mode register 2
    // TIM1->CCER  = 0x0011;     // CC1P CC2P                   < TIM capture/compare enable register
    TIM1->CCER  = 0x0001;   // Only capture 1
    TIM1->PSC   = 0x0000;     // Prescaler = (0+1)           < TIM prescaler
    TIM1->ARR   = 0xffff;     // reload at 0xfffffff         < TIM auto-reload register

    TIM1->CNT = 0x0000;  //reset the counter before we use it

    TIM1->CR1   = 0x0001;     // CEN(Counter ENable)='1'     < TIM control register 1
#endif // TARGET_STM32F4
#ifdef TARGET_STM32L4
    GPIO_InitTypeDef GPIO_InitStruct;
    __GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    VelocityTIM = TIM1;
    __TIM1_CLK_ENABLE();
    VelocityTIM->CR2 = 0x0080;//TI1S = '1' TIM1_CH1, CH2 and CH3 pins connected to TI1 input
    VelocityTIM->SMCR = 0x0044;//SMS = '100' Slave Mode = Reset | TS = '100' TI1 Edge Detector (TI1F_ED)
    VelocityTIM->CNT = 0;
    VelocityTIM->SR = 0;
    VelocityTIM->ARR = 0xFFFF;
    //Configure period = Prescaler/freq
    VelocityTIM->PSC = VELOCITY_PRESCALER-1;

    VelocityTIM->CCMR1 = 0x0001;//CC1S = '01' TI1
    VelocityTIM->CCER = 0x0007;//CC1P = '11' Both edge | Enable Capture
    VelocityTIM->CR1 = 0x0005;//URS = 1 < only Overflow/Underflow active Update flag | Enable timer
#endif // TARGET_STM32L4
}

#ifdef TARGET_STM32F4
void DigitalEncoderAB::EncoderInitialise_TIM2() // 32 bit
{
    // configure GPIO PB8 & PB9 as inputs for Encoder
    // Enable clock for GPIOB
    __GPIOB_CLK_ENABLE(); //equivalent from _hal_rcc.h

    //stm32f4xx.h
    GPIOB->MODER   |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1 ;           //PB8 & PB9 as Alternate Function   /*!< GPIO port mode register,               Address offset: 0x00      */
    GPIOB->OTYPER  |= GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9 ;                 //PB8 & PB9 as Inputs               /*!< GPIO port output type register,        Address offset: 0x04      */
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9 ;     // Low speed                        /*!< GPIO port output speed register,       Address offset: 0x08      */
    GPIOB->PUPDR   |= GPIO_PUPDR_PUPDR8_1 | GPIO_PUPDR_PUPDR9_1 ;           // Pull Down                        /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    GPIOB->AFR[0]  |= 0x00000000 ;                                          //  AF01 for PB8 & PB9              /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
    GPIOB->AFR[1]  |= 0x00000011 ;                                          //  bits here refer to gpio8..15    /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */

// configure TIM2 as Encoder input
    __TIM2_CLK_ENABLE();

    TIM2->CR2 = 0x0030; //MMS = '011' Master Mode - Compare Pulse
    TIM2->SMCR  = 0x0003;     // SMS='011' (Encoder mode 3)  < TIM slave mode control register
    // TIM2->CCMR1 = 0xF1F1;     // CC1S='01' CC2S='01'         < TIM capture/compare mode register 1
    TIM2->CCMR1 = 0x0001;   //CC1S = '01' < Only T1 for capture without filter
    TIM2->CCMR2 = 0x0000;     //                             < TIM capture/compare mode register 2
    // TIM2->CCER  = 0x0011;     // CC1P CC2P                   < TIM capture/compare enable register
    TIM2->CCER  = 0x0001;   // Only capture 1
    TIM2->PSC   = 0x0000;     // Prescaler = (0+1)           < TIM prescaler
    TIM2->ARR   = 0xffff;     // reload at 0xfffffff         < TIM auto-reload register

    TIM2->CNT = 0x0000;  //reset the counter before we use it

    TIM2->CR1   = 0x0001;     // CEN(Counter ENable)='1'     < TIM control register 1
}

/**
 *  Initialize TIM3
 */
void DigitalEncoderAB::InterPeriodInitialise_TIM3()
{
    //Enable TIM3 clock
    __TIM3_CLK_ENABLE();
    TIM3->CR2 = 0x00;
    TIM3->SMCR = 0x04; //SMS = '100' Slave Mode - Reset, TS='000' Internal Trigger ITR0 (TIM1)
    TIM3->CNT = 0;
    TIM3->SR = 0;
    //Configure period = Prescaler/freq = 2/90 Mhz ~ 22.222 ns 
    TIM3->ARR = 0xFFFF;
    TIM3->PSC = VELOCITY_PRESCALER-1;  

    TIM3->CCMR1 = 0x03; //CC1S = '11' < internal trigger without filter
    TIM3->CCER = 0x1; //Capture 1
    TIM3->CR1 = 0x5; //URS = 1 < only Overflow/Underflow active Update flag | Enable timer
}

/**
 *  Initialize TIM4
 */
void DigitalEncoderAB::InterPeriodInitialise_TIM4()
{
    //Enable TIM4 clock
    __TIM4_CLK_ENABLE();
    TIM4->CR2 = 0x00;
    TIM4->SMCR = 0x34; //SMS = '100' Slave Mode - Reset, TS='011' Internal Trigger ITR3 (TIM8)
    TIM4->CNT = 0;
    TIM4->SR = 0;
    //Configure period = Prescaler/freq = 2/90 Mhz ~ 22.222 ns 
    TIM4->ARR = 0xFFFF;
    TIM4->PSC = VELOCITY_PRESCALER-1;  

    TIM4->CCMR1 = 0x03; //CC1S = '11' < internal trigger without filter
    TIM4->CCER = 0x1; //Capture 1
    TIM4->CR1 = 0x5; //URS = 1 < only Overflow/Underflow active Update flag | Enable timer
}

// void DigitalEncoderAB::EncoderInitialise_TIM4() // 32 bit
// {
//     //PB6 PB7 aka D10 MORPHO_PB7
//     // Enable clock for GPIOB
//     __GPIOB_CLK_ENABLE(); //equivalent from hal_rcc.h
 
//     //stm32f4xx.h 
//     //PB6 & PB7 as Alternate Function   
//     /*!< GPIO port mode register, Address offset: 0x00      */
//     GPIOB->MODER   |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 ;           
//     GPIOB->OTYPER  |= GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7;                 //PB6 & PB7 as Inputs               /*!< GPIO port output type register,        Address offset: 0x04      */
//     GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7 ;     //Low speed                         /*!< GPIO port output speed register,       Address offset: 0x08      */
//     GPIOB->PUPDR   |= GPIO_PUPDR_PUPDR6_1 | GPIO_PUPDR_PUPDR7_1 ;           //Pull Down                         /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
//     GPIOB->AFR[0]  |= 0x22000000 ;                                          //AF02 for PB6 & PB7                /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
//     GPIOB->AFR[1]  |= 0x00000000 ;                                          //nibbles here refer to gpio8..15   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
   
//     // configure TIM4 as Encoder input
//     // Enable clock for TIM4
//     __TIM4_CLK_ENABLE();
 
//     TIM4->CR1   = 0x0001;     // CEN(Counter ENable)='1'     < TIM control register 1
//     //TIM_ENCODERMODE_TI1 input 1 edges trigger count
//     //TIM_ENCODERMODE_TI2 input 2 edges trigger count
//     //TIM_ENCODERMODE_TI12 all edges trigger count
//     TIM4->SMCR  = TIM_ENCODERMODE_TI12;     //               < TIM slave mode control register
//     TIM4->CCMR1 = 0xF1F1;     // CC1S='01' CC2S='01'         < TIM capture/compare mode register 1
//     TIM4->CCMR2 = 0x0000;     //                             < TIM capture/compare mode register 2
//     TIM4->CCER  = TIM_CCER_CC1E | TIM_CCER_CC2E;     //     < TIM capture/compare enable register
//     TIM4->PSC   = 0x0000;     // Prescaler = (0+1)           < TIM prescaler
//     TIM4->ARR   = 0xffff; // reload at 0xfffffff (32 BIT)         < TIM auto-reload register
  
//     TIM4->CNT = offset;  //reset the counter before we use it  
// }

void DigitalEncoderAB::EncoderInitialise_TIM8() // 32 bit
{
    //PC6 PC7 
    // Enable clock for GPIOC
    __GPIOC_CLK_ENABLE(); //equivalent from hal_rcc.h

    //stm32f4xx.h 
    GPIOC->MODER   |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 ;           //PC6 & PC7 as Alternate Function   /*!< GPIO port mode register,               Address offset: 0x00      */
    GPIOC->OTYPER  |= GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7;                 //PC6 & PC7 as Inputs               /*!< GPIO port output type register,        Address offset: 0x04      */
    GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7 ;     //Low speed                         /*!< GPIO port output speed register,       Address offset: 0x08      */
    GPIOC->PUPDR   |= GPIO_PUPDR_PUPDR6_1 | GPIO_PUPDR_PUPDR7_1 ;           //Pull Down                         /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    GPIOC->AFR[0]  |= 0x33000000 ;                                          //AF03 for PC6 & PC7                /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
    GPIOC->AFR[1]  |= 0x00000000 ;                                          //nibbles here refer to gpio8..15   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */

    // configure TIM8 as Encoder input
    // Enable clock for TIM8
    __TIM8_CLK_ENABLE();
    
    TIM8->CR2 = 0x0030; //MMS = '011' Master Mode - Compare Pulse
    TIM8->SMCR  = TIM_ENCODERMODE_TI12;     //               < TIM slave mode control register
    // TIM8->CCMR1 = 0xF1F1;     // CC1S='01' CC2S='01'         < TIM capture/compare mode register 1
    TIM8->CCMR1 = 0x0001;   //CC1S = '01' < Only T1 for capture without filter
    TIM8->CCMR2 = 0x0000;     //                             < TIM capture/compare mode register 2
    // TIM8->CCER  = TIM_CCER_CC1E | TIM_CCER_CC2E;     //     < TIM capture/compare enable register
    TIM8->CCER  = 0x0001;   // Only capture 1
    TIM8->PSC   = 0x0000;     // Prescaler = (0+1)           < TIM prescaler
    TIM8->ARR   = 0xffff; // reload at 0xfffffff (32 BIT)         < TIM auto-reload register

    TIM8->CNT = 0x0000;  //reset the counter before we use it  

    TIM8->CR1   = 0x0001;     // CEN(Counter ENable)='1'     < TIM control register 1
}
#endif // TARGET_STM32F4

#ifdef TARGET_STM32L4
int DigitalEncoderAB::getVariation() {
    int temp = 0;
    int act_value = 0;

    act_value = LPTIM1->CNT;
    if ((LPTIM1->ISR & 0x00000002) == 0)
    {
        temp = act_value - last_value;
    }
    else
    {
        // LPTIM1->ICR |= 0x00000002;
        if (act_value > last_value)
        {
            temp = (act_value - last_value) - 65536;
        }
        else
        {
            temp = 65536 - (last_value - act_value);
        }
    }

    last_value = act_value;

    direction = (temp < 0);

    return temp;
}

bool DigitalEncoderAB::setLPTIM1() {
    return EncoderInitialise_LPTIM1();
}

bool DigitalEncoderAB::setVelocity() {
    EncoderInitialise_TIM1();
    return true;
}

bool DigitalEncoderAB::EncoderInitialise_LPTIM1() // 16 bit
{
    LPTIM_HandleTypeDef  lptimer;

    GPIO_InitTypeDef GPIO_InitStruct;
    __LPTIM1_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_LPTIM1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    lptimer.Instance = LPTIM1;
    
    lptimer.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    lptimer.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;

    lptimer.Init.UltraLowPowerClock.Polarity = LPTIM_CLOCKPOLARITY_RISING_FALLING;
    lptimer.Init.UltraLowPowerClock.SampleTime = LPTIM_CLOCKSAMPLETIME_DIRECTTRANSITION;

    lptimer.Init.Trigger.Source = LPTIM_TRIGSOURCE_0;
    lptimer.Init.Trigger.ActiveEdge = 0;
    lptimer.Init.Trigger.SampleTime = LPTIM_TRIGSAMPLETIME_DIRECTTRANSITION;

    lptimer.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
    lptimer.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
    lptimer.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
    lptimer.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
    lptimer.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;

    if (HAL_LPTIM_Init(&lptimer) != HAL_OK) {
        printf("Couldn't Init LPTimer\r\n");
        return false;
    }
    
    if (HAL_LPTIM_Encoder_Start(&lptimer, 0xffff) != HAL_OK) {
        printf("Couldn't Start Encoder\r\n");
        return false;
    }

    return true;
}

#endif // TARGET_STM32L4

float DigitalEncoderAB::getVelocityRad(float dt)
{
    float sign = 1.f;
    float unsigned_out = 0;

#ifdef IC2D_SETUP

    if ((this->VelocityTIM->SR & TIM_SR_UIF) != 0 
        and overflow_correction < MAX_OVERFLOW)  {
        overflow_correction += 65536.f*BASE_VELOCITY;
    }
    if ((this->VelocityTIM->SR & TIM_SR_TIF) != 0)
    {
        period = ((float)this->VelocityTIM->CCR1)*BASE_VELOCITY;
        period += overflow_correction;
        overflow_correction = 0.f;
    }
    else
    {
        if ((period < MAX_VELOCITY_PERIOD) and 
            (overflow_correction > (MAX_OVERFLOW/2))) {
            period += dt;
        }
    }
    this->VelocityTIM->SR = 0;
    unsigned_out = (4.f/(float)this->getMaxAngle())/period;
#elif

    if ((this->VelocityTIM->SR & TIM_SR_UIF) != 0 
        and overflow_correction < MAX_OVERFLOW)  {
        overflow_correction += 65536.f*BASE_VELOCITY;
    }
    if ((this->VelocityTIM->SR & TIM_SR_TIF) != 0)
    {
        period = ((float)this->VelocityTIM->CCR1)*BASE_VELOCITY;
        period += overflow_correction;
        overflow_correction = 0.f;
    }
    else
    {
        if ((period < MAX_VELOCITY_PERIOD) and 
            (overflow_correction > (MAX_OVERFLOW/2))) {
            period += dt;
        }
    }
    this->VelocityTIM->SR = 0;
    unsigned_out = (8.f*M_PI/(float)this->getMaxAngle())/period;
#endif

#ifdef TARGET_STM32F4
    if ((this->PositionTIM->CR1 & TIM_CR1_DIR) != 0) sign = -1.f;
#elif TARGET_STM32L4
    if (direction) sign = -1.f;
#endif

    return sign*unsigned_out;
}