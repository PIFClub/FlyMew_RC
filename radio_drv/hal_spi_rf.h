/******************************************************************************
 *  Filename: hal_spi_rf_trx.h
 *
 *  Description: Implementation file for common spi access with the CCxxxx
 *               transceiver radios using trxeb. Supports CC1101/CC112X radios
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
/******************************************************************************
 *  Filename: hal_msp_exp430g2_spi.h
 *
 *  Description: Common header file for spi access to the different tranceiver
 *               radios. Supports CC1101/CC112X radios
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
#ifndef HAL_SPI_RF_H
#define HAL_SPI_RF_H

#ifdef __cplusplus
extern "C" {

#endif

/******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "msp430.h"
#include "hal_defs.h"
#include "../bsp.h"

#define     RADIO_BURST_ACCESS      0x40
#define     RADIO_SINGLE_ACCESS     0x00
#define     RADIO_READ_ACCESS       0x80
#define     RADIO_WRITE_ACCESS      0x00

/* Bit fields in the chip status byte */
#define STATUS_CHIP_RDYn_BM              0x80
#define STATUS_STATE_BM                  0x70
#define STATUS_FIFO_BYTES_AVAILABLE_BM   0x0F


/******************************************************************************
 * MACROS
 */
#define CODE
#define XDATA
#define FAR
#define NOP()  asm(" nop")

#if (defined TXBOARD) | (defined RXBOARD_V1)
#define RF_SPI_BEGIN()              st( RF_CS_N_PORT_OUT &= ~RF_CS_N_PIN; NOP(); )
#define RF_SPI_TX(x)                st( IFG2 &= ~UCA0RXIFG; UCA0TXBUF = (x); )
#define RF_SPI_WAIT_DONE()          st( while(!(IFG2 & UCA0RXIFG)); )
#define RF_SPI_WAIT_TX_DONE()       st( while(!(IFG2 & UCA0TXIFG)); )
#define RF_SPI_RX()                 UCA0RXBUF
#define UCx0CTL0                    UCA0CTL0
#define UCx0CTL1                    UCA0CTL1
#define UCx0BR1                     UCA0BR1
#define UCx0BR0                     UCA0BR0
#else
#define RF_SPI_BEGIN()              st( RF_CS_N_PORT_OUT &= ~RF_CS_N_PIN; NOP(); )
#define RF_SPI_TX(x)                st( IFG2 &= ~UCB0RXIFG; UCB0TXBUF = (x); )
#define RF_SPI_WAIT_DONE()          st( while(!(IFG2 & UCB0RXIFG)); )
#define RF_SPI_WAIT_TX_DONE()       st( while(!(IFG2 & UCB0TXIFG)); )
#define RF_SPI_RX()                 UCB0RXBUF
#define UCx0CTL0                    UCB0CTL0
#define UCx0CTL1                    UCB0CTL1
#define UCx0BR1                     UCB0BR1
#define UCx0BR0                     UCB0BR0
#endif

#define RF_SPI_WAIT_MISO_LOW(x)     st( uint8 count = 200; \
                                    while(RF_PORT_IN & RF_SPI_MISO_PIN) \
                                    { \
                                      __delay_cycles(5000); \
                                      count--; \
                                    if (count == 0) break; \
                                    } \
                                    if(count>0) (x) = 1; \
                                      else (x) = 0; )
#define RF_SPI_END()              st( NOP(); RF_CS_N_PORT_OUT |= RF_CS_N_PIN; )

/******************************************************************************
 * TYPEDEFS
 */

typedef struct
{

    uint16_t  addr;
    uint8_t   data;

}registerSetting_t;

typedef uint8_t rfStatus_t;
/******************************************************************************
 * PROTOTYPES
 */
void trxRfSpiInterfaceInit(uint8_t prescalerValue);
rfStatus_t trx8BitRegAccess(uint8_t accessType, uint8_t addrByte, uint8_t *pData, uint16_t len);
rfStatus_t trxSpiCmdStrobe(uint8_t cmd);

/* CC112X specific prototype function */
rfStatus_t trx16BitRegAccess(uint8_t accessType, uint8_t extAddr, uint8_t regAddr, uint8_t *pData, uint8_t len);

// CC Chip versions
#define DEV_UNKNOWN       10
#define DEV_CC1100        11
#define DEV_CC1101        12
#define DEV_CC2500        13
#define DEV_CC430x        14
#define DEV_CC1120        15
#define DEV_CC1121        16
#define DEV_CC1125        17
#define DEV_CC1200        18
#define DEV_CC1201        19
#define DEV_CC1175        20

#define RADIO_GENERAL_ERROR     0x00
#define RADIO_CRC_OK            0x80
#define RADIO_IDLE              0x81
#define RADIO_RX_MODE           0x82
#define RADIO_TX_MODE           0x83
#define RADIO_RX_ACTIVE         0x84
#define RADIO_TX_ACTIVE         0x85
#define RADIO_SLEEP             0x86
#define RADIO_TX_PACKET_RDY     0x87
#define RADIO_CHANNEL_NOT_CLR   0x88
#define RADIO_CHANNEL_IS_CLR    0x89

#ifdef  __cplusplus

}
#endif

#endif
