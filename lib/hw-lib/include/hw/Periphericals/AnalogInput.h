#ifndef ANALOG_INPUT_H
#define ANALOG_INPUT_H

#include "mbed.h"
#include "PeripheralPins.h"

/** @brief Class of the analog input
 * 
 * In this class is possible configure the analog hardware to operate in continuos mode and use the
 * DMA resource.
 * 
 */

/**
 * Enumeration of prescaler for ADC clock
 */
typedef enum{
    #ifdef TARGET_STM32L4
        ADC_SPCLK1 = ADC_CLOCK_SYNC_PCLK_DIV1,  /**< ADC_SPCLK1 : Use prescaler = 1 (AHB/1) */
        ADC_SPCLK2 = ADC_CLOCK_SYNC_PCLK_DIV2,  /**< ADC_SPCLK2 : Use prescaler = 2 (AHB/2) */
        ADC_SPCLK4 = ADC_CLOCK_SYNC_PCLK_DIV4,  /**< ADC_SPCLK4 : Use prescaler = 4 (AHB/4) */
        #if (!defined(STM32L412xx) && !defined(STM32L422xx))
            ADC_PCLK1 = ADC_CLOCK_ASYNC_DIV1,       /**< ADC_PCLK1 : Use prescaler = 1 (PCLK2/1) */
            ADC_PCLK2 = ADC_CLOCK_ASYNC_DIV2,       /**< ADC_PCLK2 : Use prescaler = 2 (PCLK2/2) */
            ADC_PCLK4 = ADC_CLOCK_ASYNC_DIV4,       /**< ADC_PCLK4 : Use prescaler = 4 (PCLK2/4) */
            ADC_PCLK6 = ADC_CLOCK_ASYNC_DIV6,       /**< ADC_PCLK6 : Use prescaler = 6 (PCLK2/6) */
            ADC_PCLK8 = ADC_CLOCK_ASYNC_DIV8,       /**< ADC_PCLK8 : Use prescaler = 8 (PCLK2/8) */
            ADC_PCLK10 = ADC_CLOCK_ASYNC_DIV10,     /**< ADC_PCLK10 : Use prescaler = 10 (PCLK2/10) */
            ADC_PCLK12 = ADC_CLOCK_ASYNC_DIV12,     /**< ADC_PCLK12 : Use prescaler = 12 (PCLK2/12) */
            ADC_PCLK16 = ADC_CLOCK_ASYNC_DIV16,     /**< ADC_PCLK16 : Use prescaler = 16 (PCLK2/16) */
            ADC_PCLK32 = ADC_CLOCK_ASYNC_DIV32,     /**< ADC_PCLK32 : Use prescaler = 32 (PCLK2/32) */
            ADC_PCLK64 = ADC_CLOCK_ASYNC_DIV64,     /**< ADC_PCLK64 : Use prescaler = 64 (PCLK2/64) */
            ADC_PCLK128 = ADC_CLOCK_ASYNC_DIV128,   /**< ADC_PCLK128 : Use prescaler = 128 (PCLK2/128) */
            ADC_PCLK256 = ADC_CLOCK_ASYNC_DIV256    /**< ADC_PCLK256 : Use prescaler = 256 (PCLK2/256) */
        #endif
    #elif TARGET_STM32F4
        ADC_PCLK2 = ADC_CLOCK_SYNC_PCLK_DIV2,   /**< ADC_PCLK2 : Use prescaler = 2 (PCLK2/2) */
        ADC_PCLK4 = ADC_CLOCK_SYNC_PCLK_DIV4,   /**< ADC_PCLK4 : Use prescaler = 4 (PCLK2/4) */
        ADC_PCLK6 = ADC_CLOCK_SYNC_PCLK_DIV6,   /**< ADC_PCLK6 : Use prescaler = 6 (PCLK2/6) */
        ADC_PCLK8 = ADC_CLOCK_SYNC_PCLK_DIV8    /**< ADC_PCLK8 : Use prescaler = 8 (PCLK2/8) */
    #endif
} ADCPrescaler;

/**
 * Enumeration of alignment of the convertion result
 */
typedef enum{
    ADC_Left = ADC_DATAALIGN_LEFT,  /**< ADC_Left : Alignment on the left. Bits on the right is 0 */
    ADC_Right = ADC_DATAALIGN_RIGHT /**< ADC_Right : Alignment on the right. Bits on the left is 0 */
} ADCAlign;

/**
 * Enumeration of sample time
 */
typedef enum{
    #ifdef TARGET_STM32L4
        ADC_2s5 = ADC_SAMPLETIME_2CYCLES_5,
        ADC_6s5 = ADC_SAMPLETIME_6CYCLES_5,
        ADC_12s5 = ADC_SAMPLETIME_12CYCLES_5,
        ADC_24s5 = ADC_SAMPLETIME_24CYCLES_5,
        ADC_47s5 = ADC_SAMPLETIME_47CYCLES_5,
        ADC_92s5 = ADC_SAMPLETIME_92CYCLES_5,
        ADC_247s5 = ADC_SAMPLETIME_247CYCLES_5,
        ADC_640s5 = ADC_SAMPLETIME_640CYCLES_5
    #elif TARGET_STM32F4
        ADC_3s = ADC_SAMPLETIME_3CYCLES,        /**< ADC_3s : Use 3 clock for sample */
        ADC_15s = ADC_SAMPLETIME_15CYCLES,      /**< ADC_15s : Use 15 clock for sample */
        ADC_28s = ADC_SAMPLETIME_28CYCLES,      /**< ADC_28s : Use 28 clock for sample */
        ADC_56s = ADC_SAMPLETIME_56CYCLES,      /**< ADC_56s : Use 56 clock for sample */
        ADC_84s = ADC_SAMPLETIME_84CYCLES,      /**< ADC_84s : Use 84 clock for sample */
        ADC_112s = ADC_SAMPLETIME_112CYCLES,    /**< ADC_112s : Use 112 clock for sample */
        ADC_144s = ADC_SAMPLETIME_144CYCLES,    /**< ADC_144s : Use 144 clock for sample */
        ADC_480s = ADC_SAMPLETIME_480CYCLES     /**< ADC_480s : Use 480 clock for sample */
    #endif
} ADCSample;

