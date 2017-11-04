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
 *	Module		: FlyMew_main.c
 *	Description	: Main for FLyMew platform
 *  Version     : 1.0
 *
 *	Author		: FW.Team
 *	Notes		:
 *
 *****************************************************************************/

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
#include "ADC.h"
#include "PPM.h"
#include "FlyMew_rc.h"
#include "bsp.h"

/******************************************************************************
 * LOCAL FUNCTIONS
 *****************************************************************************/
extern void msp_setup(void);
extern unsigned long volatile time_counter;

void v_ADC_Start_Conversion(void);

/******************************************************************************
 * GLOBALS VARIABLE
 *****************************************************************************/
unsigned char txBuffer[TX_BUFF_SIZE];
trx_cfg_struct trx_cfg;

/******************************************************************************
 * PAYLOAD
 *****************************************************************************/
#ifdef TXBOARD
#define TIME_3MS         (3000)              // max thresh hold (16MHZ /8 * 3ms)/NUM_CONVERT
#define NUM_CONVERT          (2)
uint16_t u16_count = 0, u16_channel[numChannel]= {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};
uint32_t u32_data1 = 0 , u32_data2 = 0 , u32_data = 0;
ENM_ADC_STATE_T enm_adc_state = ENM_ADC_STATE_INIT;
#else
uint16_t u16_channel[numChannel+1] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 3000};
uint16_t u16_rf_channel[numChannel+1] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 3000};
int index=0;
int ppmDelayTemp;
uint8_t ppmUpdateEnable=0;
#endif
/******************************************************************************
 * @fn         main
 *
 * @brief      Main GUI application level control loop is implemented in the
 *             main loop. It is a state machine that cycles thru various states
 *             during the interactive use by the operator. The presents a text
 *             based GUI, the user is then promted to type in a command with
 *             a given set of parameters. The state machine then calls a simpler
 *             parser that tries to figure out what the user wants to do.
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 *
 */
void main (void)
{
	/*  Stop WDT */
	WDTCTL = WDTPW + WDTHOLD;

	/* Setup MSP specific functions, IO's, timers and WDT */
	msp_setup();

#ifdef TXBOARD
	enm_adc_state = ENM_ADC_STATE_INIT;
    v_Config_ADC();
    v_Config_Mode_Selection();
#else
	v_Config_Timer_RX(); //Config Timer RXBOARD
#ifdef RXBOARD_V1
    P2DIR |= BIT0;
    P2OUT &= ~BIT0;
#else
	P2DIR |= BIT5;
	P2OUT &= ~BIT5;
#endif
#endif

    __delay_cycles(1600000);    //Wait for system become stable

#ifdef TXBOARD
    enm_adc_state = ENM_ADC_STATE_CALIB;
#endif

	_BIS_SR(GIE);       //Global Interrupt Enable

	/* Initialize the radio subsystem */
	trx_cfg.bit_rate = i_Radio_Init();
	trx_cfg.bit_rate = trx_cfg.bit_rate * 100;

	// Perform initial setup of the CC radio
	v_RF_Default_Setup(&trx_cfg);

#ifdef TXBOARD
	while (enm_adc_state == ENM_ADC_STATE_CALIB)
	{
	    ;   //Wait for adc calibration done
	}

        trx_cfg.trx_mode = MENU_TX_BER;
#else
        trx_cfg.trx_mode = MENU_RX_BER;
#endif

	/* Infinite loop with a 1 second timer */
	while(1)
	{
#ifdef TXBOARD
	    v_Read_Mode();
	    v_TX_Ber_Single(txBuffer, &trx_cfg);
#else
	    v_RX_Ber_Single(txBuffer, &trx_cfg);
        while (s16_Validate_Package() == 0)
	    {
            ppmUpdateEnable = 1;
#ifdef RXBOARD_V1
            P2OUT |= BIT0;
#else
            P2OUT |= BIT5;
#endif
	    }
#endif
    }
}  

/******************************************************************************
 * END OF FlyMew.c
 *****************************************************************************/

