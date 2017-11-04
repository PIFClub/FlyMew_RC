/******************************************************************************
 *
 * www.raiseyourarm.com
 * www.payitforward.edu.vn
 *
 *****************************************************************************/
/******************************************************************************
 * Raise Your Arm 2017
 *
 *****************************************************************************/

/******************************************************************************
 *
 *	Module		: ADC.c
 *	Description	: ADC
 *  Version     : 1.0
 *
 *	Author		: FW.Team
 *	Notes		:
 *
 *****************************************************************************/

/******************************************************************************
 * IMPORT
 *****************************************************************************/
#include "ADC.h"


/******************************************************************************
 * FUNCTIONS
 *****************************************************************************/
#ifdef TXBOARD
uint16_t au16_adc_result[7];
static uint16_t au16_adc_base[7] = {0, 0, 0, 0, 0, 0, 0};
extern ENM_ADC_STATE_T enm_adc_state;

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    static uint8_t u8_calib_cnt = 0;
    uint8_t u8_idx;

    ADC10CTL0 &= ~ENC;

    /* Normalize ADC value */
    for (u8_idx = 0; u8_idx < 7; u8_idx++)
    {
        if (au16_adc_result[u8_idx] >= 12)
        {
            au16_adc_result[u8_idx] -= 12;
        }
        else
        {
            au16_adc_result[u8_idx] = 0;
        }

        if (au16_adc_result[u8_idx] > 1000)
            au16_adc_result[u8_idx] = 1000;
    }

    switch (enm_adc_state)
    {
    case ENM_ADC_STATE_CALIB:
        u8_calib_cnt++;
        if (u8_calib_cnt >= 100)
        {
            enm_adc_state = ENM_ADC_STATE_RUN;
        }

        for (u8_idx = 0; u8_idx < 7; u8_idx++)
        {
            if (au16_adc_base[u8_idx] == 0)
            {
                au16_adc_base[u8_idx] = au16_adc_result[u8_idx];
            }
            else
            {
                au16_adc_base[u8_idx] = ((au16_adc_base[u8_idx] << 2) +  au16_adc_result[u8_idx]) / 5;
            }
        }

        break;
    case ENM_ADC_STATE_RUN:
        if (ADC_CH1_REVERSE)
        {
            if (au16_adc_result[ADC_CH1_IDX] > au16_adc_base[ADC_CH1_IDX])
            {
                u16_channel[0] = 1500 - (au16_adc_result[ADC_CH1_IDX] - au16_adc_base[ADC_CH1_IDX]);
            }
            else
            {
                u16_channel[0] = 1500 + (au16_adc_base[ADC_CH1_IDX] - au16_adc_result[ADC_CH1_IDX]);
            }
        }
        else
        {
            if (au16_adc_result[ADC_CH1_IDX] > au16_adc_base[ADC_CH1_IDX])
            {
                u16_channel[0] = 1500 + (au16_adc_result[ADC_CH1_IDX] - au16_adc_base[ADC_CH1_IDX]);
            }
            else
            {
                u16_channel[0] = 1500 - (au16_adc_base[ADC_CH1_IDX] - au16_adc_result[ADC_CH1_IDX]);
            }
        }

        if (ADC_CH2_REVERSE)
        {
            if (au16_adc_result[ADC_CH2_IDX] > au16_adc_base[ADC_CH2_IDX])
            {
                u16_channel[1] = 1500 - (au16_adc_result[ADC_CH2_IDX] - au16_adc_base[ADC_CH2_IDX]);
            }
            else
            {
                u16_channel[1] = 1500 + (au16_adc_base[ADC_CH2_IDX] - au16_adc_result[ADC_CH2_IDX]);
            }
        }
        else
        {
            if (au16_adc_result[ADC_CH2_IDX] > au16_adc_base[ADC_CH2_IDX])
            {
                u16_channel[1] = 1500 + (au16_adc_result[ADC_CH2_IDX] - au16_adc_base[ADC_CH2_IDX]);
            }
            else
            {
                u16_channel[1] = 1500 - (au16_adc_base[ADC_CH2_IDX] - au16_adc_result[ADC_CH2_IDX]);
            }
        }

        if (ADC_CH3_REVERSE)
        {
            if (au16_adc_result[ADC_CH3_IDX] > au16_adc_base[ADC_CH3_IDX])
            {
                u16_channel[2] = 1500 - (au16_adc_result[ADC_CH3_IDX] - au16_adc_base[ADC_CH3_IDX]);
            }
            else
            {
                u16_channel[2] = 1500 + (au16_adc_base[ADC_CH3_IDX] - au16_adc_result[ADC_CH3_IDX]);
            }
        }
        else
        {
            if (au16_adc_result[ADC_CH3_IDX] > au16_adc_base[ADC_CH3_IDX])
            {
                u16_channel[2] = 1500 + (au16_adc_result[ADC_CH3_IDX] - au16_adc_base[ADC_CH3_IDX]);
            }
            else
            {
                u16_channel[2] = 1500 - (au16_adc_base[ADC_CH3_IDX] - au16_adc_result[ADC_CH3_IDX]);
            }
        }

        if (ADC_CH4_REVERSE)
        {
            if (au16_adc_result[ADC_CH4_IDX] > au16_adc_base[ADC_CH4_IDX])
            {
                u16_channel[3] = 1500 - (au16_adc_result[ADC_CH4_IDX] - au16_adc_base[ADC_CH4_IDX]);
            }
            else
            {
                u16_channel[3] = 1500 + (au16_adc_base[ADC_CH4_IDX] - au16_adc_result[ADC_CH4_IDX]);
            }
        }
        else
        {
            if (au16_adc_result[ADC_CH4_IDX] > au16_adc_base[ADC_CH4_IDX])
            {
                u16_channel[3] = 1500 + (au16_adc_result[ADC_CH4_IDX] - au16_adc_base[ADC_CH4_IDX]);
            }
            else
            {
                u16_channel[3] = 1500 - (au16_adc_base[ADC_CH4_IDX] - au16_adc_result[ADC_CH4_IDX]);
            }
        }

        break;
    default:
        break;
    }

    while (ADC10CTL1 & ADC10BUSY);               // Wait if ADC10 core is active
    ADC10SA = (uint16_t)au16_adc_result;         // Data buffer start
    ADC10CTL0 |= ENC + ADC10SC;             	 // Sampling and conversion ready
}

