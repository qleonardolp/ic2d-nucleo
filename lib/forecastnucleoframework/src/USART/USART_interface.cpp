#include <USART/USART_interface.hpp>

USART_interface* USART_interface::_ptr[6] = {NULL, NULL, NULL, NULL, NULL, NULL};

USART_interface::USART_interface(PinName _pin_tx, PinName _pin_rx, Word_length _length, Mode _mode, uint32_t _baudrate, Parity_bits _parity, Stop_bits _stopbit, DMA_conf _dma_conf, RX_conf _overrun, uint16_t _size_tx, uint16_t _size_rx){
    init(_pin_tx, _pin_rx, _length, _mode, _baudrate, _parity, _stopbit, _dma_conf, _overrun, _size_tx, _size_rx);
}

USART_interface::USART_interface(PinName _pin_tx, PinName _pin_rx, Word_length _length, Mode _mode, uint32_t _baudrate, Parity_bits _parity, Stop_bits _stopbit, DMA_conf _dma_conf, uint16_t _size_tx, uint16_t _size_rx){
    init(_pin_tx, _pin_rx, _length, _mode, _baudrate, _parity, _stopbit, _dma_conf, C_Enable_Overrun, _size_tx, _size_rx);
}

USART_interface::USART_interface(PinName _pin_tx, PinName _pin_rx, uint32_t _baudrate, DMA_conf _dma_conf, uint16_t _size_tx, uint16_t _size_rx){
    #ifdef TARGET_STM32L4
        init(_pin_tx, _pin_rx, D_length, D_mode, _baudrate, D_parity, D_stopbit, _dma_conf, C_Unable_Overrun, _size_tx, _size_rx);
    #endif
    #ifdef TARGET_STM32F4
        init(_pin_tx, _pin_rx, D_length, D_mode, _baudrate, D_parity, D_stopbit, _dma_conf, C_Enable_Overrun, _size_tx, _size_rx);
    #endif
}

