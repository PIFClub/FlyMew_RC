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
 *  Module      : PPM.c
 *  Description : PPM
 *  Version     : 1.0
 *
 *  Author      : FW.Team
 *  Notes       :
 *
 *****************************************************************************/

/******************************************************************************
 * IMPORT
 *****************************************************************************/
#include "PPM.h"

/******************************************************************************
 * FUNCTIONS
 *****************************************************************************/
#ifdef TXBOARD

void v_Config_Mode_Selection(void)
{
    TX_MODE_PORT_DIR &= ~TX_MODE_1_PIN | TX_MODE_2_PIN;
    TX_MODE_PORT_REN |= TX_MODE_1_PIN | TX_MODE_2_PIN;
    TX_MODE_PORT_OUT |= TX_MODE_1_PIN | TX_MODE_2_PIN;
}

void v_Read_Mode(void)
{
    static uint8_t u8_mode_1_ = 0;
    static uint8_t u8_mode_2_ = 0;
    uint8_t u8_flight_mode = 0;

    if (TX_MODE_PORT_IN & TX_MODE_1_PIN)
    {
        if (u8_mode_1_ == 1)
        {
            u8_flight_mode |= 0x02;
        }
        u8_mode_1_ = 1;
    }
    else
    {
        u8_mode_1_ = 0;
    }

    if (TX_MODE_PORT_IN & TX_MODE_2_PIN)
    {
        if (u8_mode_2_ == 1)
        {
            u8_flight_mode |= 0x01;
        }
        u8_mode_2_ = 1;
    }
    else
    {
        u8_mode_2_ = 0;
    }

    if (u8_flight_mode > 5)
    {
        u8_flight_mode = 5;
    }

    u16_channel[4] = 1170 + (130 * u8_flight_mode);

    if (u8_flight_mode & 0x01)  //Use the Left switch to control channel 7
    {
        u16_channel[6] = 2000;
    }
    else
    {
        u16_channel[6] = 1000;
    }
}

#else
extern uint8_t ppmUpdateEnable;

#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer_A1_ISR(void)
{
    static uint8_t u8_channel_idx = 0;
    static uint16_t u16_rf_check = 0;

    switch(TA1IV)
    {
        case 2: PPM_PORT_OUT &= ~PPM_PIN_OUT; break;          // CCR1 interrupt
        case 4: break;          // CCR2 interrupt
        case 10:
            TA1CCTL0 &= ~TAIFG;

            // RC cycle ~20-30ms
            // 50 * RC cycle ~ 1-1.5s
            if (u16_rf_check < 100)
            {
                PPM_PORT_OUT |= PPM_PIN_OUT;
            }

            u8_channel_idx++;

            if (u8_channel_idx > numChannel)
            {
                u8_channel_idx = 0;
                if (ppmUpdateEnable == 0)   //No rf signal
                {
                    if (u16_rf_check < 100)
                        u16_rf_check++;
#ifdef RXBOARD_V1
                    P2OUT &= ~BIT0;
#else
                    P2OUT &= ~BIT5;
#endif
                }
                else
                {
                    u16_rf_check = 0;
                    ppmUpdateEnable = 0;
                }
            }

            TA1CCR0 = u16_channel[u8_channel_idx] * 2;
            break;
    }
}

void v_Config_Timer_RX(void)
{
	PPM_PORT_DIR |= PPM_PIN_OUT;
	PPM_PORT_OUT &= ~PPM_PIN_OUT;

	u16_channel[numChannel]=6000;

	TA1CCR1 = 500*2;       						// PWM pulse width
	TA1CTL |= TASSEL_2 + MC_1 + TAIE + ID_3; 	//config timer :
	                            				//clock DCO, mode MC_2, TAIE: cho phep ngat
	TA1CCTL1 |= CCIE;
	TA1CCR0 = (u16_channel[0]) << 1;      				//PWM period
	PPM_PORT_OUT |= PPM_PIN_OUT;
}
#endif

/******************************************************************************
 * END OF PPM.c
 *****************************************************************************/
