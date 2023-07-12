#ifndef __USART_interface_H
#define __USART_interface_H

#include "mbed.h"
#include "PeripheralPins.h"

#include "Clock.h"
// #ifdef __cplusplus
//  extern "C" {
// #endif //__cplusplus

#ifndef HAL_DMA_MODULE_ENABLE
#define HAL_DMA_MODULE_ENABLE
#endif //HAL_DMA_MODULE_ENABLE

#ifndef HAL_USART_MODULE_ENABLED
#define HAL_USART_MODULE_ENABLED
#endif //HAL_USART_MODULE_ENABLED

typedef enum{
    #ifdef TARGET_STM32L4
        C_7bits = USART_WORDLENGTH_7B,
        C_8bits = USART_WORDLENGTH_8B,
        C_9bits = USART_WORDLENGTH_9B
    #endif
    #ifdef TARGET_STM32F4
        C_8bits = USART_WORDLENGTH_8B,
        C_9bits = USART_WORDLENGTH_9B
    #endif
} Word_length;

typedef enum{
    #ifdef TARGET_STM32L4
        C_Parity_None = USART_PARITY_NONE,
        C_Parity_Odd = USART_PARITY_EVEN,
        C_Parity_Even = USART_PARITY_EVEN
    #endif
    #ifdef TARGET_STM32F4
        C_Parity_None = USART_PARITY_NONE,
        C_Parity_Odd = USART_PARITY_EVEN,
        C_Parity_Even = USART_PARITY_EVEN
    #endif
} Parity_bits;

typedef enum{
    #ifdef TARGET_STM32L4
        C_05stop = USART_STOPBITS_0_5,
        C_1stop = USART_STOPBITS_1,
        C_15stop = USART_STOPBITS_1_5,
        C_2stop = USART_STOPBITS_2
    #endif
    #ifdef TARGET_STM32F4
        C_05stop = USART_STOPBITS_0_5,
        C_1stop = USART_STOPBITS_1,
        C_15stop = USART_STOPBITS_1_5,
        C_2stop = USART_STOPBITS_2
    #endif
} Stop_bits;

typedef enum{
    #ifdef TARGET_STM32L4
        C_Non_DMA = 0,
        C_RX_DMA = 1,
        C_TX_DMA = 2,
        C_RX_TX_DMA = 3,
        C_RX_CIRC_DMA = 5,
        C_RX_CIRC_TX_DMA = 7 //(USART_CR3_DMAR | USART_CR3_DMAT)
    #endif
    #ifdef TARGET_STM32F4
        C_Non_DMA = 0,
        C_RX_DMA = 1,
        C_TX_DMA = 2,
        C_RX_TX_DMA = 3,
        C_RX_CIRC_DMA = 5,
        C_RX_CIRC_TX_DMA = 7 //(USART_CR3_DMAR | USART_CR3_DMAT)
    #endif
} DMA_conf;

typedef enum{
    #ifdef TARGET_STM32L4
        C_Use_RX = USART_CR1_RE,
        C_Use_TX = USART_CR1_TE,
        C_Use_RX_TX = USART_CR1_RE | USART_CR1_TE
    #endif
    #ifdef TARGET_STM32F4
        C_Use_RX = USART_CR1_RE,
        C_Use_TX = USART_CR1_TE,
        C_Use_RX_TX = USART_CR1_RE | USART_CR1_TE
    #endif
} Mode;

typedef enum{
    #ifdef TARGET_STM32L4
        C_Unable_Overrun = USART_CR3_OVRDIS,
        C_Enable_Overrun = 0
    #endif
    #ifdef TARGET_STM32F4
        C_Enable_Overrun = 0
    #endif
} RX_conf;

#define D_length    C_8bits
#define D_parity    C_Parity_None
#define D_stopbit   C_1stop
#define D_mode      C_Use_RX_TX