USART_status USART_interface::init(PinName _pin_tx, PinName _pin_rx, Word_length _length, Mode _mode, uint32_t _baudrate, Parity_bits _parity, Stop_bits _stopbit, DMA_conf _dma_conf, RX_conf _overrun, uint16_t _size_tx, uint16_t _size_rx){
    uint32_t function_RX = pinmap_function(_pin_rx, PinMap_UART_RX);
    uint32_t function_TX = pinmap_function(_pin_tx, PinMap_UART_TX);

    uint32_t clock_source = 0;
    uint32_t clock_usart = 0;
    float clock_div_a = 0.0f;
    uint32_t clock_div = 0;
    uint32_t oversampling = 0;

    uint32_t _config_dma = 0;

    USART_TypeDef * _USART_RX = (USART_TypeDef *)pinmap_peripheral(_pin_rx, PinMap_UART_RX);
    USART_TypeDef * _USART_TX = (USART_TypeDef *)pinmap_peripheral(_pin_tx, PinMap_UART_TX);

    if (_USART_RX == NULL) {
        _status = S_fail_RX_unavailable;
        return _status;
    }

    if (_USART_TX == NULL) {
        _status = S_fail_TX_unavailable;
        return _status;
    }

    if (_USART_RX != _USART_TX) {
        _status = S_fail_TX_RX_different;
        return _status;
    }

    _size_buffer_rx = _size_rx;
    _size_buffer_tx = _size_tx+1;

    if ((_mode & C_Use_RX) != 0) _use_rx = true;
    if ((_mode & C_Use_TX) != 0) _use_tx = true;

    if ((_dma_conf & C_RX_DMA) != 0) {
        if (_use_rx)
        {
            if (_size_buffer_rx > 0) _use_rx_dma = true;
            else
            {
                _status = S_fail_DMA_RX_buffer;
                return _status;
            }
            
        }
        else
        {
            _status = S_fail_DMA_RX_unable;
            return _status;
        }
    } else {
        _use_rx_dma = false;
    }
    
    if ((_dma_conf & C_TX_DMA) != 0) {
        if (_use_tx)
        {
            if (_size_buffer_tx > 0) _use_tx_dma = true;
            else
            {
                _status = S_fail_DMA_TX_buffer;
                return _status;
            }
            
        }
        else
        {
            _status = S_fail_DMA_TX_unable;
            return _status;
        }
    } else {
        _use_tx_dma = false;
    }

    if ((_USART_RX->CR1 & USART_CR1_UE) != 0) {
        _status = S_fail_USART_unavailable;
        return _status;
    }

    _interface = _USART_RX;

    #ifdef USART1
        if(_interface == USART1)
        {
            __HAL_RCC_USART1_CLK_ENABLE();
            _IRQn = USART1_IRQn;
            _IRQ_address = (uint32_t)&interrupt_usart1;
            _ptr[0] = this;
        }
    #endif
    #ifdef USART2
        if(_interface == USART2)
        {
            __HAL_RCC_USART2_CLK_ENABLE();
            _IRQn = USART2_IRQn;
            _IRQ_address = (uint32_t)&interrupt_usart2;
            _ptr[1] = this;
        }
    #endif
    #ifdef USART3
        if(_interface == USART3)
        {
            __HAL_RCC_USART3_CLK_ENABLE();
            _IRQn = USART3_IRQn;
            _IRQ_address = (uint32_t)&interrupt_usart3;
            _ptr[2] = this;
        }
    #endif
    #ifdef USART4
        if(_interface == USART4)
        {
            __HAL_RCC_USART4_CLK_ENABLE();
            _IRQn = USART4_IRQn;
            _IRQ_address = (uint32_t)&interrupt_usart4;
            _ptr[3] = this;
        }
    #endif
    #ifdef USART5
        if(_interface == USART5)
        {
            __HAL_RCC_USART5_CLK_ENABLE();
            _IRQn = USART5_IRQn;
            _IRQ_address = (uint32_t)&interrupt_usart5;
            _ptr[4] = this;
        }
    #endif
    #ifdef USART6
        if(_interface == USART6)
        {
            __HAL_RCC_USART6_CLK_ENABLE();
            _IRQn = USART6_IRQn;
            _IRQ_address = (uint32_t)&interrupt_usart6;
            _ptr[5] = this;
        }
    #endif


    #ifdef TARGET_STM32L4
        clock_usart = get_usart_clock(_interface);
    
        clock_div = clock_usart/_baudrate;

        if (clock_div < 16) {
            oversampling = USART_CR1_OVER8;
            clock_div = (clock_usart*2)/_baudrate;
            clock_div = (clock_div & 0xFFFFFFF0) | ((clock_div & 0x0000000F)>>1);
        }
    #elif TARGET_STM32F4
        clock_usart = get_usart_clock(_interface);
        clock_div_a = ((float)clock_usart)/(16.0f*((float)_baudrate));
        if (clock_div_a < 1.0f)
        {
            clock_div_a = ((float)clock_usart)/(8.0f*((float)_baudrate));
            if (clock_div_a < 1.0f)
            {
                _status = S_fail_baudrate_wrong;
                return _status;
            }
            else
            {
                oversampling = USART_CR1_OVER8;
                clock_div = (uint32_t)(roundf((clock_div_a-(uint32_t)clock_div_a)*8.0f));
                if (clock_div == 8) clock_div = 16;
                clock_div += ((uint32_t)clock_div_a)<<4;
            }
        }
        else
        {
            clock_div = (uint32_t)(roundf((clock_div_a-(uint32_t)clock_div_a)*16.0f));
            clock_div += ((uint32_t)clock_div_a)<<4;
        }
        
    #endif

    switch(_dma_conf) {
        case C_Non_DMA:
            _config_dma = 0;
            break;
        case C_RX_DMA:
        case C_RX_CIRC_DMA:
            _config_dma = USART_CR3_DMAR;
            break;
        case C_TX_DMA:
            _config_dma = USART_CR3_DMAT;
            break;
        case C_RX_TX_DMA:
        case C_RX_CIRC_TX_DMA:
            _config_dma = (USART_CR3_DMAR | USART_CR3_DMAT);
            break;
    }

    pinmap_pinout(_pin_rx, PinMap_UART_RX);
    pinmap_pinout(_pin_tx, PinMap_UART_TX);

    _interface->CR1 &= ~(USART_CR1_UE);
    while ((_interface->CR1 & (USART_CR1_UE)) != 0);

    #ifdef TARGET_STML4
        _interface->ICR = 0xFFFFFFFF;
    #elif TARGET_STM32F4
        _interface->SR = 0x00000000;
    #endif
    _interface->CR1 = _length | _parity | oversampling;
    _interface->CR2 = _stopbit;
    _interface->CR3 = _config_dma | _overrun;
    _interface->BRR = clock_div;
    
    if (_size_buffer_rx > 0) {
        _buffer_rx = (uint8_t *)malloc(_size_buffer_rx);
        _input_pointer_rx = 0;
        _output_pointer_rx = 0;
    }

    if (_size_buffer_tx > 0) {
        _buffer_tx = (uint8_t *)malloc(_size_buffer_tx);
        _input_pointer_tx = 0;
        _output_pointer_tx = 0;
    }

    if (_use_rx_dma) {
        #if defined(USART1)
            if (_interface == USART1)
            {
                _DMA_RX.IRQ_address = (uint32_t)&interrupt_usart1_dma_rx;
                #ifdef TARGET_STM32L4
                    if ((DMA1_Channel5->CCR & DMA_CCR_EN) == 0)
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _DMA_Channel_RX = DMA1_Channel5;
                        _DMA_RX.IRQn = DMA1_Channel5_IRQn;
                        _DMA_RX.CISRP = (uint32_t *)DMA1->IFCR;
                        _DMA_RX.ISRP = (uint32_t *)DMA1->ISR;
                        _dma_rx_channel = 5;
                        DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & 0xFFF0FFFF) | (DMA_REQUEST_2<<16);
                    }
                    else if ((DMA2_Channel7->CCR & DMA_CCR_EN) == 0)
                    {
                        if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                        _DMA_Channel_RX = DMA2_Channel7;
                        _DMA_RX.IRQn = DMA2_Channel7_IRQn;
                        _DMA_RX.CISRP = (uint32_t *)DMA2->IFCR;
                        _DMA_RX.ISRP = (uint32_t *)DMA2->ISR;
                        _dma_rx_channel = 7;
                        DMA2_CSELR->CSELR = (DMA2_CSELR->CSELR & 0xF0FFFFFFF) | (DMA_REQUEST_2<<24);
                    }
                #elif TARGET_STM32F4
                    if ((DMA2_Stream2->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                        _DMA_Channel_RX = DMA2_Stream2;
                        _DMA_RX.IRQn = DMA2_Stream2_IRQn;
                        _DMA_RX.CISRP = (uint32_t *)(DMA2_BASE+0x08);
                        _DMA_RX.ISRP = (uint32_t *)(DMA2_BASE+0x00);
                        _dma_rx_channel = DMA_CHANNEL_4;
                        _dma_rx_stream = 2;
                    }
                    else if ((DMA2_Stream5->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                        _DMA_Channel_RX = DMA2_Stream5;
                        _DMA_RX.IRQn = DMA2_Stream5_IRQn;
                        _DMA_RX.CISRP = (uint32_t *)(DMA2_BASE+0x0C);
                        _DMA_RX.ISRP = (uint32_t *)(DMA2_BASE+0x04);
                        _dma_rx_channel = DMA_CHANNEL_4;
                        _dma_rx_stream = 5;
                    }
                #endif //TARGET
                else
                {
                    _status = S_fail_DMA_RX_unavailable;
                    return _status;
                }
            }
        #endif
        #if defined(USART2)
            if (_interface == USART2)
            {
                _DMA_RX.IRQ_address = (uint32_t)&interrupt_usart2_dma_rx;
                #ifdef TARGET_STM32L4
                    if ((DMA1_Channel6->CCR & DMA_CCR_EN) == 0)
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _DMA_Channel_RX = DMA1_Channel6;
                        _DMA_RX.IRQn = DMA1_Channel6_IRQn;
                        _DMA_RX.CISRP = (uint32_t *)DMA1->IFCR;
                        _DMA_RX.ISRP = (uint32_t *)DMA1->ISR;
                        _dma_rx_channel = 6;
                        DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & 0xFF0FFFFF) | (DMA_REQUEST_2<<20);
                    }
                #elif TARGET_STM32F4
                    if ((DMA1_Stream5->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _DMA_Channel_RX = DMA1_Stream5;
                        _DMA_RX.IRQn = DMA1_Stream5_IRQn;
                        _DMA_RX.CISRP = (uint32_t *)(DMA1_BASE+0x0C);
                        _DMA_RX.ISRP = (uint32_t *)(DMA1_BASE+0x04);
                        _dma_rx_channel = DMA_CHANNEL_4;
                        _dma_rx_stream = 5;
                    }
                #endif //TARGET
                else
                {
                    _status = S_fail_DMA_RX_unavailable;
                    return _status;
                }
            }
        #endif
        #if defined(USART3)
            if (_interface == USART3)
            {
                _DMA_RX.IRQ_address = (uint32_t)&interrupt_usart3_dma_rx;
                #ifdef TARGET_STM32L4
                    if ((DMA1_Channel3->CPAR == 0) && (DMA1_Channel3->CMAR == 0))
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _DMA_Channel_RX = DMA1_Channel3;
                        _DMA_RX.IRQn = DMA1_Channel3_IRQn;
                        _DMA_RX.CISRP = (uint32_t *)DMA1->IFCR;
                        _DMA_RX.ISRP = (uint32_t *)DMA1->ISR;
                        _dma_rx_channel = 3;
                        DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & 0xFFFFF0FF) | (DMA_REQUEST_2<<8);
                    }
                #elif TARGET_STM32F4
                    if ((DMA1_Stream1->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _DMA_Channel_RX = DMA1_Stream1;
                        _DMA_RX.IRQn = DMA1_Stream1_IRQn;
                        _DMA_RX.CISRP = (uint32_t *)(DMA1_BASE+0x08);
                        _DMA_RX.ISRP = (uint32_t *)(DMA1_BASE+0x00);
                        _dma_rx_channel = DMA_CHANNEL_4;
                        _dma_rx_stream = 1;
                    }
                #endif //TARGET
                else
                {
                    _status = S_fail_DMA_RX_unavailable;
                    return _status;
                }
            }
        #endif
        #if defined(USART4)
            if (_interface == USART4)
            {
                _DMA_RX.IRQ_address = (uint32_t)&interrupt_usart4_dma_rx;
                if ((DMA2_Channel5->CPAR == 0) && (DMA2_Channel5->CMAR == 0))
                {
                    if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                    _DMA_Channel_RX = DMA2_Channel5;
                    _DMA_RX.IRQn = DMA2_Channel5_IRQn;
                    _DMA_RX.CISRP = (uint32_t *)DMA2->IFCR;
                    _DMA_RX.ISRP = (uint32_t *)DMA2->ISR;
                    _dma_rx_channel = 5;
                    DMA2_CSELR->CSELR = (DMA2_CSELR->CSELR & 0xFFF0FFFF) | (DMA_REQUEST_2<<16);
                }
                else
                {
                    _status = S_fail_DMA_RX_unavailable;
                    return _status;
                }
            }
        #endif
        #if defined(USART5)
        #endif
        #if defined(USART6)
            if (_interface == USART6)
            {
                _DMA_RX.IRQ_address = (uint32_t)&interrupt_usart6_dma_rx;
                #ifdef TARGET_STM32F4
                    if ((DMA2_Stream1->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                        _DMA_Channel_RX = DMA2_Stream1;
                        _DMA_RX.IRQn = DMA2_Stream1_IRQn;
                        _DMA_RX.CISRP = (uint32_t *)(DMA2_BASE+0x08);
                        _DMA_RX.ISRP = (uint32_t *)(DMA2_BASE+0x00);
                        _dma_rx_channel = DMA_CHANNEL_5;
                        _dma_rx_stream = 1;
                    }
                    else if ((DMA2_Stream2->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                        _DMA_Channel_RX = DMA2_Stream2;
                        _DMA_RX.IRQn = DMA2_Stream2_IRQn;
                        _DMA_TX.CISRP = (uint32_t *)(DMA2_BASE+0x08);
                        _DMA_TX.ISRP = (uint32_t *)(DMA2_BASE+0x00);
                        _dma_rx_channel = DMA_CHANNEL_5;
                        _dma_rx_stream = 2;
                    }
                #endif //TARGET
                else
                {
                    _status = S_fail_DMA_RX_unavailable;
                    return _status;
                }
            }
        #endif
        #ifdef TARGET_STM32L4
            _DMA_Channel_RX->CCR = DMA_CCR_MINC | DMA_CCR_PL;
            if ((_dma_conf == C_RX_CIRC_DMA) || (_dma_conf == C_RX_CIRC_TX_DMA))
            {
                _DMA_Channel_RX->CCR |= DMA_CCR_CIRC;
                _buffer_rx_circ = true;
            }
            else _buffer_rx_circ = false;
            _DMA_Channel_RX->CNDTR = _size_buffer_rx;
            _DMA_Channel_RX->CPAR = (uint32_t)&_interface->RDR;
            _DMA_Channel_RX->CMAR = (uint32_t)_buffer_rx;
            _interface->CR3 |= USART_CR3_OVRDIS;
            _DMA_RX.ISRB = (0xF<<((_dma_tx_channel-1)*4));
        #elif TARGET_STM32F4
            _DMA_Channel_RX->CR = DMA_SxCR_MINC | DMA_SxCR_PL | _dma_rx_channel;
            if ((_dma_conf == C_RX_CIRC_DMA) || (_dma_conf == C_RX_CIRC_TX_DMA))
            {
                _DMA_Channel_RX->CR |= DMA_SxCR_CIRC;
                _buffer_rx_circ = true;
            }
            else _buffer_rx_circ = false;
            _DMA_Channel_RX->NDTR = _size_buffer_rx;
            _DMA_Channel_RX->PAR = (uint32_t)&_interface->DR;
            _DMA_Channel_RX->M0AR = (uint32_t)_buffer_rx;
            switch(_dma_rx_stream)
            {
                case 0:
                    _DMA_RX.ISRB = 0x0000003D;
                break;
                case 1:
                    _DMA_RX.ISRB = 0x00000F40;
                break;
                case 2:
                    _DMA_RX.ISRB = 0x003D0000;
                break;
                case 3:
                    _DMA_RX.ISRB = 0x0F400000;
                break;
                case 4:
                    _DMA_RX.ISRB = 0x0000003D;
                break;
                case 5:
                    _DMA_RX.ISRB = 0x00000F40;
                break;
                case 6:
                    _DMA_RX.ISRB = 0x003D0000;
                break;
                case 7:
                    _DMA_RX.ISRB = 0x0F400000;
                break;
            }
        #endif //TARGET
    }

    if (_use_tx_dma) {
        #if defined(USART1)
            if (_interface == USART1)
            {
                _DMA_TX.IRQ_address = (uint32_t)&interrupt_usart1_dma_tx;
                #ifdef TARGET_STM32L4
                    if ((DMA1_Channel4->CCR & DMA_CCR_EN) == 0)
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _DMA_Channel_TX = DMA1_Channel4;
                        _DMA_TX.IRQn = DMA1_Channel4_IRQn;
                        _DMA_TX.CISRP = (uint32_t *)DMA1->IFCR;
                        _DMA_TX.ISRP = (uint32_t *)DMA1->ISR;
                        _dma_tx_channel = 4;
                        DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & 0xFFFF0FFF) | (DMA_REQUEST_2<<12);
                    }
                    else if ((DMA2_Channel6->CCR & DMA_CCR_EN) == 0)
                    {
                        if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                        _DMA_Channel_TX = DMA2_Channel6;
                        _DMA_TX.IRQn = DMA2_Channel6_IRQn;
                        _DMA_TX.CISRP = (uint32_t *)DMA2->IFCR;
                        _DMA_TX.ISRP = (uint32_t *)DMA2->ISR;
                        _dma_tx_channel = 6;
                        DMA2_CSELR->CSELR = (DMA2_CSELR->CSELR & 0xFF0FFFFF) | (DMA_REQUEST_2<<20);
                    }
                #elif TARGET_STM32F4
                    if ((DMA2_Stream7->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                        _DMA_Channel_TX = DMA2_Stream7;
                        _DMA_TX.IRQn = DMA2_Stream7_IRQn;
                        _DMA_TX.CISRP = (uint32_t *)(DMA2_BASE+0x0C);
                        _DMA_TX.ISRP = (uint32_t *)(DMA2_BASE+0x04);
                        _dma_tx_channel = DMA_CHANNEL_4;
                        _dma_tx_stream = 7;
                    }
                #endif //TARGET
                else
                {
                    _status = S_fail_DMA_TX_unavailable;
                    return _status;
                }
            }
        #endif
        #if defined(USART2)
            if (_interface == USART2)
            {
                _DMA_TX.IRQ_address = (uint32_t)&interrupt_usart2_dma_tx;
                #ifdef TARGET_STM32L4
                    if ((DMA1_Channel7->CCR & DMA_CCR_EN) == 0)
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _DMA_Channel_TX = DMA1_Channel7;
                        _DMA_TX.IRQn = DMA1_Channel7_IRQn;
                        _DMA_TX.CISRP = (uint32_t *)DMA1->IFCR;
                        _DMA_TX.ISRP = (uint32_t *)DMA1->ISR;
                        _dma_tx_channel = 7;
                        DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & 0xF0FFFFFF) | (DMA_REQUEST_2<<24);
                    }
                #elif TARGET_STM32F4
                    if ((DMA1_Stream6->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _DMA_Channel_TX = DMA1_Stream6;
                        _DMA_TX.IRQn = DMA1_Stream6_IRQn;
                        _DMA_TX.CISRP = (uint32_t *)(DMA1_BASE+0x0C);
                        _DMA_TX.ISRP = (uint32_t *)(DMA1_BASE+0x04);
                        _dma_tx_channel = DMA_CHANNEL_4;
                        _dma_tx_stream = 6;
                    }
                #endif //TARGET
                else
                {
                    _status = S_fail_DMA_TX_unavailable;
                    return _status;
                }
            }
        #endif
        #if defined(USART3)
            if (_interface == USART3)
            {
                _DMA_TX.IRQ_address = (uint32_t)&interrupt_usart3_dma_tx;
                #ifdef TARGET_STM32L4
                    if ((DMA1_Channel2->CPAR == 0) && (DMA1_Channel2->CMAR == 0))
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _DMA_Channel_TX = DMA1_Channel2;
                        _DMA_TX.IRQn = DMA1_Channel2_IRQn;
                        _DMA_TX.CISRP = (uint32_t *)DMA1->IFCR;
                        _DMA_TX.ISRP = (uint32_t *)DMA1->ISR;
                        _dma_tx_channel = 2;
                        DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & 0xFFFFFF0F) | (DMA_REQUEST_2<<4);
                    }
                #elif TARGET_STM32F4
                    if ((DMA1_Stream3->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _DMA_Channel_TX = DMA1_Stream3;
                        _DMA_TX.IRQn = DMA1_Stream3_IRQn;
                        _DMA_TX.CISRP = (uint32_t *)(DMA1_BASE+0x08);
                        _DMA_TX.ISRP = (uint32_t *)(DMA1_BASE+0x00);
                        _dma_tx_channel = DMA_CHANNEL_4;
                        _dma_tx_stream = 3;
                    }
                    else if ((DMA1_Stream4->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA1_IS_CLK_DISABLED()) __DMA1_CLK_ENABLE();
                        _DMA_Channel_TX = DMA1_Stream4;
                        _DMA_TX.IRQn = DMA1_Stream4_IRQn;
                        _DMA_TX.CISRP = (uint32_t *)(DMA1_BASE+0x08);
                        _DMA_TX.ISRP = (uint32_t *)(DMA1_BASE+0x00);
                        _dma_tx_channel = DMA_CHANNEL_7;
                        _dma_tx_stream = 4;
                    }
                #endif //TARGET
                else
                {
                    _status = S_fail_DMA_TX_unavailable;
                    return _status;
                }
            }
        #endif
        #if defined(USART4)
            if (_interface == USART4)
            {
                _DMA_TX.IRQ_address = (uint32_t)&interrupt_usart4_dma_tx;
                if ((DMA2_Channel3->CPAR == 0) && (DMA2_Channel3->CMAR == 0))
                {
                    if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                    _DMA_Channel_TX = DMA2_Channel3;
                    _DMA_TX.IRQn = DMA2_Channel3_IRQn;
                    _DMA_TX.CISRP = (uint32_t *)DMA2->IFCR;
                    _DMA_TX.ISRP = (uint32_t *)DMA2->ISR;
                    _dma_tx_channel = 3;
                    DMA2_CSELR->CSELR = (DMA2_CSELR->CSELR & 0xFFFF0FF) | (DMA_REQUEST_2<<8);
                }
                else
                {
                    _status = S_fail_DMA_TX_unavailable;
                    return _status;
                }
            }
        #endif
        #if defined(USART5)
        #endif
        #if defined(USART6)
            if (_interface == USART6)
            {
                _DMA_TX.IRQ_address = (uint32_t)&interrupt_usart6_dma_tx;
                #ifdef TARGET_STM32L4
                #elif TARGET_STM32F4
                    if ((DMA2_Stream6->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                        _DMA_Channel_TX = DMA2_Stream6;
                        _DMA_TX.IRQn = DMA2_Stream6_IRQn;
                        _DMA_TX.CISRP = (uint32_t *)(DMA2_BASE+0x0C);
                        _DMA_TX.ISRP = (uint32_t *)(DMA2_BASE+0x04);
                        _dma_tx_channel = DMA_CHANNEL_5;
                        _dma_tx_stream = 6;
                    }
                    else if ((DMA2_Stream7->CR & DMA_SxCR_EN) == 0)
                    {
                        if(__DMA2_IS_CLK_DISABLED()) __DMA2_CLK_ENABLE();
                        _DMA_Channel_TX = DMA2_Stream7;
                        _DMA_TX.IRQn = DMA2_Stream7_IRQn;
                        _DMA_TX.CISRP = (uint32_t *)(DMA2_BASE+0x0C);
                        _DMA_TX.ISRP = (uint32_t *)(DMA2_BASE+0x04);
                        _dma_tx_channel = DMA_CHANNEL_5;
                        _dma_tx_stream = 7;
                    }
                #endif //TARGET
                else
                {
                    _status = S_fail_DMA_TX_unavailable;
                    return _status;
                }
            }
        #endif
        #ifdef TARGET_STM32L4
            _DMA_Channel_TX->CCR = DMA_CCR_MINC | DMA_CCR_PL | DMA_CCR_DIR;
            _DMA_Channel_TX->CNDTR = _size_buffer_tx;
            _DMA_Channel_TX->CPAR = (uint32_t)&_interface->TDR;
            _DMA_Channel_TX->CMAR = (uint32_t)_buffer_tx;
            _DMA_TX.ISRB = (0xF<<((_dma_tx_channel-1)*4));
        #elif TARGET_STM32F4
            _DMA_Channel_TX->CR = DMA_SxCR_MINC | DMA_SxCR_PL | DMA_SxCR_DIR_0 | _dma_tx_channel;
            _DMA_Channel_TX->NDTR = _size_buffer_tx;
            _DMA_Channel_TX->PAR = (uint32_t)&_interface->DR;
            _DMA_Channel_TX->M0AR = (uint32_t)_buffer_tx;
            switch(_dma_tx_stream)
            {
                case 0:
                    _DMA_TX.ISRB = 0x0000003D;
                break;
                case 1:
                    _DMA_TX.ISRB = 0x00000F40;
                break;
                case 2:
                    _DMA_TX.ISRB = 0x003D0000;
                break;
                case 3:
                    _DMA_TX.ISRB = 0x0F400000;
                break;
                case 4:
                    _DMA_TX.ISRB = 0x0000003D;
                break;
                case 5:
                    _DMA_TX.ISRB = 0x00000F40;
                break;
                case 6:
                    _DMA_TX.ISRB = 0x003D0000;
                break;
                case 7:
                    _DMA_TX.ISRB = 0x0F400000;
                break;
            }
        #endif //TARGET
    }

    _initialized = true;
    _transmit_completed = false;
    _receive_buffer_full = false;
    _buffer_tx_empty = true;
    _buffer_tx_full = false;
    _buffer_rx_empty = true;
    _buffer_rx_full = false;
    _dma_transfer_run = false;

    _use_end = false;
    _use_escape = false;
    _received_end = false;
    _received_escape = false;

    _ISR_mask = 0;
    _use_tx_IRQ = false;
    _use_rx_IRQ = false;

    _status = S_ok;
}