/**
 * Enumeration of resolution value
 */ 
typedef enum{
    ADC_6b = ADC_RESOLUTION_6B,     /**< ADC_6b : Use 6 bits. For left align b'XXXXXX00_00000000'. For right align b'00000000_00XXXXXX' */
    ADC_8b = ADC_RESOLUTION_8B,     /**< ADC_8b : Use 8 bits. For left align b'XXXXXXXX_00000000'. For right align b'00000000_XXXXXXXX' */
    ADC_10b = ADC_RESOLUTION_10B,   /**< ADC_10b : Use 10 bits. For left align b'XXXXXXXX_XX000000'. For right align b'000000XX_XXXXXXXX' */
    ADC_12b = ADC_RESOLUTION_12B    /**< ADC_12b : Use 12 bits. For left align b'XXXXXXXX_XXXX0000'. For right align b'0000XXXX_XXXXXXXX' */
} ADCResolution;

/**
 * Enumeration of to use the continuos mode
 */
typedef enum{
    #ifdef TARGET_STM32L4
        ADC_Non_Continuous = 0U,                     /**< ADC_Non_Continuos : Don't use the continuos mode */
        ADC_Continuous = ADC_CFGR_CONT               /**< ADC_Continuos : Use the continuos mode */
    #elif TARGET_STM32F4
        ADC_Non_Continuous = ADC_CR2_CONTINUOUS(0U), /**< ADC_Non_Continuos : Don't use the continuos mode */
        ADC_Continuous = ADC_CR2_CONTINUOUS(1U)      /**< ADC_Continuos : Use the continuos mode */
    #endif
} ADCContinuous;

/**
 * Enumeration of to use the DMA
 */
typedef enum{
    #ifdef TARGET_STM32L4
        ADC_Non_Dma = 0U,                   /**< ADC_Non_Dma : Don't use the DMA */
        ADC_Dma = ADC_CFGR_DMAEN | ADC_CFGR_DMACFG /**< ADC_Dma : Use the DMA */
    #elif TARGET_STM32F4
        ADC_Non_Dma = 0U,                   /**< ADC_Non_Dma : Don't use the DMA */
        ADC_Dma = ADC_CR2_DMA | ADC_CR2_DDS /**< ADC_Dma : Use the DMA */
    #endif
} ADCDma;

/**
 * @brief Structure of the Analog_Input class
 * 
 * Public methods is
 * * Analog_Input : constructor of the class. @see Analog_Input(PinName pin, ADCPrescaler Prescaler, ADCAlign Alignment, ADCSample Sample, ADCResolution Resolution, ADCContinuos Continuos, ADCDma Dma, uint32_t Size_buffer)
 * * enable : method for enable the ADC channel. @see int8_t enable()
 * * unable : method for unable the ADC channel. @see int8_t unable()
 * * start : method for start conversion. @see int8_t start()
 * * stop : method for stop convertson. @see int8_t stop()
 * * is_enabled : method for check if the ADC is enabled. @see uint8_t is_enabled()
 * * is_started : method for check if the conversion is started. @see uint8_t is_started()
 * * data_ready : method for check if the conversion is finished. @see uint8_t data_ready()
 * * read_average_float : method for read the average of the value as float. @see float read_average_float()
 * * read_average_word : method for read the average of the value as word. @see uint16_t read_average_word()
 * * read_last_float : method for read the last value converted as float. @see float read_last_float()
 * * read_last_word : method for read the last value converted as word. @see uint16_t read_last_word()
 * 
 * Public attributes is
 * 
 */
class AnalogInput
{
public:
    AnalogInput(PinName pin);
    AnalogInput(PinName pin, ADCPrescaler Prescaler, ADCAlign Alignment, ADCSample Sample, ADCResolution Resolution);
    AnalogInput(PinName pin, ADCPrescaler Prescaler, ADCAlign Alignment, ADCSample Sample, ADCResolution Resolution, ADCContinuous Continuous, ADCDma Dma, uint32_t Size_buffer);
    ~AnalogInput();
    int8_t enable();
    int8_t unable();
    int8_t start();
    int8_t stop();
    uint8_t is_enabled();
    uint8_t is_started();
    uint8_t data_ready();
    uint16_t read_converted_value();
    void reset_complete_flag();
    float read_average_float();
    uint32_t read_average_word();
    float read_last_float();
    uint16_t read_last_word();

    static uint8_t _using_ADC1;
    static uint8_t _using_ADC2;
    static uint8_t _using_ADC3;

private:
    ADC_TypeDef *_Conversor;
    #ifdef TARGET_STM32L4
        DMA_Channel_TypeDef *_Stream_DMA;
    #elif TARGET_STM32F4
        DMA_Stream_TypeDef *_Stream_DMA;
    #endif
    uint8_t _Channel;
    bool _initialized = false;
    bool _continuous_mode;
    bool _usage_dma;
    bool _usage_fast_divider;
    bool _injection_convertion;
    uint32_t _Size_buffer;
    uint32_t _Divider;
    uint16_t *_pointer;
};

#endif // ANALOG_INPUT_H