typedef enum{
    S_fail_baudrate_wrong = -110,
    S_fail_DMA_TX_unavailable = -109,
    S_fail_DMA_RX_unavailable = -108,
    S_fail_DMA_TX_buffer = -107,
    S_fail_DMA_RX_buffer = -106,
    S_fail_DMA_TX_unable = -105,
    S_fail_DMA_RX_unable = -104,
    S_fail_TX_RX_different = -103,
    S_fail_TX_unavailable = -102,
    S_fail_RX_unavailable = -101,
    S_fail_USART_unavailable = -100,
    S_using_rx_dma = -16,
    S_using_tx_dma = -15,
    S_dma_transmit_process = -14,
    S_tx_dma_unavailable = -13,
    S_rx_dma_unavailable = -12,
    S_rx_buffer_already_empty = -11,
    S_rx_buffer_already_full = -10,
    S_tx_buffer_already_empty = -9,
    S_tx_buffer_already_full = -8,
    S_rx_buffer_not_initialized = -7,
    S_tx_buffer_not_initialized = -6,
    S_transmit_busy = -5,
    //S_transmit_unable = -4,
    //S_receive_unable = -3,
    S_unable = -2,
    S_not_initialized = -1,
    S_ok = 0,
    S_USART_enable = 1,
    S_USART_unable = 2,
    S_already_enable = 3,
    S_already_unable = 4,
    S_receive_enable = 5,
    S_receive_unable = 6,
    S_receive_already_enable = 7,
    S_receive_already_unable = 8,
    S_transmit_enable = 9,
    S_transmit_unable = 10,
    S_transmit_already_enable = 11,
    S_transmit_already_unable = 12,
    S_data_available = 13,
    S_data_unavailable = 14,
    S_tx_buffer_empty = 15,
    S_tx_buffer_full = 16,
    S_rx_buffer_empty = 17,
    S_rx_buffer_full = 18,
    S_tx_dma_free = 19,
} USART_status;

typedef struct {
    uint32_t *ISRP;
    uint32_t *CISRP;
    uint32_t ISRB;
    int16_t IRQn;
    uint32_t IRQ_address;
} typedefDMA;

class USART_interface {
    public:
        typedef void (*usart_callback_rx)(USART_interface *obj, uint8_t last, uint8_t length);
        typedef void (*usart_callback_tx)(USART_interface *obj);
        static USART_interface* _ptr[];
        USART_status _status;
        USART_interface(PinName _pin_tx, PinName _pin_rx, Word_length _length, Mode _mode, uint32_t _baudrate, Parity_bits _parity, Stop_bits _stopbit, DMA_conf _dma_conf, RX_conf _overrun, uint16_t _size_tx, uint16_t _size_rx);
        USART_interface(PinName _pin_tx, PinName _pin_rx, Word_length _length, Mode _mode, uint32_t _baudrate, Parity_bits _parity, Stop_bits _stopbit, DMA_conf _dma_conf, uint16_t _size_tx, uint16_t _size_rx);
        USART_interface(PinName _pin_tx, PinName _pin_rx, uint32_t _baudrate, DMA_conf _dma_conf, uint16_t _size_tx, uint16_t _size_rx);
        USART_status init(PinName _pin_tx, PinName _pin_rx, Word_length _length, Mode _mode, uint32_t _baudrate, Parity_bits _parity, Stop_bits _stopbit, DMA_conf _dma_conf, RX_conf _overrun, uint16_t _size_tx, uint16_t _size_rx);
        USART_status enable();
        USART_status unable();
        USART_status receive_enable();
        USART_status receive_unable();
        USART_status transmit_enable();
        USART_status transmit_unable();
        USART_status is_enable();
        USART_status is_receive_enable();
        USART_status is_transmit_enable();
        USART_status is_data_available();
        USART_status is_data_buffer(uint16_t *_length);
        USART_status read_data(uint8_t *_data);
        USART_status write_data(uint8_t _data, bool _wait);
        USART_status send(bool wait = true);
        USART_status send(uint16_t size, bool wait = true);
        USART_status get_tx_buffer(uint8_t *_data);
        USART_status put_rx_buffer(uint8_t _data);
        
