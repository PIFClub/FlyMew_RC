/******************************************************************************
 *  Filename: LaunchPad_trx_main.c
 *
 *  Description: TRX_ui Range Test for various EVM boards and transcievers
 *  	         This wireless demo applications is meant to show the
 *  	         advantage of using. Frequency hopping Spread Spectrum (FHSS)
 *  	         to implement your wireless sensor network.
 *
 *  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************/

/******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>
#include "msp430.h"
#include "stdio.h"
#include "radio_drv.h"
#include "cc1x_utils.h"
#include "hal_spi_rf.h"
#include "LaunchPad_trx_demo.h"

//#define TXBOARD

#define ADC_CH1_IDX     3
#define ADC_CH2_IDX     6
#define ADC_CH3_IDX     1
#define ADC_CH4_IDX     0

#define ADC_CH1_REVERSE     0
#define ADC_CH2_REVERSE     0
#define ADC_CH3_REVERSE     1
#define ADC_CH4_REVERSE     1

/******************************************************************************
 * LOCAL FUNCTIONS
 */
extern void msp_setup(void);
extern unsigned long volatile time_counter;

void ConfigClock_IO_Timer_TX(void);
void ConfigClock_IO_Timer_RX(void);
void v_Config_ADC(void);
void v_ADC_Start_Conversion(void);
/******************************************************************************
 * GLOBALS
 */
unsigned char txBuffer[TX_BUFF_SIZE];
trx_cfg_struct trx_cfg;
extern unsigned char rf_end_packet;

/******************************************************************************
 * PAYLOAD
 */
#define numChannel       (12)

#ifdef TXBOARD
#define time_3ms         (3000)              // max thresh hold (16MHZ /8 * 3ms)/numconvert
#define numconvert          (2)
uint16_t count = 0, channel[numChannel]= {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};
uint32_t data1 = 0 , data2 = 0 , data = 0;
#else
uint16_t channel[numChannel+1] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 3000};
uint16_t rf_channel[numChannel+1] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 3000};
int index=0;
int ppmDelayTemp;
uint8_t ppmUpdateEnable=0;
#endif
#ifdef TXBOARD
uint16_t au16_adc_result[7];
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

//	v_Config_ADC();

#ifdef TXBOARD
	//ConfigClock_IO_Timer_TX(); //Config Timer TXBOARD
#else
	ConfigClock_IO_Timer_RX(); //Config Timer RXBOARD
#endif

	_BIS_SR(GIE);       //Global Interrupt Enable

	/* initialize the radio subsystem */
	trx_cfg.bit_rate = radio_init();
	trx_cfg.bit_rate = trx_cfg.bit_rate * 100;

	// Perform initial setup of the CC radio
	rf_default_setup(&trx_cfg);

#ifdef TXBOARD
        trx_cfg.trx_mode = MENU_TX_BER;
#else
        trx_cfg.trx_mode = MENU_RX_BER;
#endif

	/* Infinite loop with a 1 second timer */
	while(1)
	{
#ifdef TXBOARD
	    //v_ADC_Start_Conversion();
	    tx_ber_single(txBuffer, &trx_cfg);
//        __delay_cycles(160);                 // Idle for 10 us
#else
	    rx_ber_single(txBuffer, &trx_cfg);
        while (s16_Validate_Package() == 0)
	    {
            ;   //Do nothing, get the latest RC value
	    }
#endif
    }
}  

#ifdef TXBOARD
#pragma vector = PORT2_VECTOR;
__interrupt void timerA0(void)

{
	if(RF_GDO_PxIFG & RF_GDO_PIN) {

	// Clear LPM0 bits from 0(SR)
	__bic_SR_register_on_exit(LPM3_bits);

	// clear the interrupt flag
	RF_GDO_PxIFG &= ~RF_GDO_PIN;

	// indicate that end of packet has been found
	rf_end_packet = 1;
		}
	else
	{
    static uint8_t u8_pulse_idx = 0;
    TA1CCTL1 ^= CCIS0;
    P2IFG &= ~BIT0;
    data2 = data1;
    data1 = TA1CCR1 ;
    data = ((int32_t)data1 + (int32_t)count*(int32_t)65536 -(int32_t)data2)/(int32_t)numconvert;
    count = 0;

    /* If the measured time is longer than 3ms, it's the terminate pulse */
    if (data > time_3ms)
    {
      u8_pulse_idx = 0;
    }
    else
    {
      if (u8_pulse_idx < numChannel)
      {
        channel[u8_pulse_idx] = data;
        u8_pulse_idx++;
      }
    }
	}
}
#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_ISR (void)
{

    switch (TA1IV)
    {
    case 0x02:  //TACCR1 CCIFG
        /*
        TA0CTL |= TACLR;
        //Resets Counter, the clock divider, and the count direction
        */
        TA1CCTL1 &= ~CCIFG;     //Clear Interrupt Flag
        break;
    case 0x0a:  //Timer Overflow
        /*
         *
         */
    	//TA1CTL &= ~TAIFG;
        count++;
        break;
    }
}

