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
 *	Module		: bsp.h
 *	Description	: 
 *  Version     : 1.0
 *
 *	Author		: FW.Team
 *	Notes		:
 *
 *****************************************************************************/

#ifndef BSP_H_
#define BSP_H_

/******************************************************************************
 * SPI_RF
 *****************************************************************************/

/******************************************************************************
 * DEFINITIONS
 * Define tranceiver to use
 *****************************************************************************/

#define USE_CC1101                     /* use the CC110x transciever commands */
#define RF_XTAL 26000                  /* default is 26000 for CC1101 */
                                       /* 32000 for CC1120 */
                                       /* 40000 for CC1125 */
#define TXBOARD

//#define RXBOARD_V1
#ifdef TXBOARD
#undef RXBOARD_V1
#endif

/******************************************************************************
 * CONSTANT
 *****************************************************************************/

#ifdef TXBOARD
/* Transceiver SPI signal */
#define     RF_PORT_SEL             P1SEL
#define     RF_PORT_SEL2            P1SEL2
#define     RF_PORT_DIR             P1DIR
#define     RF_PORT_OUT             P1OUT
#define     RF_PORT_IN              P1IN

#define     RF_MOSI_PIN             BIT2
#define     RF_MISO_PIN             BIT1
#define     RF_SCLK_PIN             BIT4

/* Transceiver chip select signal */
#define     RF_CS_N_PORT_SEL        P2SEL
#define     RF_CS_N_PORT_DIR        P2DIR
#define     RF_CS_N_PORT_OUT        P2OUT
#define     RF_CS_N_PIN             BIT5

/* Transceiver interrupt configuration */
#define     RF_PORT_VECTOR         PORT2_VECTOR
#define     RF_GDO_OUT             P2OUT
#define     RF_GDO_DIR             P2DIR
#define     RF_GDO_IN              P2IN
#define     RF_GDO_SEL             P2SEL
#define     RF_GDO_PxIES           P2IES
#define     RF_GDO_PxIFG           P2IFG
#define     RF_GDO_PxIE            P2IE
#define     RF_GDO_PIN             BIT4
#else
#ifdef RXBOARD_V1
/* Transceiver SPI signal */
#define     RF_PORT_SEL             P1SEL
#define     RF_PORT_SEL2            P1SEL2
#define     RF_PORT_DIR             P1DIR
#define     RF_PORT_OUT             P1OUT
#define     RF_PORT_IN              P1IN

#define     RF_MOSI_PIN             BIT2
#define     RF_MISO_PIN             BIT1
#define     RF_SCLK_PIN             BIT4

/* Transceiver chip select signal */
#define     RF_CS_N_PORT_SEL        P1SEL
#define     RF_CS_N_PORT_DIR        P1DIR
#define     RF_CS_N_PORT_OUT        P1OUT
#define     RF_CS_N_PIN             BIT5

/* Transceiver interrupt configuration */
#define     RF_PORT_VECTOR         PORT2_VECTOR
#define     RF_GDO_OUT             P2OUT
#define     RF_GDO_DIR             P2DIR
#define     RF_GDO_IN              P2IN
#define     RF_GDO_SEL             P2SEL
#define     RF_GDO_PxIES           P2IES
#define     RF_GDO_PxIFG           P2IFG
#define     RF_GDO_PxIE            P2IE
#define     RF_GDO_PIN             BIT4
#else
/* Transceiver SPI signal */
#define     RF_PORT_SEL             P1SEL
#define     RF_PORT_SEL2            P1SEL2
#define     RF_PORT_DIR             P1DIR
#define     RF_PORT_OUT             P1OUT
#define     RF_PORT_IN              P1IN

#define     RF_MOSI_PIN             BIT7
#define     RF_MISO_PIN             BIT6
#define     RF_SCLK_PIN             BIT5

/* Transceiver chip select signal */
#define     RF_CS_N_PORT_SEL        P1SEL
#define     RF_CS_N_PORT_DIR        P1DIR
#define     RF_CS_N_PORT_OUT        P1OUT
#define     RF_CS_N_PIN             BIT4

/* Transceiver interrupt configuration */
#define     RF_PORT_VECTOR         PORT2_VECTOR
#define     RF_GDO_OUT             P2OUT
#define     RF_GDO_DIR             P2DIR
#define     RF_GDO_IN              P2IN
#define     RF_GDO_SEL             P2SEL
#define     RF_GDO_PxIES           P2IES
#define     RF_GDO_PxIFG           P2IFG
#define     RF_GDO_PxIE            P2IE
#define     RF_GDO_PIN             BIT4
#endif
#endif
/*******************************************************************************
 * TX_MODE_SELECTION
 */

#define		TX_MODE_PORT_DIR		P2DIR
#define		TX_MODE_PORT_REN		P2REN
#define		TX_MODE_PORT_OUT		P2OUT
#define		TX_MODE_PORT_IN			P2IN
#define		TX_MODE_1_PIN			BIT1
#define  	TX_MODE_2_PIN			BIT2

/*******************************************************************************
 * PPM
 */
#define 	numChannel       		(12)
#define 	PPM_PORT_DIR 			P2DIR
#define 	PPM_PORT_OUT			P2OUT
#ifdef RXBOARD_V1
#define     PPM_PIN_OUT             BIT5
#else
#define		PPM_PIN_OUT				BIT0
#endif
/*******************************************************************************
 * ADC
 */

#define  	ADC_ANALOG_INPUT_0		BIT0
#define  	ADC_ANALOG_INPUT_1		BIT1
#define  	ADC_ANALOG_INPUT_2		BIT2
#define  	ADC_ANALOG_INPUT_3		BIT3
#define  	ADC_ANALOG_INPUT_4		BIT4
#define  	ADC_ANALOG_INPUT_5		BIT5
#define  	ADC_ANALOG_INPUT_6		BIT6
#define  	ADC_ANALOG_INPUT_7		BIT7

#define     RF_CHANNEL              0

#endif /* BSP_H_ */

/******************************************************************************
 * END OF bsp.h
 *****************************************************************************/