        USART_status start_dma_transmit();
        USART_status is_tx_dma_free();
        USART_status flush_rx_buffer();
        USART_status flush_tx_buffer();
        bool is_tx_buffer_empty();
        uint8_t *get_rx_buffer_pointer();
        uint32_t get_rx_buffer_dma_position();
        uint8_t *get_tx_buffer_pointer(bool wait = false);
        uint32_t get_tx_buffer_dma_position();
        
        USART_status set_end_caracter(uint8_t end_caracter, bool use_escape = false, uint8_t escape_caracter = 0x00);

        USART_status enable_rx_interrupt(usart_callback_rx _callback);
        USART_status unable_rx_interrupt(void);
        USART_status enable_tx_interrupt(usart_callback_tx _callback);
        USART_status unable_tx_interrupt(void);
        

        USART_status printf(const char * format, ...) MBED_PRINTF_METHOD(1, 2);
        USART_status printf(bool wait, const char * format, ...) MBED_PRINTF_METHOD(2, 3);
        USART_status bprintf(const char * format, ...) MBED_PRINTF_METHOD(1, 2);
        USART_status bprintf(bool wait, const char * format, ...) MBED_PRINTF_METHOD(2, 3);

        template<typename T> 
        USART_status put_tx_buffer(T _data) {return put_tx_buffer_int(sizeof(T), (uint8_t*)&_data);}

        template<typename T>
        USART_status put_tx_buffer(uint16_t len, T *_data) {
            _status = S_ok;
            while (len > 0)
            {
                len--;
                _status = put_tx_buffer_int(sizeof(T), (uint8_t*)_data);
                if (_status != S_ok) break;
                _data++;
            }
            return (_status);
        }

        template<typename T>
        USART_status get_rx_buffer(T *_data) {return get_rx_buffer_int(sizeof(T), (uint8_t*)_data);} 

        template<typename T>
        USART_status get_rx_buffer(uint16_t *len, T *_data) {
            uint16_t read = 0;
            _status = S_ok;
            if (*len == 0) *len = _size_buffer_rx;
            while (*len > 0)
            {
                *len--;
                _status = get_rx_buffer_int(sizeof(T), (uint8_t*)_data);
                if (_status != S_ok)
                {
                    if (_status == S_rx_buffer_empty) read++;
                    *len = read; 
                    break;
                }
                read++;
                _data++;
            }
            return (_status);
        }

    private:
        USART_TypeDef * _interface;
        #ifdef TARGET_STM32L4
            DMA_Channel_TypeDef *_DMA_Channel_RX;
            DMA_Channel_TypeDef *_DMA_Channel_TX;
        #elif TARGET_STM32F4
            DMA_Stream_TypeDef *_DMA_Channel_RX;
            DMA_Stream_TypeDef *_DMA_Channel_TX;
        #endif

        typedefDMA _DMA_RX;
        typedefDMA _DMA_TX;

        bool _initialized = false;
        bool _transmit_completed;
        bool _receive_buffer_full;
        bool _use_rx;
        bool _use_tx;
        bool _use_rx_dma;
        bool _use_tx_dma;
        bool _use_rx_IRQ;
        bool _use_tx_IRQ;
        uint16_t _size_buffer_rx;
        uint16_t _size_buffer_tx;
        uint8_t *_buffer_rx;
        uint8_t *_buffer_tx;
        uint16_t _input_pointer_rx;
        uint16_t _output_pointer_rx;
        uint16_t _input_pointer_tx;
        uint16_t _output_pointer_tx;
        bool _buffer_rx_full;
        bool _buffer_rx_empty;
        bool _buffer_tx_full;
        bool _buffer_tx_empty;
        bool _dma_transfer_run;
        bool _buffer_rx_circ;
        uint32_t _dma_rx_channel;
        uint32_t _dma_rx_stream;
        uint32_t _dma_tx_channel;
        uint32_t _dma_tx_stream;
        uint8_t _last_received;
        bool _use_end;
        bool _use_escape;
        bool _received_escape;
        bool _received_end;
        bool _reset_escape_flag;
        uint8_t _end_caracter;
        uint8_t _escape_caracter;
        int16_t _IRQn;
        uint32_t _IRQ_address;
        uint32_t _ISR_mask;