USART_status USART_interface::enable(){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_UE) == 0)
        {
            #ifdef TARGET_STM32L4
                _interface->ICR = 0xFFFFFFFF;
            #endif
            _interface->CR1 |= USART_CR1_UE;
            while((_interface->CR1 & USART_CR1_UE) == 0);
        }
        else _status = S_already_enable;
    }
    else  _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::unable(){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_UE) != 0)
        {
            #ifdef TARGET_STM32L4
                _interface->ICR = 0xFFFFFFFF;
            #endif
            _interface->CR1 &= ~USART_CR1_UE;
            while((_interface->CR1 & USART_CR1_UE) != 0);
            _interface->CR1 &= ~(USART_CR1_TE | USART_CR1_RE);
        }
        else _status = S_already_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::receive_enable(){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_RE) == 0)
        {
            #ifdef TARGET_STM32L4
                _interface->ICR = USART_ICR_PECF | USART_ICR_FECF | USART_ICR_NCF | USART_ICR_ORECF | USART_ICR_IDLECF | USART_ICR_RTOCF;
                if (_use_rx_dma) _DMA_Channel_RX->CCR |= DMA_CCR_EN;
            #elif TARGET_STM32F4
                if (_use_rx_dma) _DMA_Channel_RX->CR |= DMA_SxCR_EN;
            #endif
            _interface->CR1 |= USART_CR1_RE;
            _buffer_rx_empty = false;
            _buffer_rx_full = false;
        }
        else _status = S_receive_already_enable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::receive_unable(){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_RE) != 0)
        {
            _interface->CR1 &= ~USART_CR1_RE;
            #ifdef TARGET_STM32L4
                _interface->ICR = USART_ICR_PECF | USART_ICR_FECF | USART_ICR_NCF | USART_ICR_ORECF | USART_ICR_IDLECF | USART_ICR_RTOCF;
                if (_use_rx_dma) _DMA_Channel_RX->CCR &= ~DMA_CCR_EN;
            #elif TARGET_STM32F4
                if (_use_rx_dma) _DMA_Channel_RX->CR &= ~DMA_SxCR_EN;
            #endif
        }
        else _status = S_receive_already_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::transmit_enable(){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_TE) == 0)
        {
            _interface->CR1 |= USART_CR1_TE;
            _buffer_tx_empty = true;
            _buffer_tx_full = false;
            _dma_transfer_run = false;
        }
        else _status = S_transmit_already_enable;
        write_data(0x00, true);
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::transmit_unable(){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_TE) != 0)
        {
            #ifdef TARGET_STM32L4
                while((_interface->ISR & USART_ISR_TC) == 0);
                _interface->ICR = USART_ICR_TCCF;
            #elif TARGET_STM32F4
                while((_interface->SR & USART_SR_TC) == 0);
                _interface->SR &= ~USART_SR_TC;
            #endif
            _interface->CR1 &= ~USART_CR1_TE;
        }
        else _status = S_transmit_already_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::is_enable(){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_UE) != 0) _status = S_USART_enable;
        else _status = S_USART_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::is_receive_enable(){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_RE) != 0) _status = S_receive_enable;
        else _status = S_receive_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::is_transmit_enable(){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_TE) != 0) _status = S_transmit_enable;
        else _status = S_transmit_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::is_data_available(){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_UE) != 0)
        {
            if ((_interface->CR1 & USART_CR1_RE) != 0)
            {
                if (_use_rx_dma == true)
                {
                    #ifdef TARGET_STM32L4
                        if (_DMA_Channel_RX->CNDTR != _size_buffer_rx)
                    #elif TARGET_STM32F4
                        //if (_DMA_Channel_RX->NDTR != _size_buffer_rx)
                        _input_pointer_rx = _size_buffer_rx - _DMA_Channel_RX->NDTR;
                        if (_input_pointer_rx != _output_pointer_rx)
                    #endif
                        {
                            _status = S_data_available;
                            _buffer_rx_empty = false;
                        }
                        else 
                        {
                            _status = S_data_unavailable;
                            _buffer_rx_empty = true;
                        }
                }
                else
                {
                    #ifdef TARGET_STM32L4
                        if ((_interface->ISR & USART_ISR_RXNE) != 0) _status = S_data_available;
                    #elif TARGET_STM32F4
                        if ((_interface->SR & USART_SR_RXNE) != 0) _status = S_data_available;
                    #endif
                        else _status = S_data_unavailable;
                }
            }
            else _status = S_receive_unable;
        }
        else _status = S_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::is_data_buffer(uint16_t *_length){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_UE) != 0)
        {
            if ((_interface->CR1 & USART_CR1_RE) != 0)
            {
                _status = is_data_available();
                if (_status == S_data_available)
                {
                    if (_use_rx_dma)
                    {
                        #ifdef TARGET_STM32L4
                            *_length = _size_buffer_rx - (uint16_t)_DMA_Channel_RX->CNDTR;
                        #elif TARGET_STM32F4
                            *_length = _size_buffer_rx - (uint16_t)_DMA_Channel_RX->NDTR;
                        #endif
                    }
                    else
                    {
                        *_length = _output_pointer_rx - _input_pointer_rx;
                        _status = S_data_available;
                    }
                }
                else _status = S_data_unavailable;
            }
            else _status = S_receive_unable;
        }
        else _status = S_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::read_data(uint8_t *_data){
    _status = S_ok;
    if (_initialized)
    {
        if (_use_rx_dma == false)
        {
            if ((_interface->CR1 & USART_CR1_UE) != 0)
            {
                if ((_interface->CR1 & USART_CR1_RE) != 0)
                {
                    #ifdef TARGET_STM32L4
                        if ((_interface->ISR & USART_ISR_RXNE) != 0) *_data = (uint8_t)_interface->RDR;
                    #elif TARGET_STM32F4
                        if ((_interface->SR & USART_SR_RXNE) != 0) *_data = (uint8_t)_interface->DR;
                    #endif
                    else _status = S_data_unavailable;
                }
                else _status = S_receive_unable;
            }
            else _status = S_unable;
        }
        else _status = S_using_rx_dma;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::write_data(uint8_t _data, bool _wait){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_UE) != 0)
        {
            if ((_interface->CR1 & USART_CR1_TE) != 0)
            {
                #ifdef TARGET_STM32L4
                    if ((_interface->ISR & USART_ISR_TXE) != 0)
                    {
                        _interface->TDR = _data;
                        _interface->ICR |= USART_ICR_TCCF;
                        if (_wait) while((_interface->ISR & USART_ISR_TC) == 0);
                    }
                #elif TARGET_STM32F4
                    if ((_interface->SR & USART_SR_TXE) != 0)
                    {
                        _interface->DR = _data;
                        _interface->SR &= ~USART_SR_TC;
                        if (_wait) while((_interface->SR & USART_SR_TC) == 0);
                    }
                #endif
                else _status = S_transmit_busy;
            }
            else _status = S_transmit_unable;
        }
        else _status = S_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::send(bool wait){
    _status = S_ok;
    uint8_t _data;

    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_UE) != 0)
        {
            if ((_interface->CR1 & USART_CR1_TE) != 0)
            {
                if (_buffer_tx_empty == false)
                {
                    #ifdef TARGET_STM32L4
                        while((_interface->ISR & USART_ISR_TC) == 0);
                        if (_use_tx_dma)
                        {
                            start_dma_transmit();
                            if (wait) while((_interface->ISR & USART_ISR_TC) == 0);
                            _buffer_tx_empty = true;
                        }
                        else
                        {
                            while (get_tx_buffer(&_data) != S_tx_buffer_already_empty)
                            {
                                
                                while ((_interface->ISR & USART_ISR_TXE) == 0);
                                _interface->TDR = _data;
                                _interface->ICR |= USART_ICR_TCCF;
                                while((_interface->ISR & USART_ISR_TC) == 0);
                            }
                        }
                    #elif TARGET_STM32F4
                        while((_interface->SR & USART_SR_TC) == 0);
                        if (_use_tx_dma)
                        {
                            start_dma_transmit();
                            if (wait) while((_interface->SR & USART_SR_TC) == 0);
                            _buffer_tx_empty = true;
                        }
                        else
                        {
                            while (get_tx_buffer(&_data) != S_tx_buffer_already_empty)
                            {
                                
                                while ((_interface->SR & USART_SR_TXE) == 0);
                                _interface->DR = _data;
                                _interface->SR &= ~USART_SR_TC;
                                while((_interface->SR & USART_SR_TC) == 0);
                            }
                        }
                    #endif
                }
                else _status = S_tx_buffer_empty;
            }
            else _status = S_transmit_unable;
        }
        else _status = S_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::send(uint16_t size, bool wait){
    _status = S_ok;
    
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_UE) != 0)
        {
            if ((_interface->CR1 & USART_CR1_TE) != 0)
            {
                if (size > 0)
                {
                    #ifdef TARGET_STM32L4
                        while((_interface->ISR & USART_ISR_TC) == 0);
                    #elif TARGET_STM32F4
                        while((_interface->SR & USART_SR_TC) == 0);
                    #endif
                    _input_pointer_tx = size;
                    _output_pointer_tx = 0;
                    _buffer_tx_empty = false;
                    send(wait);
                }
                else _status = S_tx_buffer_empty;
            }
            else _status = S_transmit_unable;
        }
        else _status = S_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::put_tx_buffer_int(uint8_t _data){
    _status = S_ok;
    if (_buffer_tx != NULL)
    {
        if (_buffer_tx_full == false)
        {
            _buffer_tx_empty = false;
            _buffer_tx[_input_pointer_tx] = _data;
            if (_input_pointer_tx == _size_buffer_tx)
            {
                _buffer_tx_full = true;
                if (_use_tx_dma) send(false);
                else send();
                _buffer_tx_full = false;
                _buffer_tx_empty = true;
                _input_pointer_tx = 0;
            }
            else _input_pointer_tx++;
        }
        else _status = S_tx_buffer_already_full;
    }
    else _status = S_tx_buffer_not_initialized;

    return(_status);
}