void v_Config_ADC(void)
{
    ADC10CTL1 = INCH_6 + CONSEQ_3;            // A1/A0, multi channel
    ADC10CTL0 = ADC10SHT_2 + MSC + ADC10ON + ADC10IE;
    ADC10AE0 = BIT0 | BIT3 | BIT5 | BIT6;                          // P1.0,1 ADC option select
    ADC10DTC1 = 56;                         // 8 conversions
    ADC10CTL0 &= ~ENC;

    while (ADC10CTL1 & ADC10BUSY);               // Wait if ADC10 core is active
    ADC10SA = (uint16_t)au16_adc_result;                        // Data buffer start
    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion ready
}

void v_ADC_Start_Conversion(void)
{
//    if (ADC10CTL0 & ENC)
//    {
//
//    }
//    else
//    {
//        while (ADC10CTL1 & ADC10BUSY);               // Wait if ADC10 core is active
//        ADC10SA = (uint16_t)au16_adc_result;                        // Data buffer start
//        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion ready
//    }
//    ADC10CTL0 &= ~ENC;
}

void ConfigClock_IO_Timer_TX(void)
{
    P2SEL &= ~BIT0;
    P2SEL2 &= ~BIT0;
    P2DIR &= ~BIT0;
    P2REN |= BIT0;
    P2OUT |= BIT0;
    P2IE  |= BIT0;     // enalble ngat p2.0
    P2IES |= BIT0;   // ngat canh len
    P2IFG &= ~BIT0;     // xoa co ngat

    TA1CTL |= TASSEL_2 + ID_3 + MC_2 + TAIE;                // clock 16MHz, chia 8 , + continues , bat ngat timer
    TA1CCTL1 = CM_3 + CCIS_2 + CAP + SCS;                      // capture., dong bo clock,
}
#else
#pragma vector = PORT2_VECTOR;
__interrupt void radio_isr(void) {

	if(RF_GDO_PxIFG & RF_GDO_PIN) {

		// Clear LPM0 bits from 0(SR)
        __bic_SR_register_on_exit(LPM0_bits);

		// clear the interrupt flag
		RF_GDO_PxIFG &= ~RF_GDO_PIN;

		// indicate that end of packet has been found
		rf_end_packet = 1;
	}
}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer_A_1(void)
{
    static uint8_t u8_channel_idx = 0;
    switch(TA1IV)
    {
        case 2: P2OUT&=~BIT0; break;          // CCR1 interrupt
        case 4: break;          // CCR2 interrupt
        case 10:
            TA1CCTL0 &= ~TAIFG;

            P2OUT|=BIT0;
            u8_channel_idx++;

            if (u8_channel_idx > numChannel)
                u8_channel_idx = 0;

            TA1CCR0 = channel[u8_channel_idx] * 2;
            break;
    }
}

void ConfigClock_IO_Timer_RX(void)
{
	P2DIR|=BIT0;
	P2OUT&=~BIT0;

	channel[numChannel]=6000;

	TA1CCR1 = 500*2;       // PWM pulse width
	TA1CTL |= TASSEL_2 + MC_1 + TAIE + ID_3; //config timer :
	                            //clock DCO, mode MC_2, TAIE: cho phep ngat
	TA1CCTL1 |= CCIE;
	TA1CCR0 = 2*(channel[0]);      //PWM period
	P2OUT|=BIT0;
}
#endif
#ifdef TXBOARD
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    uint8_t u8_idx;

    ADC10CTL0 &= ~ENC;

    for (u8_idx = 0; u8_idx < 7; u8_idx++)
    {
        if (au16_adc_result[u8_idx] > 1000)
            au16_adc_result[u8_idx] = 1000;
    }

    if (ADC_CH1_REVERSE)
    {
        channel[0] = 2000 - au16_adc_result[ADC_CH1_IDX];
    }
    else
    {
        channel[0] = 1000 + au16_adc_result[ADC_CH1_IDX];
    }

    if (ADC_CH2_REVERSE)
    {
        channel[1] = 2000 - au16_adc_result[ADC_CH2_IDX];
    }
    else
    {
        channel[1] = 1000 + au16_adc_result[ADC_CH2_IDX];
    }

    if (ADC_CH3_REVERSE)
    {
        channel[2] = 2000 - au16_adc_result[ADC_CH3_IDX];
    }
    else
    {
        channel[2] = 1000 + au16_adc_result[ADC_CH3_IDX];
    }

    if (ADC_CH4_REVERSE)
    {
        channel[3] = 2000 - au16_adc_result[ADC_CH4_IDX];
    }
    else
    {
        channel[3] = 1000 + au16_adc_result[ADC_CH4_IDX];
    }

    while (ADC10CTL1 & ADC10BUSY);               // Wait if ADC10 core is active
    ADC10SA = (uint16_t)au16_adc_result;                        // Data buffer start
    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion ready
}
#endif