        USART_status get_rx_buffer_int(uint8_t *_data);
        USART_status get_rx_buffer_int(uint16_t len, uint8_t *_data);
        USART_status put_tx_buffer_int(uint8_t _data);
        USART_status put_tx_buffer_int(uint16_t len, uint8_t *buf);

        void interrupt_usart_tx_rx(void);
        void interrupt_dma_rx(void);
        void interrupt_dma_tx(void);

        #ifdef USART1
            static void interrupt_usart1(void) {if (USART_interface::_ptr[0] != NULL) USART_interface::_ptr[0]->interrupt_usart_tx_rx();}
            static void interrupt_usart1_dma_rx(void) {if (USART_interface::_ptr[0] != NULL) USART_interface::_ptr[0]->interrupt_dma_rx();}
            static void interrupt_usart1_dma_tx(void) {if (USART_interface::_ptr[0] != NULL) USART_interface::_ptr[0]->interrupt_dma_tx();}
        #endif
        #ifdef USART2
            static void interrupt_usart2(void) {if (USART_interface::_ptr[1] != NULL) USART_interface::_ptr[1]->interrupt_usart_tx_rx();}
            static void interrupt_usart2_dma_rx(void) {if (USART_interface::_ptr[1] != NULL) USART_interface::_ptr[1]->interrupt_dma_rx();}
            static void interrupt_usart2_dma_tx(void) {if (USART_interface::_ptr[1] != NULL) USART_interface::_ptr[1]->interrupt_dma_tx();}
        #endif
        #ifdef USART3
            static void interrupt_usart3(void) {if (USART_interface::_ptr[2] != NULL) USART_interface::_ptr[2]->interrupt_usart_tx_rx();}
            static void interrupt_usart3_dma_rx(void) {if (USART_interface::_ptr[2] != NULL) USART_interface::_ptr[2]->interrupt_dma_rx();}
            static void interrupt_usart3_dma_tx(void) {if (USART_interface::_ptr[2] != NULL) USART_interface::_ptr[2]->interrupt_dma_tx();}
        #endif
        #ifdef USART4
            static void interrupt_usart4(void) {if (USART_interface::_ptr[3] != NULL) USART_interface::_ptr[3]->interrupt_usart_tx_rx();}
            static void interrupt_usart4_dma_rx(void) {if (USART_interface::_ptr[3] != NULL) USART_interface::_ptr[3]->interrupt_dma_rx();}
            static void interrupt_usart4_dma_tx(void) {if (USART_interface::_ptr[3] != NULL) USART_interface::_ptr[3]->interrupt_dma_tx();}
        #endif
        #ifdef USART5
            static void interrupt_usart5(void) {if (USART_interface::_ptr[4] != NULL) USART_interface::_ptr[4]->interrupt_usart_tx_rx();}
            static void interrupt_usart5_dma_rx(void) {if (USART_interface::_ptr[4] != NULL) USART_interface::_ptr[4]->interrupt_dma_rx();}
            static void interrupt_usart5_dma_tx(void) {if (USART_interface::_ptr[4] != NULL) USART_interface::_ptr[4]->interrupt_dma_tx();}
        #endif
        #ifdef USART6
            static void interrupt_usart6(void) {if (USART_interface::_ptr[5] != NULL) USART_interface::_ptr[5]->interrupt_usart_tx_rx();}
            static void interrupt_usart6_dma_rx(void) {if (USART_interface::_ptr[5] != NULL) USART_interface::_ptr[5]->interrupt_dma_rx();}
            static void interrupt_usart6_dma_tx(void) {if (USART_interface::_ptr[5] != NULL) USART_interface::_ptr[5]->interrupt_dma_tx();}
        #endif


        // void (*callback_rx)(USART_interface *obj, uint8_t last, uint8_t length) = NULL;
        // void (*callback_tx)(USART_interface *obj) = NULL;
        usart_callback_rx callback_rx = NULL;
        usart_callback_tx callback_tx = NULL; 
};

// #ifdef __cplusplus
// }
// #endif //__cplusplus

#endif //__USART_interface_H
