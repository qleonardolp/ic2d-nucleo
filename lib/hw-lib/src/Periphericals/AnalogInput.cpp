#include "hw/Periphericals/AnalogInput.h"

uint8_t AnalogInput::_using_ADC1 = 0;
uint8_t AnalogInput::_using_ADC2 = 0;
uint8_t AnalogInput::_using_ADC3 = 0;

AnalogInput::AnalogInput(PinName pin) {
    bool first_instance;

    //default setting for ADC
    #ifdef TARGET_STM32L4
        ADCPrescaler Prescaler = ADC_SPCLK1;
        ADCAlign Alignment = ADC_Right;
        ADCSample Sample = ADC_2s5;
        ADCResolution Resolution = ADC_12b;
    #endif

    #ifdef TARGET_STM32F4
        ADCPrescaler Prescaler = ADC_PCLK2;
        ADCAlign Alignment = ADC_Right;
        ADCSample Sample = ADC_3s;
        ADCResolution Resolution = ADC_12b;
    #endif

    ADC_Common_TypeDef ADC_Common;
    uint32_t function = pinmap_function(pin, PinMap_ADC);
    //find for ADC (1, 2 or 3)
    _Conversor = (ADC_TypeDef *)pinmap_peripheral(pin, PinMap_ADC);
    //configure the GPIO
    pinmap_pinout(pin, PinMap_ADC);
    //find for channel (1, 2, ... or 15)
    _Channel = STM_PIN_CHANNEL(function);
    //enable the clock of the ADC
    #ifdef TARGET_STM32L4
        #if defined(ADC1)
            if ((ADCName &)_Conversor == ADC_1) {
                __HAL_RCC_ADC_CLK_ENABLE();
                if (_using_ADC1 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC1++;
            }
        #endif
        #if defined(ADC2)
            if ((ADCName &)_Conversor == ADC_2) {
                __HAL_RCC_ADC_CLK_ENABLE();
                if (_using_ADC2 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC2++;
            }
        #endif

        if (first_instance)
        {
            if ((Prescaler == ADC_SPCLK1) || (Prescaler == ADC_SPCLK2) || (Prescaler == ADC_SPCLK4)) {
                RCC->CCIPR |= RCC_ADCCLKSOURCE_SYSCLK;
                //HAL_RCC_GetSysClockFreq();
            }
            else {
                RCC->CCIPR |= RCC_ADCCLKSOURCE_PLLSAI1;
                RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1REN;
                RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN;
                __HAL_RCC_PLLSAI1_ENABLE();
            }
        }

    #elif TARGET_STM32F4
        #if defined(ADC1)
            if ((ADCName &)_Conversor == ADC_1) {
                __HAL_RCC_ADC1_CLK_ENABLE();
                if (_using_ADC1 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC1++;
            }
        #endif
        #if defined(ADC2)
            if ((ADCName &)_Conversor == ADC_2) {
                __HAL_RCC_ADC2_CLK_ENABLE();
                if (_using_ADC2 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC2++;
            }
        #endif
        #if defined(ADC3)
            if ((ADCName &)_Conversor == ADC_3) {
                __HAL_RCC_ADC3_CLK_ENABLE();
                if (_using_ADC3 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC3++;
            }
        #endif
    #endif
    //configure the clock prescaler of the ADC
    if (first_instance) ADC_Common.CCR = Prescaler;

    //configure the ADC
    #ifdef TARGET_STM32L4
        if (first_instance) _Conversor->CFGR = Resolution | Alignment;
        _Conversor->CFGR2 = 0U;
    #elif TARGET_STM32F4
        if (first_instance)
        {
            _Conversor->CR1 = Resolution;
            _Conversor->CR2 = Alignment;
        }
    #endif
    
    _continuous_mode = false;
    _usage_dma = false;
    _usage_fast_divider = false;
    _injection_convertion = true;

    //set initializated flag
    _initialized = true;
}

AnalogInput::AnalogInput(PinName pin, ADCPrescaler Prescaler, ADCAlign Alignment, ADCSample Sample, ADCResolution Resolution) {
    bool first_instance;
    ADC_Common_TypeDef ADC_Common;
    uint32_t function = pinmap_function(pin, PinMap_ADC);
    //find for ADC (1, 2 or 3)
    _Conversor = (ADC_TypeDef *)pinmap_peripheral(pin, PinMap_ADC);
    //configure the GPIO
    pinmap_pinout(pin, PinMap_ADC);
    //find for channel (1, 2, ... or 15)
    _Channel = STM_PIN_CHANNEL(function);
    //enable the clock of the ADC
    #ifdef TARGET_STM32L4
        #if defined(ADC1)
            if ((ADCName &)_Conversor == ADC_1) {
                __HAL_RCC_ADC_CLK_ENABLE();
                if (_using_ADC1 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC1++;
            }
        #endif
        #if defined(ADC2)
            if ((ADCName &)_Conversor == ADC_2) {
                __HAL_RCC_ADC_CLK_ENABLE();
                if (_using_ADC2 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC2++;
            }
        #endif

        if (first_instance)
        {
            if ((Prescaler == ADC_SPCLK1) || (Prescaler == ADC_SPCLK2) || (Prescaler == ADC_SPCLK4)) {
                RCC->CCIPR |= RCC_ADCCLKSOURCE_SYSCLK;
                //HAL_RCC_GetSysClockFreq();
            }
            else {
                RCC->CCIPR |= RCC_ADCCLKSOURCE_PLLSAI1;
                RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1REN;
                RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN;
                __HAL_RCC_PLLSAI1_ENABLE();
            }
        }

    #elif TARGET_STM32F4
        #if defined(ADC1)
            if ((ADCName &)_Conversor == ADC_1) {
                __HAL_RCC_ADC1_CLK_ENABLE();
                if (_using_ADC1 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC1++;
            }
        #endif
        #if defined(ADC2)
            if ((ADCName &)_Conversor == ADC_2) {
                __HAL_RCC_ADC2_CLK_ENABLE();
                if (_using_ADC2 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC2++;
            }
        #endif
        #if defined(ADC3)
            if ((ADCName &)_Conversor == ADC_3) {
                __HAL_RCC_ADC3_CLK_ENABLE();
                if (_using_ADC3 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC3++;
            }
        #endif
    #endif
    //configure the clock prescaler of the ADC
    if (first_instance) ADC_Common.CCR = Prescaler;
    
    //configure the ADC
    #ifdef TARGET_STM32L4
        if (first_instance) _Conversor->CFGR = Resolution | Alignment;
        _Conversor->CFGR2 = 0U;
    #elif TARGET_STM32F4
        if (first_instance)
        {
            _Conversor->CR1 = Resolution;
            _Conversor->CR2 = Alignment;
        }
    #endif
    
    _continuous_mode = false;
    _usage_dma = false;
    _usage_fast_divider = false;
    _injection_convertion = true;

    //set initializated flag
    _initialized = true;
}

AnalogInput::AnalogInput(PinName pin, ADCPrescaler Prescaler, ADCAlign Alignment, ADCSample Sample, ADCResolution Resolution, ADCContinuous Continuous, ADCDma Dma, uint32_t Size_buffer) {
    bool first_instance;
    uint32_t i, j, k;
    ADC_Common_TypeDef ADC_Common;
    uint32_t function = pinmap_function(pin, PinMap_ADC);
    //find for ADC (1, 2 or 3)
    _Conversor = (ADC_TypeDef *)pinmap_peripheral(pin, PinMap_ADC);
    //configure the GPIO
    pinmap_pinout(pin, PinMap_ADC);
    //find for channel (1, 2, ... or 15)
    _Channel = STM_PIN_CHANNEL(function);
    //enable the clock of the ADC
    #ifdef TARGET_STM32L4
        #if defined(ADC1)
            if ((ADCName &)_Conversor == ADC_1) {
                __HAL_RCC_ADC_CLK_ENABLE();
                if (_using_ADC1 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC1++;
            }
        #endif
        #if defined(ADC2)
            if ((ADCName &)_Conversor == ADC_2) {
                __HAL_RCC_ADC_CLK_ENABLE();
                if (_using_ADC2 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC2++;
            }
        #endif

        if (first_instance)
        {
            if ((Prescaler == ADC_SPCLK1) || (Prescaler == ADC_SPCLK2) || (Prescaler == ADC_SPCLK4)) {
                RCC->CCIPR |= RCC_ADCCLKSOURCE_SYSCLK;
                //HAL_RCC_GetSysClockFreq();
            }
            else {
                RCC->CCIPR |= RCC_ADCCLKSOURCE_PLLSAI1;
                RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1REN;
                RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN;
                __HAL_RCC_PLLSAI1_ENABLE();
            }
        }

    #elif TARGET_STM32F4
        #if defined(ADC1)
            if ((ADCName &)_Conversor == ADC_1) {
                __HAL_RCC_ADC1_CLK_ENABLE();
                if (_using_ADC1 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC1++;
            }
        #endif
        #if defined(ADC2)
            if ((ADCName &)_Conversor == ADC_2) {
                __HAL_RCC_ADC2_CLK_ENABLE();
                if (_using_ADC2 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC2++;
            }
        #endif
        #if defined(ADC3)
            if ((ADCName &)_Conversor == ADC_3) {
                __HAL_RCC_ADC3_CLK_ENABLE();
                if (_using_ADC3 == 0) first_instance = true;
                else first_instance = false;
                _using_ADC3++;
            }
        #endif
    #endif
    //configure the clock prescaler of the ADC
    if (first_instance) ADC_Common.CCR = Prescaler;

    #ifdef TARGET_STM32L4
        //configure the time sample
        if (_Channel < 10)
        {
            _Conversor->SMPR1 = Sample<<(_Channel*3);
            //_Conversor->SMPR2 = 0U;
        }
        else
        {
            //_Conversor->SMPR1 = 0U;
            _Conversor->SMPR2 = Sample<<(_Channel*3);
        }
        //configure the channel for convertion
        _Conversor->SQR1 = _Channel<<6;
        _Conversor->SQR2 = 0U;
        _Conversor->SQR3 = 0U;
        _Conversor->SQR4 = 0U;
    #elif TARGET_STM32F4
        //configure the time sample
        if (_Channel < 10)
        {
            _Conversor->SMPR1 = 0U;
            _Conversor->SMPR2 = Sample<<(_Channel*3);
        }
        else
        {
            _Conversor->SMPR1 = Sample<<(_Channel*3);
            _Conversor->SMPR2 = 0U;
        }
        //configure the channel for convertion
        _Conversor->SQR1 = 0U;
        _Conversor->SQR2 = 0U;
        _Conversor->SQR3 = _Channel;
    #endif
    
    //check if using DMA
    if (Size_buffer > 65535) _Size_buffer = 65535;
    else if (Size_buffer == 0) _Size_buffer = 1;
    else _Size_buffer = Size_buffer;
    if (Dma == ADC_Dma)
    {
        _pointer = (uint16_t *)malloc(_Size_buffer*2);
        #ifdef TARGET_STM32L4
            #if defined(ADC1)
                if ((ADCName &)_Conversor == ADC_1)
                {
                    if ((DMA1_Channel1->CCR & DMA_CCR_EN) == 0)
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _Stream_DMA = DMA1_Channel1;
                        DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & 0xFFFFFFF0) | (DMA_REQUEST_0<<0);
                    }
                    else if ((DMA2_Channel3->CCR & DMA_CCR_EN) == 0)
                    {
                        if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                        _Stream_DMA = DMA2_Channel3;
                        DMA2_CSELR->CSELR = (DMA2_CSELR->CSELR & 0xFFFFF0FF) | (DMA_REQUEST_0<<8);
                    }
                }
            #endif
            #if defined(ADC2)
                if ((ADCName &)_Conversor == ADC_2)
                {
                    if ((DMA1_Channel2->CCR & DMA_CCR_EN) == 0)
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _Stream_DMA = DMA1_Channel2;
                        DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & 0xFFFFFF0F) | (DMA_REQUEST_0<<4);
                    }
                    else if ((DMA2_Channel4->CCR & DMA_CCR_EN) == 0)
                    {
                        if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                        _Stream_DMA = DMA2_Channel4;
                        DMA2_CSELR->CSELR = (DMA2_CSELR->CSELR & 0xFFFF0FFF) | (DMA_REQUEST_0<<12);
                    }
                }
            #endif
            _Stream_DMA->CCR = DMA_CCR_CIRC | DMA_CCR_MINC | DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0 | DMA_CCR_PL;
            _Stream_DMA->CNDTR = _Size_buffer;
            _Stream_DMA->CPAR = (uint32_t)&_Conversor->DR;
            _Stream_DMA->CMAR = (uint32_t)_pointer;
            _Stream_DMA->CCR |= DMA_CCR_EN;
        #elif TARGET_STM32F4
            if (__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
            #if defined(ADC1)
                if ((ADCName &)_Conversor == ADC_1)
                {
                    if ((DMA2_Stream0->CR & 0x1) == 0)
                    {
                        _Stream_DMA = DMA2_Stream0;
                        _Stream_DMA->CR = 0U;
                    }
                    else if ((DMA2_Stream4->CR & 0x1) == 0)
                    {
                        _Stream_DMA = DMA2_Stream4;
                        _Stream_DMA->CR = 0U;
                    }
                }
            #endif
            #if defined(ADC2)
                if ((ADCName &)_Conversor == ADC_2) {
                    if ((DMA2_Stream2->CR & 0x1) == 0)
                    {
                        _Stream_DMA = DMA2_Stream2;
                        _Stream_DMA->CR = 0x02000000;
                    }
                    else if ((DMA2_Stream3->CR & 0x1) == 0)
                    {
                        _Stream_DMA = DMA2_Stream3;
                        _Stream_DMA->CR = 0x02000000;
                    }
                }
            #endif
            #if defined(ADC3)
                if ((ADCName &)_Conversor == ADC_3) {
                    if ((DMA2_Stream0->CR & 0x1) == 0)
                    {
                        _Stream_DMA = DMA2_Stream0;
                        _Stream_DMA->CR = 0x04000000;
                    }
                    else if ((DMA2_Stream1->CR & 0x1) == 0)
                    {
                        _Stream_DMA = DMA2_Stream1;
                        _Stream_DMA->CR = 0x04000000;
                    }
                }
            #endif
            _Stream_DMA->CR |= 0x00032D00;
            _Stream_DMA->NDTR = _Size_buffer;
            _Stream_DMA->PAR = (uint32_t)&_Conversor->DR;
            _Stream_DMA->M0AR = (uint32_t)_pointer;
            _Stream_DMA->CR |= 1;
        #endif
        Continuous = ADC_Continuous;
    }
    //configure the ADC
    #ifdef TARGET_STM32L4
        if (first_instance) _Conversor->CFGR = Dma | Resolution | Alignment | Continuous;
        else _Conversor->CFGR |= (Dma | Continuous);
        _Conversor->CFGR2 = 0U;
    #elif TARGET_STM32F4
        if (first_instance)
        {
            _Conversor->CR1 = Resolution;
            _Conversor->CR2 = Alignment | Continuous | Dma;
        }
        else _Conversor->CR2 |= (Continuous | Dma);
    #endif
    //check configuration
    if (Continuous == ADC_Continuous) _continuous_mode = true;
    else _continuous_mode = false;
    if (Dma == ADC_Dma) _usage_dma = true;
    else _usage_dma = false;
    
    //determine the divider coeficiente
    k = _Size_buffer;
    j = 0;
    for (i = 0; i < 32; i++)
    {
        if ((k & 0x1) != 0) j++;
        k>>1;
    }
    if (j > 1)
    {
        _usage_fast_divider = false;
        _Divider = _Size_buffer;
    }
    else 
    {
        _usage_fast_divider = true;
        k = _Size_buffer;
        _Divider = 0;
        do
        {
            k = k>>1;
            _Divider++;
        } while ((k & 0x1) == 0);
    }

    _injection_convertion = false;

    //set initializated flag
    _initialized = true;
}

AnalogInput::~AnalogInput() {
    #ifdef TARGET_STM32L4
        #if defined(ADC1)
            if ((ADCName &)_Conversor == ADC_1) {
                _using_ADC1--;
            }
        #endif
        #if defined(ADC2)
            if ((ADCName &)_Conversor == ADC_2) {
                _using_ADC2--;
            }
        #endif
    #elif TARGET_STM32F4
        #if defined(ADC1)
            if ((ADCName &)_Conversor == ADC_1) {
                _using_ADC1--;
            }
        #endif
        #if defined(ADC2)
            if ((ADCName &)_Conversor == ADC_2) {
                _using_ADC2--;
            }
        #endif
        #if defined(ADC3)
            if ((ADCName &)_Conversor == ADC_3) {
                _using_ADC3--;
            }
        #endif
    #endif
}

int8_t AnalogInput::enable()
{
    u_int32_t timeout = 0;
    if (_initialized) //Check if the ADC was initialized
    {
        #ifdef TARGET_STM32L4
            if (is_enabled() == 0) //Check if the ADC is enable
            {
                _Conversor->CR &= ~ADC_CR_DEEPPWD; //Ensure the ADC out deep power
                _Conversor->CR |= ADC_CR_ADVREGEN; //Enable the ADC regulator
                wait_us(25); //Wait for regulater power on
                _Conversor->CR |= ADC_CR_ADCAL; //Start calibration sequence
                timeout = 5000000;
                while ((_Conversor->CR & ADC_CR_ADCAL) != 0U) //Wait for calibration
                {
                    timeout--;
                    if (timeout == 0) return(-2);
                }
                wait_us(1); //Ensure time after calibration
                _Conversor->ISR |= ADC_ISR_ADRDY; //Reset bit ADC enable
                _Conversor->CR |= ADC_CR_ADEN; //Start enable sequence
                timeout = 5000000;
                while ((_Conversor->ISR & ADC_ISR_ADRDY) != 0U) //Wait for enable is complited
                {
                    timeout--;
                    if (timeout == 0) return(-3);
                }
            }
            else
            {
                if (_continuous_mode && is_started() == 0)
                {
                    start();
                    return(2);
                }
                return(1); //ADC already is enable
            }
        #elif TARGET_STM32F4
            _Conversor->CR2 |= 0x00000001; //Enable the ADC
        #endif
        if (_continuous_mode) //Check if the continuous mode is active
            start(); //Start the convertion
        return(0); //Enabled with success
    }
    else return(-1); //ADC not initialized
}

int8_t AnalogInput::unable()
{
    if (_initialized) //Check if the ADC was initialized
    {
        stop(); //Stop the convertion
        #ifdef TARGET_STM32L4
            if (is_enabled() == 1) //Check if the ADC is unable
            {
                _Conversor->CR |= ADC_CR_ADDIS; //Start unable sequence
                while (is_enabled() == 1); //Wait for unable is complited
            }
            else return(1); //ADC already is unable
        #elif TARGET_STM32F4
            _Conversor->CR2 &= 0xFFFFFFFE; //Unable the ADC
        #endif
        return(0); //Unabled with success
    }
    else return(-1); //ADC not initialized

}

int8_t AnalogInput::start()
{
    if (_initialized) //Check if the ADC was initialized
    {
        #ifdef TARGET_STM32L4
            if (is_enabled() == 1) //Check if the ADC is enable
            {
                if (is_started() == 0) //Check if the conversion is in progress
                {
                    if (_injection_convertion)
                    {
                        _Conversor->CFGR |= (ADC_CFGR_JQDIS | ADC_CFGR_JDISCEN);
                        _Conversor->JSQR = _Channel<<8;
                        _Conversor->CR |= ADC_CR_JADSTART;
                    }
                    else
                    {
                        _Conversor->CR |= ADC_CR_ADSTART; //Start conversion
                    }
                }
                else
                    return(1); //Conversion already is in progress
            }
            else
                return(-2); //ADC not enabled
        #elif TARGET_STM32F4
            if (_injection_convertion)
            {
                _Conversor->CR1 &= ~ADC_CR1_JAUTO;
                _Conversor->CR1 |= ADC_CR1_JDISCEN;
                _Conversor->JSQR = _Channel<<15;
                _Conversor->CR2 |= ADC_CR2_JSWSTART;
            }
            else
            {
                _Conversor->CR2 |= ADC_CR2_SWSTART; //Start convertion
            }
        #endif
        return(0); //Conversion started with success
    }
    else return(-1); //ADC not initialized
}

int8_t AnalogInput::stop()
{
    if (_initialized) //Check if the ADC was initialized
    {
        #ifdef TARGET_STM32L4
            if (is_started() == 1) //Check if have the conversion is in progress
            {
                if (_injection_convertion)
                {
                    _Conversor->CR |= ADC_CR_JADSTP;
                    while((_Conversor->CR & ADC_CR_JADSTP) != 0U);
                }
                else
                {
                    _Conversor->CR |= ADC_CR_ADSTP; //Start stop conversion sequence
                    while((_Conversor->CR & ADC_CR_ADSTP) != 0U); //Wait finish stop sequence
                }
            }
        #elif TARGET_STM32F4
            _Conversor->CR2 &= 0xBFFFFFFF; //Stop convertion
        #endif
        return(0); //Convertion stoped with success
    }
    else return(-1); //ADC not initialized
}

uint8_t AnalogInput::is_enabled()
{
    #ifdef TARGET_STM32L4
        if ((_Conversor->CR & ADC_CR_ADEN) != 0U) //Check if the ADC is enable
            return 1; //ADC is enable
    #elif TARGET_STM32F4
        if ((_Conversor->CR2 & 0x00000001) != 0U) //Check if the ADC is enable
            return 1; //ADC is enable
    #endif
    else
        return 0; //ADC is unable
}

uint8_t AnalogInput::is_started()
{
    #ifdef TARGET_STM32L4
        if (_injection_convertion)
        {
            if ((_Conversor->CR & ADC_CR_JADSTART) != 0U) //Check if the conversion is in progress
                return 1; //Convertion is in progress
        }
        else
        {
            if ((_Conversor->CR & ADC_CR_ADSTART) != 0U) //Check if the conversion is in progress
                return 1; //Convertion is in progress
        }
    #elif TARGET_STM32F4
        if ((_Conversor->SR & 0x00000010) != 0) //Check if the conversion is in progress
            return 1; //Convertion is in progress
    #endif
    return 0; //Convertion isn't in progress
}

uint8_t AnalogInput::data_ready()
{
    #ifdef TARGET_STM32L4
        if (_injection_convertion)
        {
            if ((_Conversor->ISR & ADC_ISR_JEOC) != 0U)
                return 1;
        }
        else
        {
            if ((_Conversor->ISR & ADC_ISR_EOC) != 0U) //Check if the conversion completed
                return 1; //Conversion completed
        }
    #elif TARGET_STM32F4
        if (_injection_convertion)
        {
            if ((_Conversor->SR & ADC_SR_JEOC) != 0)
                return 1;
        }
        else
        {
            if ((_Conversor->SR & ADC_SR_EOC) != 0) //Check if the conversion completed
                return 1; //Conversion completed
        }
    #endif
        return 0; //Conversion didn't complete
}

uint16_t AnalogInput::read_converted_value()
{
    if (_injection_convertion) return _Conversor->JDR1;
    else return _Conversor->DR; //Read the converted value
}

void AnalogInput::reset_complete_flag()
{
    #ifdef TARGET_STM32L4
        if (_injection_convertion) _Conversor->ISR |= ADC_ISR_JEOC;
        else _Conversor->ISR |= ADC_ISR_EOC; //Reset the complete conversion flag
    #elif TARGET_STM32F4
        if (_injection_convertion) _Conversor->SR &= ~ADC_SR_JEOC;
        else _Conversor->SR &= ~ADC_SR_EOC; //Reset the complete conversion flag
    #endif
}

float AnalogInput::read_average_float()
{
    uint16_t average = 0;
    if (_injection_convertion) average = read_last_word();
    else average = read_average_word();
    #ifdef TARGET_STM32L4
        if ((_Conversor->CFGR & ADC_CFGR_ALIGN) != 0) return ((float)average)/((float)0xFFFF);
    #elif TARGET_STM32F4
        if ((_Conversor->CR2 & ADC_CR2_ALIGN) != 0) return ((float)average)/((float)0xFFFF);
    #endif
    else return ((float)average)/((float)0x0FFF);
}

uint32_t AnalogInput::read_average_word()
{
    uint32_t i;
    uint32_t sum = 0;
    uint32_t average = 0;

    if (is_enabled() == 0) return 0;

    if (_injection_convertion)
    {
        average = read_last_word();
    }
    else if (_usage_dma)
    {
        for (i = 0; i < _Size_buffer; i++) sum += _pointer[i];
        if (_usage_fast_divider) average = sum>>_Divider;
        else average = sum/_Divider;
    }
    else if (_continuous_mode)
    {
        average = read_converted_value();
        reset_complete_flag();
    }
    else
    {
        for (i = 0; i < _Size_buffer; i++) sum += read_last_word();
        if (_usage_fast_divider) average = sum>>_Divider;
        else average = sum/_Divider;
    }
    return(average);
}

float AnalogInput::read_last_float()
{
    uint16_t last_value = read_last_word();
    #ifdef TARGET_STM32L4
        if ((_Conversor->CFGR & ADC_CFGR_ALIGN) != 0) return ((float)last_value)/((float)0xFFFF);
    #elif TARGET_STM32F4
        if ((_Conversor->CR2 & ADC_CR2_ALIGN) != 0) return ((float)last_value)/((float)0xFFFF);
    #endif
    else return ((float)last_value)/((float)0x0FFF);
}

uint16_t AnalogInput::read_last_word()
{
    uint16_t last_value;
    uint32_t last_pointer;
    uint32_t actual_buffer_position;

    if (is_enabled() == 0) return 0;

    if (_usage_dma)
    {
        #ifdef TARGET_STM32L4
        actual_buffer_position = _Stream_DMA->CNDTR;
        #elif TARGET_STM32F4
            actual_buffer_position = _Stream_DMA->NDTR;
        #endif
        last_pointer = _Size_buffer - actual_buffer_position;
        if (last_pointer == 0) last_pointer = _Size_buffer-1;
        else last_pointer -= 1;
        last_value = _pointer[last_pointer];
    }
    else if (_continuous_mode)
    {
        last_value = read_converted_value();
        reset_complete_flag();
    }
    else
    {
        start();
        while (data_ready() == 0);
        last_value = read_converted_value();
        reset_complete_flag();
    }
    return last_value;
}