void v_Config_ADC(void)
{
    ADC10CTL1 = INCH_6 + CONSEQ_3;          					   // A1/A0, multi channel
    ADC10CTL0 = ADC10SHT_3 + MSC + ADC10ON + ADC10IE;
    ADC10AE0 = ADC_ANALOG_INPUT_0 | ADC_ANALOG_INPUT_3
    		   | ADC_ANALOG_INPUT_5 | ADC_ANALOG_INPUT_6;          // P1.0,1 ADC option select
    ADC10DTC1 = 7;                         		 				   // 1 conversions
    ADC10CTL0 &= ~ENC;

    while (ADC10CTL1 & ADC10BUSY);               				   // Wait if ADC10 core is active
    ADC10SA = (uint16_t)au16_adc_result;        				   // Data buffer start
    ADC10CTL0 |= ENC + ADC10SC;             	 				   // Sampling and conversion ready
}

uint16_t u16_Throttle_Get_Value(uint16_t u16_adc_val)
{
    static uint16_t u16_throttle = 0;
    static uint8_t u8_pre_scale = 0;
    static uint16_t u16_adc_filt = 0;

    u16_adc_filt = ((u16_adc_filt << 2) + u16_adc_val) / 5;

    u8_pre_scale++;
    if (u8_pre_scale < 16)
    {
    }
    else
    {
        u8_pre_scale = 0;
        if ((u16_adc_filt < 550) && (u16_adc_filt > 470))
        {
            //Do nothing, the throttle stick is at middle position
        }
        else if (u16_adc_filt <= 470)
        {
            if (u16_throttle > ARM_THROTTLE_THRESHOLD)
            {
                u16_throttle--;
            }
            else
            {
                if (u16_adc_filt < ARM_THROTTLE_THRESHOLD)
                {
                    if (u16_throttle > 0)
                    {
                        u16_throttle--;
                    }
                }
            }
        }
        else    //u16_adc_val >= 550
        {
            u16_throttle++;
            if (u16_throttle >= 1000)
            {
                u16_throttle = 1000;
            }
        }
    }

    return u16_throttle;
}
#else
#endif
/******************************************************************************
 * END OF ADC.c
 *****************************************************************************/