USART_status USART_interface::put_tx_buffer_int(uint16_t len, uint8_t *buf){
    _status = S_ok;
    while(len > 0)
    {
        len--;
        _status = put_tx_buffer_int(*buf);
        if (_status != S_ok) break;
        buf++;
    }
    return _status;
}

USART_status USART_interface::get_tx_buffer(uint8_t *_data){
    _status = S_ok;
    if (_use_tx_dma == false)
    {
        if (_buffer_tx != NULL)
        {
            if (_buffer_tx_empty == false)
            {
                _buffer_tx_full = false;
                *_data = _buffer_tx[_output_pointer_tx];
                if (_output_pointer_tx == _size_buffer_tx) _output_pointer_tx = 0;
                else _output_pointer_tx++;

                if (_output_pointer_tx == _input_pointer_tx)
                {
                    _buffer_tx_empty = true;
                    _output_pointer_tx = 0;
                    _input_pointer_tx = 0;
                    _status = S_tx_buffer_empty;
                }
            }
            else _status = S_tx_buffer_already_empty;
        }
        else _status = S_tx_buffer_not_initialized;
    }
    else _status = S_using_tx_dma;

    return(_status);
}

USART_status USART_interface::put_rx_buffer(uint8_t _data){
    _status = S_ok;
    if (_use_rx_dma == false)
    {
        if (_buffer_rx != NULL)
        {
            if (_buffer_rx_full == false)
            {
                _buffer_rx_empty = false;
                _buffer_rx[_input_pointer_rx] = _data;
                if (_input_pointer_rx == _size_buffer_rx) _input_pointer_rx = 0;
                else _input_pointer_rx++;

                if (_input_pointer_rx == _output_pointer_rx)
                {
                    _buffer_rx_full = true;
                    _status = S_rx_buffer_empty;
                }
            }
            else _status = S_rx_buffer_already_full;
        }
        else _status = S_rx_buffer_not_initialized;
    }
    else _status = S_using_rx_dma;

    return(_status);
}

