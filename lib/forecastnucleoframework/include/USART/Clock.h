//version 2.0 -> 21/01/2021

#ifndef CLOCK_H
#define CLOCK_H

// ignoring the unused-functions warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function" 

// ignoring unsused variables
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable" 

    #ifdef TARGET_STM32L4
        #include "stm32l4xx.h"
    #elif TARGET_STM32F4
        #include "stm32f4xx.h"
    #endif

    #ifndef HSE_CLOCK
        #define HSE_CLOCK 0
    #endif

    #ifdef TARGET_STM32L4
        static float get_msi_range(){
            switch (RCC->CR & RCC_CR_MSIRANGE)
            {
                case RCC_CR_MSIRANGE_0:
                    return 100000.0f;
                
                case RCC_CR_MSIRANGE_1:
                    return 200000.0f;
                
                case RCC_CR_MSIRANGE_2:
                    return 400000.0f;
                
                case RCC_CR_MSIRANGE_3:
                    return 800000.0f;
                
                case RCC_CR_MSIRANGE_4:
                    return 1000000.0f;
                
                case RCC_CR_MSIRANGE_5:
                    return 2000000.0f;
                
                case RCC_CR_MSIRANGE_6:
                    return 4000000.0f;
                
                case RCC_CR_MSIRANGE_7:
                    return 8000000.0f;
                
                case RCC_CR_MSIRANGE_8:
                    return 16000000.0f;
                
                case RCC_CR_MSIRANGE_9:
                    return 24000000.0f;
                
                case RCC_CR_MSIRANGE_10:
                    return 32000000.0f;
                
                case RCC_CR_MSIRANGE_11:
                    return 48000000.0f;
                
                default:
                    return 0.0f;
            }
        }
    #endif

    static float get_pll_source_clock(){
        switch (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC)
        {
            #ifdef TARGET_STM32L4
                case RCC_PLLCFGR_PLLSRC_MSI:
                    return get_msi_range();
            #endif

            case RCC_PLLCFGR_PLLSRC_HSI:
                return 16000000.0f;

            case RCC_PLLCFGR_PLLSRC_HSE:
                return (float)HSE_CLOCK;

            default:
                return 0.0f;
        }
    }

    static float get_m(){
        #ifdef TARGET_STM32L4
            return ((float)(((RCC->PLLCFGR & RCC_PLLCFGR_PLLM)>>RCC_PLLCFGR_PLLM_Pos)+1));
        #endif
        #ifdef TARGET_STM32F4
            return ((float)((RCC->PLLCFGR & RCC_PLLCFGR_PLLM)>>RCC_PLLCFGR_PLLM_Pos));
        #endif
    }

    static float get_n(){
        return ((float)((RCC->PLLCFGR & RCC_PLLCFGR_PLLN)>>RCC_PLLCFGR_PLLN_Pos));
    }

    static float get_p(){
        #ifdef TARGET_STM32L4
            if ((RCC->PLLCFGR & RCC_PLLCFGR_PLLPDIV) == 0)
            {
                if ((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) == RCC_PLLCFGR_PLLP) return 17.0f;
                else return 7.0f;
            }
            else return ((float)((RCC->PLLCFGR & RCC_PLLCFGR_PLLPDIV)>>RCC_PLLCFGR_PLLPDIV_Pos));
        #endif
        #ifdef TARGET_STM32F4
            return ((float)(((RCC->PLLCFGR & RCC_PLLCFGR_PLLP)>>RCC_PLLCFGR_PLLP_Pos)*2+2));
        #endif
    }

    static float get_q(){
        #ifdef TARGET_STM32L4
            return ((float)(((RCC->PLLCFGR & RCC_PLLCFGR_PLLQ)>>RCC_PLLCFGR_PLLQ_Pos)*2+2));
        #endif
        #ifdef TARGET_STM32F4
            return ((float)((RCC->PLLCFGR & RCC_PLLCFGR_PLLQ)>>RCC_PLLCFGR_PLLQ_Pos));
        #endif
    }

    static float get_r(){
        #ifdef TARGET_STM32L4
            return ((float)(((RCC->PLLCFGR & RCC_PLLCFGR_PLLR)>>RCC_PLLCFGR_PLLR_Pos)*2+2));
        #endif
        #ifdef TARGET_STM32F4
            return ((float)((RCC->PLLCFGR & RCC_PLLCFGR_PLLR)>>RCC_PLLCFGR_PLLR_Pos));
        #endif
    }

    static float get_pllclk(){
        return ((get_pll_source_clock()*(get_n()/get_m()))/get_p());
    }

    static float get_pllq(){
        return ((get_pll_source_clock()*(get_n()/get_m()))/get_q());
    }

    static float get_pllr(){
        return ((get_pll_source_clock()*(get_n()/get_m()))/get_r());
    }

    static float get_pllsaip(){
        //M3 /P
        return 0.0f;
    }

    static float get_pllsaiq(){
        //M3 /Q DIV
        return 0.0f;
    }

    static float get_plli2sp(){
        //M2 /P
        return 0.0f;
    }

    static float get_plli2s_saiclk(){
        //M2 /Q DIV
        return 0.0f;
    }

    static float get_plli2sr(){
        //M2 /R
        return 0.0f;
    }

    static float get_system_clock(){
        switch(RCC->CFGR & RCC_CFGR_SWS)
        {
            case RCC_CFGR_SWS_HSI:
                return 16000000.0f;
            
            case RCC_CFGR_SWS_HSE:
                return (float)HSE_CLOCK;
            
            #ifdef TARGET_STM32L4
                case RCC_CFGR_SWS_PLL:
                    return get_pllr();
            #endif
            
            #ifdef TARGET_STM32F4
                case RCC_CFGR_SWS_PLL:
                    return get_pllclk();
                    
                case RCC_CFGR_SWS_PLLR:
                    return get_pllr();
            #endif

            default:
                return 0.0f;
        }
    }

    static float get_ahb_clock(){
        float P = 0.0f;
        switch(RCC->CFGR & RCC_CFGR_HPRE)
        {
            case RCC_CFGR_HPRE_DIV1:
                P = 1.0;
                break;

            case RCC_CFGR_HPRE_DIV2:
                P = 2.0;
                break;

            case RCC_CFGR_HPRE_DIV4:
                P = 4.0;
                break;
            
            case RCC_CFGR_HPRE_DIV8:
                P = 8.0;
                break;
            
            case RCC_CFGR_HPRE_DIV16:
                P = 16.0;
                break;
            
            case RCC_CFGR_HPRE_DIV64:
                P = 64.0;
                break;
            
            case RCC_CFGR_HPRE_DIV128:
                P = 128.0;
                break;
            
            case RCC_CFGR_HPRE_DIV256:
                P = 256.0;
                break;
            
            case RCC_CFGR_HPRE_DIV512:
                P = 512.0;
                break;
            
            default:
                P = 1.0;
        }
        return (get_system_clock()/P);
    }

    static float get_apb1_clock(){
        float P1 = 0.0f;
        switch(RCC->CFGR & RCC_CFGR_PPRE1)
        {
            case RCC_CFGR_PPRE1_DIV1:
                P1 = 1.0;
                break;

            case RCC_CFGR_PPRE1_DIV2:
                P1 = 2.0;
                break;

            case RCC_CFGR_PPRE1_DIV4:
                P1 = 4.0;
                break;
            
            case RCC_CFGR_PPRE1_DIV8:
                P1 = 8.0;
                break;
            
            case RCC_CFGR_PPRE1_DIV16:
                P1 = 16.0;
                break;
            
            default:
                P1 = 1.0;
        }
        return (get_ahb_clock()/P1);
    }

    static float get_apb2_clock(){
        float P2 = 0.0f;
        switch(RCC->CFGR & RCC_CFGR_PPRE2)
        {
            case RCC_CFGR_PPRE2_DIV1:
                P2 = 1.0;
                break;

            case RCC_CFGR_PPRE2_DIV2:
                P2 = 2.0;
                break;

            case RCC_CFGR_PPRE2_DIV4:
                P2 = 4.0;
                break;
            
            case RCC_CFGR_PPRE2_DIV8:
                P2 = 8.0;
                break;
            
            case RCC_CFGR_PPRE2_DIV16:
                P2 = 16.0;
                break;
            
            default:
                P2 = 1.0;
        }
        return (get_ahb_clock()/P2);
    }

    static uint32_t get_usart_clock(USART_TypeDef *usart)
    {
        #ifdef TARGET_STM32L4
            if (usart == USART1)
            {
                switch(RCC->CCIPR & RCC_CCIPR_USART1SEL)
                {
                    case RCC_CCIPR_USART1SEL:
                        return LSE_VALUE;
                        break;
                    
                    case RCC_CCIPR_USART1SEL_1:
                        return HSI_VALUE;
                    
                    case RCC_CCIPR_USART1SEL_0:
                        return (uint32_t)get_system_clock();
                    
                    default:
                        return (uint32_t)get_apb2_clock();
                }
            }
            else if (usart == USART2)
            {
                switch(RCC->CCIPR & RCC_CCIPR_USART2SEL)
                {
                    case RCC_CCIPR_USART2SEL:
                        return LSE_VALUE;
                        break;
                    
                    case RCC_CCIPR_USART2SEL_1:
                        return HSI_VALUE;
                    
                    case RCC_CCIPR_USART2SEL_0:
                        return (uint32_t)get_system_clock();
                    
                    default:
                        return (uint32_t)get_apb1_clock();
                }
            }
            return (0.0f);
        #elif TARGET_STM32F4
            if (usart == USART1) return (uint32_t)get_apb2_clock();
            else if (usart == USART2) return (uint32_t)get_apb1_clock();
            else if (usart == USART3) return (uint32_t)get_apb1_clock();
            else if (usart == USART6) return (uint32_t)get_apb2_clock();
            return (0.0f);
        #endif
    }


// ignore unsused var
#pragma GCC diagnostic pop 

// ignore unused func
#pragma GCC diagnostic pop 

#endif //CLOCK_H