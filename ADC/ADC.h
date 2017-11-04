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
 *	Module		: ADC.h
 *	Description	: Header of ADC Module
 *  Version     : 1.0
 *
 *	Author		: FW.Team
 *	Notes		:
 *
 *****************************************************************************/

#ifndef ADC_ADC_H_
#define ADC_ADC_H_

/******************************************************************************
 * IMPORT
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "msp430.h"
#include "stdio.h"
#include "radio_drv.h"
#include "cc1x_utils.h"
#include "hal_spi_rf.h"
#include "../bsp.h"

/******************************************************************************
 * DEFINITIONS
 *****************************************************************************/
#define ADC_CH1_IDX     3
#define ADC_CH2_IDX     6
#define ADC_CH3_IDX     1
#define ADC_CH4_IDX     0

#define ADC_CH1_REVERSE     0
#define ADC_CH2_REVERSE     1
#define ADC_CH3_REVERSE     1
#define ADC_CH4_REVERSE     1

#define ARM_THROTTLE_THRESHOLD  100

#ifdef TXBOARD
extern uint16_t u16_count, u16_channel[];
#else
#endif


typedef enum
{
    ENM_ADC_STATE_INIT = 0,
    ENM_ADC_STATE_CALIB,
    ENM_ADC_STATE_RUN
} ENM_ADC_STATE_T;

/******************************************************************************
 * PROTOTYPES
 *****************************************************************************/

#ifdef TXBOARD

__interrupt void ADC10_ISR (void);

//Configure ADC
void v_Config_ADC(void);
//
uint16_t u16_Throttle_Get_Value(uint16_t u16_adc_val);

#else
#endif




#endif /* ADC_ADC_H_ */

/******************************************************************************
 * END OF ADC.h
 *****************************************************************************/