USART_status USART_interface::get_rx_buffer_int(uint16_t len, uint8_t *_data){
    _status = S_ok;
    while (len > 0)
    {
        len--;
        if (_use_rx_dma) _status = get_rx_buffer_int(_data);
        else _status = read_data(_data);
        if (_status != S_ok) break;
        _data++;
    }
    return(_status);
}

USART_status USART_interface::get_rx_buffer_int(uint8_t *_data){
    _status = S_ok;
    if (_buffer_rx != NULL)
    {
        if (_buffer_rx_empty == false)
        {

            if (_use_rx_dma)
            {
                #ifdef TARGET_STM32L4
                    _input_pointer_rx = _size_buffer_rx - _DMA_Channel_RX->CNDTR;
                #elif TARGET_STM32F4
                    _input_pointer_rx = _size_buffer_rx - _DMA_Channel_RX->NDTR;
                #endif
            }

            _buffer_rx_full = false;
            *_data = _buffer_rx[_output_pointer_rx];
            if (_output_pointer_rx == _size_buffer_rx)
            {
                if (!_buffer_rx_circ && _use_rx_dma)
                {
                    _buffer_rx_empty = true;
                    _status = S_rx_buffer_empty;
                }
                _output_pointer_rx = 0;
            }
            else _output_pointer_rx++;

            #ifdef TARGET_STM32L4
                if (((_output_pointer_rx == _input_pointer_rx) && !_use_rx_dma) || ((_output_pointer_rx == (_size_buffer_rx - _DMA_Channel_RX->CNDTR)) && _use_rx_dma))
            #elif TARGET_STM32F4
                if (((_output_pointer_rx == _input_pointer_rx) && !_use_rx_dma) || ((_output_pointer_rx == (_size_buffer_rx - _DMA_Channel_RX->NDTR)) && _use_rx_dma))
            #endif
                {
                    _buffer_rx_empty = true;
                    _output_pointer_rx = 0;
                    _input_pointer_rx = 0;
                    _status = S_rx_buffer_empty;
                }

            if(_buffer_rx_empty == true)
            {
                flush_rx_buffer();
            }
        }
        else _status = S_rx_buffer_already_empty;
    }
    else _status = S_rx_buffer_not_initialized;

    return(_status);
}

