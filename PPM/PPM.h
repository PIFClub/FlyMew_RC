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
 *	Module		: PPM.h
 *	Description	: Header of PPM Module
 *  Version     : 1.0
 *
 *	Author		: FW.Team
 *	Notes		:
 *
 *****************************************************************************/

#ifndef PPM_PPM_H_
#define PPM_PPM_H_

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

#ifdef TXBOARD
extern uint16_t u16_channel[];
#else

extern uint16_t u16_channel[];
extern uint16_t u16_rf_channel[];

#endif

/******************************************************************************
 * PROTOTYPES
 *****************************************************************************/
#ifdef TXBOARD

void v_Config_Mode_Selection(void);

void v_Read_Mode(void);

#else

__interrupt void Timer_A1_ISR(void);

void v_Config_Timer_RX(void);

#endif

#endif /* PPM_PPM_H_ */

/******************************************************************************
 * END OF PPM.h
 *****************************************************************************/