USART_status USART_interface::start_dma_transmit(){
    _status = S_ok;
    if (_initialized)
    {
        if ((_interface->CR1 & USART_CR1_TE) != 0)
        {
            if (_use_tx_dma)
            {
                if (_buffer_tx_empty == false)
                {
                    #ifdef TARGET_STM32L4
                        if (is_tx_dma_free() == S_tx_dma_free)
                        {
                            _DMA_Channel_TX->CCR &= ~DMA_CCR_EN;
                            _DMA_Channel_TX->CNDTR = (_input_pointer_tx - _output_pointer_tx);
                            _input_pointer_tx = 0;
                            _output_pointer_tx = 0;
                            _buffer_tx_empty = true;
                            _dma_transfer_run = true;
                            *_DMA_TX.CISRP |= _DMA_TX.ISRB;
                            _interface->ICR |= USART_ICR_TCCF;
                            _DMA_Channel_TX->CCR |= DMA_CCR_EN;
                        }
                    #elif TARGET_STM32F4
                        if (is_tx_dma_free() == S_tx_dma_free)
                        {
                            _DMA_Channel_TX->CR &= ~DMA_SxCR_EN;
                            _DMA_Channel_TX->NDTR = (_input_pointer_tx - _output_pointer_tx);
                            _input_pointer_tx = 0;
                            _output_pointer_tx = 0;
                            _buffer_tx_empty = true;
                            _dma_transfer_run = true;
                            *_DMA_TX.CISRP |= _DMA_TX.ISRB;
                            _interface->SR &= ~USART_SR_TC;
                            _DMA_Channel_TX->CR |= DMA_SxCR_EN;
                        }
                    #endif
                }
                else _status = S_tx_buffer_empty;
            }
            else _status = S_tx_dma_unavailable;
        }
        else _status = S_transmit_unable;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::is_tx_dma_free(){
    _status = S_ok;
    if (_initialized)
    {
        #ifdef TARGET_STM32L4
            if ((_interface->ISR & USART_ISR_TC) != 0) _dma_transfer_run = false;
        #elif TARGET_STM32F4
            if ((_interface->SR & USART_SR_TC) != 0) _dma_transfer_run = false;
            else _dma_transfer_run = true;
        #endif
        if (_dma_transfer_run) _status = S_dma_transmit_process;
        else _status = S_tx_dma_free;
    }
    else _status = S_not_initialized;

    return(_status);
}

USART_status USART_interface::flush_rx_buffer(){
    _status = S_ok;
    if (_use_rx_dma)
    {
        #ifdef TARGET_STM32L4
            _DMA_Channel_RX->CCR &= ~DMA_CCR_EN;
            _DMA_Channel_RX->CNDTR = _size_buffer_rx;
            if ((_interface->CR1 & USART_CR1_RE) == USART_CR1_RE) _DMA_Channel_RX->CCR |= DMA_CCR_EN;
        #elif TARGET_STM32F4
            _DMA_Channel_RX->CR &= ~DMA_SxCR_EN;
            while((_DMA_Channel_RX->CR & DMA_SxCR_EN) != 0);
            _DMA_Channel_RX->NDTR = _size_buffer_rx;
            if ((_interface->CR1 & USART_CR1_RE) == USART_CR1_RE) 
            {
                *_DMA_RX.CISRP |= _DMA_RX.ISRB;
                _DMA_Channel_RX->CR |= DMA_SxCR_EN;
            }
        #endif
    }        
    _buffer_rx_empty = true;
    _buffer_rx_full = false;
    _output_pointer_rx = 0;
    _input_pointer_rx = 0;
    _status = S_rx_buffer_empty;
    
    return(_status);
}

USART_status USART_interface::flush_tx_buffer(){
    _status = S_ok;
    if (_use_tx_dma)
    {
        #ifdef TARGET_STM32L4
            _DMA_Channel_TX->CCR &= ~DMA_CCR_EN;
            _DMA_Channel_TX->CNDTR = _size_buffer_tx;
        #elif TARGET_STM32F4
            _DMA_Channel_TX->CR &= ~DMA_SxCR_EN;
            _DMA_Channel_TX->NDTR = _size_buffer_tx;
        #endif
    }        
    _buffer_tx_empty = true;
    _buffer_tx_full = false;
    _output_pointer_tx = 0;
    _input_pointer_tx = 0;
    _status = S_tx_buffer_empty;
    
    return(_status);
}

bool USART_interface::is_tx_buffer_empty(){
    if (_buffer_tx_empty == true) return true;
    return false;
}

uint8_t *USART_interface::get_rx_buffer_pointer(){
    return _buffer_rx;
}

uint32_t USART_interface::get_rx_buffer_dma_position()
{
    #ifdef TARGET_STM32L4
        return _DMA_Channel_RX->CNDTR;
    #elif TARGET_STM32F4
        return _DMA_Channel_RX->NDTR;
    #endif
}

uint8_t *USART_interface::get_tx_buffer_pointer(bool wait){
    #ifdef TARGET_STM32L4
        if (wait) while((_interface->ISR & USART_ISR_TC) == 0);
    #elif TARGET_STM32F4
        if (wait) while((_interface->SR & USART_SR_TC) == 0);
    #endif 
    return _buffer_tx;
}

uint32_t USART_interface::get_tx_buffer_dma_position()
{
    #ifdef TARGET_STM32L4
        return _DMA_Channel_TX->CNDTR;
    #elif TARGET_STM32F4
        return _DMA_Channel_TX->NDTR;
    #endif
}

USART_status USART_interface::printf(const char * format, ...){
    std::va_list arg;
    _status = S_ok;
    if (_buffer_tx != NULL)
    {
        va_start(arg, format);
        #ifdef TARGET_STM32L4
            while (((_interface->ISR & USART_ISR_TC) == 0));
        #elif TARGET_STM32F4
            while((_interface->SR & USART_SR_TC) == 0);
        #endif
        flush_tx_buffer();
        _input_pointer_tx = vsnprintf((char *)_buffer_tx, _size_buffer_tx, format, arg);
        va_end(arg);
        if (_input_pointer_tx > 0) {
            _buffer_tx_empty = false;
            send();
            _buffer_tx_empty = true;
        }
        else _status = S_tx_buffer_empty;
    }
    else _status = S_tx_buffer_not_initialized;

    return _status;
}

USART_status USART_interface::printf(bool wait, const char * format, ...){
    std::va_list arg;
    _status = S_ok;
    if (_buffer_tx != NULL)
    {
        va_start(arg, format);
        #ifdef TARGET_STM32L4
            while (((_interface->ISR & USART_ISR_TC) == 0));
        #elif TARGET_STM32F4
            while (((_interface->SR & USART_SR_TC) == 0));
        #endif
        flush_tx_buffer();
        _input_pointer_tx = vsnprintf((char *)_buffer_tx, _size_buffer_tx, format, arg);
        va_end(arg);
        if (_input_pointer_tx > 0) {
            _buffer_tx_empty = false;
            send(wait);
            _buffer_tx_empty = true;
        }
        else _status = S_tx_buffer_empty;
    }
    else _status = S_tx_buffer_not_initialized;

    return _status;
}

USART_status USART_interface::bprintf(const char * format, ...){
    std::va_list arg;
    uint16_t temp_len = _input_pointer_tx;
    _status = S_ok;
    if (_buffer_tx != NULL)
    {
        va_start(arg, format);
        #ifdef TARGET_STM32L4
            while (((_interface->ISR & USART_ISR_TC) == 0));
        #elif TARGET_STM32F4
            while (((_interface->SR & USART_SR_TC) == 0));
        #endif
        _input_pointer_tx += vsnprintf((char *)_buffer_tx, _size_buffer_tx - temp_len, format, arg);
        va_end(arg);
        if (_input_pointer_tx >= _size_buffer_tx)
        {
            _buffer_tx_empty = false;
            send();
            _buffer_tx_empty = true;
        }
        else if (_input_pointer_tx > 0) _buffer_tx_empty = false;
        else _status = S_tx_buffer_empty;
    }
    else _status = S_tx_buffer_not_initialized;

    return _status;
}

USART_status USART_interface::bprintf(bool wait, const char * format, ...){
    std::va_list arg;
    uint16_t temp_len = _input_pointer_tx;
    _status = S_ok;
    if (_buffer_tx != NULL)
    {
        va_start(arg, format);
        #ifdef TARGET_STM32L4
            while (((_interface->ISR & USART_ISR_TC) == 0));
        #elif TARGET_STM32F4
            while (((_interface->SR & USART_SR_TC) == 0));
        #endif
        _input_pointer_tx += vsnprintf((char *)_buffer_tx, _size_buffer_tx - temp_len, format, arg);
        va_end(arg);
        if (_input_pointer_tx >= _size_buffer_tx)
        {
            _buffer_tx_empty = false;
            send(wait);
            _buffer_tx_empty = true;
        }
        else if (_input_pointer_tx > 0) _buffer_tx_empty = false;
        else _status = S_tx_buffer_empty;
    }
    else _status = S_tx_buffer_not_initialized;

    return _status;
}

USART_status USART_interface::set_end_caracter(uint8_t end_caracter, bool use_escape, uint8_t escape_caracter)
{
    _status = S_ok;
    _end_caracter = end_caracter;
    _use_end = true;
    if (use_escape)
    {
        _use_escape = true;
        _escape_caracter = escape_caracter;
    }
    else _use_escape = false;

    _received_end = false;
    _received_escape = false;

    return _status;
}

USART_status USART_interface::enable_rx_interrupt(usart_callback_rx _callback)
{
    _status = S_ok;
    //fazer a ativação e registro da ISR
    if (_initialized)
    {
        if (_use_rx)
        {
            if (_use_rx_dma)
            {
                #ifdef TARGET_STM32L4
                    *_DMA_RX.CISRP |= _DMA_RX.ISRB;
                    NVIC_SetVector((IRQn_Type)_DMA_RX.IRQn, _DMA_RX.IRQ_address);
                    NVIC_ClearPendingIRQ((IRQn_Type)_DMA_RX.IRQn);
                    NVIC_EnableIRQ((IRQn_Type)_DMA_RX.IRQn);
                    _DMA_Channel_RX->CCR |= DMA_CCR_TCIE;
                #elif TARGET_STM32F4
                    *_DMA_RX.CISRP |= _DMA_RX.ISRB;
                    NVIC_SetVector((IRQn_Type)_DMA_RX.IRQn, _DMA_RX.IRQ_address);
                    NVIC_ClearPendingIRQ((IRQn_Type)_DMA_RX.IRQn);
                    NVIC_EnableIRQ((IRQn_Type)_DMA_RX.IRQn);
                    _DMA_Channel_RX->CR |= DMA_SxCR_TCIE;
                #endif
            }
            else
            {
                _use_rx_IRQ = true;
                #ifdef TARGET_STM32L4
                    _interface->RQR |= USART_RQR_RXFRQ;
                    NVIC_SetVector((IRQn_Type)_IRQn, _IRQ_address);
                    NVIC_ClearPendingIRQ((IRQn_Type)_IRQn);
                    NVIC_EnableIRQ((IRQn_Type)_IRQn);
                    _interface->CR1 |= USART_CR1_RXNEIE;
                    _ISR_mask |= USART_ISR_RXNE;
                #elif TARGET_STM32F4
                    _interface->SR &= ~USART_SR_RXNE;
                    NVIC_SetVector((IRQn_Type)_IRQn, _IRQ_address);
                    NVIC_ClearPendingIRQ((IRQn_Type)_IRQn);
                    NVIC_EnableIRQ((IRQn_Type)_IRQn);
                    _interface->CR1 |= USART_CR1_RXNEIE;
                    _ISR_mask |= USART_SR_RXNE;
                #endif
            }
            callback_rx = _callback;
        }
        else _status = S_fail_RX_unavailable;
    }
    else _status = S_not_initialized;

    return _status;
}

USART_status USART_interface::unable_rx_interrupt(void)
{
    _status = S_ok;
    //fazer a desativação
    if (_initialized)
    {
        if (_use_rx)
        {
            if (_use_rx_dma)
            {
                #ifdef TARGET_STM32L4
                    _DMA_Channel_RX->CCR &= ~DMA_CCR_TCIE;
                    NVIC_DisableIRQ((IRQn_Type)_DMA_RX.IRQn);
                    *_DMA_RX.CISRP |= _DMA_RX.ISRB;
                    NVIC_ClearPendingIRQ((IRQn_Type)_DMA_RX.IRQn);
                #elif TARGET_STM32F4
                    _DMA_Channel_RX->CR &= ~DMA_SxCR_TCIE;
                    NVIC_DisableIRQ((IRQn_Type)_DMA_RX.IRQn);
                    *_DMA_RX.CISRP |= _DMA_RX.ISRB;
                    NVIC_ClearPendingIRQ((IRQn_Type)_DMA_RX.IRQn);
                #endif
            }
            else
            {
                _use_rx_IRQ = false;
                #ifdef TARGET_STM32L4
                    _interface->CR1 &= ~USART_CR1_RXNEIE;
                    _interface->RQR |= USART_RQR_RXFRQ;
                    _ISR_mask &= ~USART_ISR_RXNE;
                #elif TARGET_STM32F4
                    _interface->CR1 &= ~USART_CR1_RXNEIE;
                    _interface->SR &= ~USART_SR_RXNE;
                    _ISR_mask &= ~USART_SR_RXNE;
                #endif
                if (!_use_tx_IRQ)
                {
                    NVIC_DisableIRQ((IRQn_Type)_IRQn);
                    NVIC_ClearPendingIRQ((IRQn_Type)_IRQn);
                }
            }
            callback_rx = NULL;
        }
        else _status = S_fail_RX_unavailable;
    }
    else _status = S_not_initialized;

    return _status;
}

USART_status USART_interface::enable_tx_interrupt(usart_callback_tx _callback)
{
    _status = S_ok;
    //fazer a ativação e registro da ISR
    if (_initialized)
    {
        if (_use_tx)
        {
            if (_use_tx_dma)
            {
                #ifdef TARGET_STM32L4
                    *_DMA_TX.CISRP |= _DMA_TX.ISRB;
                    NVIC_SetVector((IRQn_Type)_DMA_TX.IRQn, _DMA_TX.IRQ_address);
                    NVIC_ClearPendingIRQ((IRQn_Type)_DMA_TX.IRQn);
                    NVIC_EnableIRQ((IRQn_Type)_DMA_TX.IRQn);
                    _DMA_Channel_TX->CCR |= DMA_CCR_TCIE;
                #elif TARGET_STM32F4
                    *_DMA_TX.CISRP |= _DMA_TX.ISRB;
                    NVIC_SetVector((IRQn_Type)_DMA_TX.IRQn, _DMA_TX.IRQ_address);
                    NVIC_ClearPendingIRQ((IRQn_Type)_DMA_TX.IRQn);
                    NVIC_EnableIRQ((IRQn_Type)_DMA_TX.IRQn);
                    _DMA_Channel_TX->CR |= DMA_SxCR_TCIE;
                #endif
            }
            else
            {
                _use_tx_IRQ = true;
                #ifdef TARGET_STM32L4
                    _interface->ICR |= USART_ICR_TCCF;
                    NVIC_SetVector((IRQn_Type)_IRQn, _IRQ_address);
                    NVIC_ClearPendingIRQ((IRQn_Type)_IRQn);
                    NVIC_EnableIRQ((IRQn_Type)_IRQn);
                    _interface->CR1 |= USART_CR1_TCIE;
                    _ISR_mask |= USART_ISR_TC;
                #elif TARGET_STM32F4
                    _interface->SR &= ~USART_SR_TC;
                    NVIC_SetVector((IRQn_Type)_IRQn, _IRQ_address);
                    NVIC_ClearPendingIRQ((IRQn_Type)_IRQn);
                    NVIC_EnableIRQ((IRQn_Type)_IRQn);
                    _interface->CR1 |= USART_CR1_TCIE;
                    _ISR_mask |= USART_SR_TC;
                #endif
            }
            callback_tx = _callback;
        }
        else _status = S_fail_TX_unavailable;
    }
    else _status = S_not_initialized;

    return _status;
}

USART_status USART_interface::unable_tx_interrupt(void)
{
    _status = S_ok;
    //fazer a desativação
    if (_initialized)
    {
        if (_use_tx)
        {
            if (_use_tx_dma)
            {
                #ifdef TARGET_STM32L4
                    _DMA_Channel_TX->CCR &= ~DMA_CCR_TCIE;
                    NVIC_DisableIRQ((IRQn_Type)_DMA_TX.IRQn);
                    *_DMA_TX.CISRP |= _DMA_TX.ISRB;
                    NVIC_ClearPendingIRQ((IRQn_Type)_DMA_TX.IRQn);
                #elif TARGET_STM32F4
                    _DMA_Channel_TX->CR &= ~DMA_SxCR_TCIE;
                    NVIC_DisableIRQ((IRQn_Type)_DMA_TX.IRQn);
                    *_DMA_TX.CISRP |= _DMA_TX.ISRB;
                    NVIC_ClearPendingIRQ((IRQn_Type)_DMA_TX.IRQn);
                #endif
            }
            else
            {
                _use_tx_IRQ = false;
                #ifdef TARGET_STM32L4
                    _interface->CR1 &= ~USART_CR1_TCIE;
                    _interface->ICR |= USART_ICR_TCCF;
                    _ISR_mask &= ~USART_ISR_TC;
                #elif TARGET_STM32F4
                    _interface->CR1 &= ~USART_CR1_TCIE;
                    _interface->SR &= ~USART_SR_TC;
                    _ISR_mask &= ~USART_SR_TC;
                #endif
                if (!_use_rx_IRQ)
                {
                    NVIC_DisableIRQ((IRQn_Type)_IRQn);
                    NVIC_ClearPendingIRQ((IRQn_Type)_IRQn);
                }
            }
            callback_tx = NULL;
        }
        else _status = S_fail_TX_unavailable;
    }
    else _status = S_not_initialized;

    return _status;
}

void USART_interface::interrupt_usart_tx_rx(void)
{
    #ifdef TARGET_STM32L4
        while ((_interface->ISR & _ISR_mask) != 0)
        {
            if ((_interface->ISR & USART_ISR_TC) != 0 and _use_tx_IRQ)
            {
                if (callback_tx != NULL) callback_tx(this);
                _interface->ICR |= USART_ICR_TCCF;
            }
            if ((_interface->ISR & USART_ISR_RXNE) != 0 and _use_rx_IRQ)
            {
                _last_received = (uint8_t)_interface->RDR;
                if (_buffer_rx != NULL) put_rx_buffer(_last_received);
                else
                {
                    _received_end = false;
                    if (_use_end and not _received_escape) _received_end = (_last_received == _end_caracter ? true : false);
                    if (_use_escape)
                    {
                        if (_received_escape) _received_escape = false;
                        else _received_escape = (_last_received == _escape_caracter ? true : false);
                    }
                }
                if (callback_rx != NULL and ((_use_end and _received_end) or not _use_end)) callback_rx(this, _last_received, _output_pointer_rx - _input_pointer_rx);
                _interface->RQR |= USART_RQR_RXFRQ;
            }
        }
    #elif TARGET_STM32F4
        while ((_interface->SR & _ISR_mask) != 0)
        {
            if ((_interface->SR & USART_SR_TC) != 0 and _use_tx_IRQ)
            {
                if (callback_tx != NULL) callback_tx(this);
                _interface->SR &= ~USART_SR_TC;
            }
            if ((_interface->SR & USART_SR_RXNE) != 0 and _use_rx_IRQ)
            {
                _last_received = (uint8_t)_interface->DR;
                if (_buffer_rx != NULL) put_rx_buffer(_last_received);
                else
                {
                    _received_end = false;
                    if (_use_end and not _received_escape) _received_end = (_last_received == _end_caracter ? true : false);
                    if (_use_escape)
                    {
                        if (_received_escape) _received_escape = false;
                        else _received_escape = (_last_received == _escape_caracter ? true : false);
                    }
                }
                if (callback_rx != NULL and ((_use_end and _received_end) or not _use_end)) callback_rx(this, _last_received, _output_pointer_rx - _input_pointer_rx);
                _interface->SR &= ~USART_SR_RXNE;
            }
        }
    #endif
    NVIC_ClearPendingIRQ((IRQn_Type)_IRQn);
}

void USART_interface::interrupt_dma_rx(void)
{
    #ifdef TARGET_STM32L4
        _last_received = (uint8_t)_interface->RDR;
    #elif TARGET_STM32F4
        _last_received = (uint8_t)_interface->DR;
    #endif
    if (callback_rx != NULL) callback_rx(this, _last_received, _size_buffer_rx);
    *_DMA_RX.CISRP |= _DMA_RX.ISRB;
    NVIC_ClearPendingIRQ((IRQn_Type)_DMA_RX.IRQn);
}

void USART_interface::interrupt_dma_tx(void)
{
    if (callback_tx != NULL) callback_tx(this);
    *_DMA_TX.CISRP |= _DMA_TX.ISRB;
    NVIC_ClearPendingIRQ((IRQn_Type)_DMA_TX.IRQn);
}
