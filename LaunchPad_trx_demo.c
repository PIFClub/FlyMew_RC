/******************************************************************************
 *  Filename: LaunchPad_trx_demo.c
 *
 *  Description: Range Test for various EVM boards and transcievers
 *  	         This wireless demo applications is meant to show the
 *  	         advantage of using. Frequency hopping Spread Spectrum (FHSS)
 *  	         to implement your wireless sensor network. This code base
 *  	         has been developed to run on a number of TI devices including
 *  	         most MSP430F2x and MSP430F5x series microcontrolers using stand
 *               alone wireless transcievers (CC1101, CC112x).
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
#include <stdint.h>
#include <cc1x_utils.h>
#include "stdio.h"
#include "stdlib.h"
#include "msp430.h"
#include "LaunchPad_trx_demo.h"
#include "hal_spi_rf.h"
#include "radio_drv.h"

/******************************************************************************
 * DEFINES - define the system ID
 */
#define MAX_RX_BUFF_LENGTH      (255)

#define SYNC_NIBBLE             (0xA0)
#define SYSTEM_ID               0       /*Valid IDs are 0-15*/
#define DATA_LENGTH             (12*2)
#define META_DATA_LENGTH        (4)     /* SYNC + DATA LENGTH + 2 byte-CRC */

/******************************************************************************
 * INTERNAL FUNCTIONS
 */
static uint16_t ModRTU_CRC(int8_t *ps8_buf, int16_t s16_len);

/******************************************************************************
 * INTERNAL VARIABLES - state machine control
 */
static uint8_t au8_rx_buff[MAX_RX_BUFF_LENGTH];
static uint16_t u16_rx_idx = 0;
extern uint16_t channel[];

/******************************************************************************
 * @fn         rf_default_setup
 *
 * @brief      read chip information to identity the specific device in circuit
 *
 * input parameters
 *
 * @param       trx_cfg_struct *trx_cfg    -  structure for control information
 *
 * output parameters
 *
 * @return      void
 *
 */
void rf_default_setup(trx_cfg_struct *trx_cfg)
{
	trx_cfg->trx_mode   = MENU_RESTART;
	trx_cfg->cc_state   = CC_IDLE;
	trx_cfg->no_burst   = 1;
	trx_cfg->rf_channel = 0;
	trx_cfg->smartrf_selector = 1;
	trx_cfg->packet_timer = 16000;
	trx_cfg->normal_wait_timer = 8000;
	trx_cfg->max_wait_timer = 32000;
	trx_cfg->sync_timer = 6432;
	trx_cfg->xtal_wait_timer = 32;
	trx_cfg->b_length = 28;
	trx_cfg->start_freq = 432999;
	trx_cfg->ch_spc = 200;
	trx_cfg->cc_device_id = get_device_id();

	// Set the packet length to a fixed packet size
	set_rf_packet_length(trx_cfg->b_length);

	/* configure the rx frequency to perform packet sniffing on */
	radio_set_freq(trx_cfg->start_freq+(trx_cfg->ch_spc*trx_cfg->rf_channel));
}

/******************************************************************************
 * @fn         tx_ber_single
 *
 * @brief      Perform RF burst transmit function on a single channel
 *
 * input parameters
 *
 * @param       trx_cfg_struct *trx_cfg    -  structure for control information
 *
 * output parameters
 *
 * @return      void
 *
 */
void tx_ber_single(unsigned char *txBuffer, trx_cfg_struct *trx_cfg)
{
    uint16_t u16_cnt;
    uint8_t *pu8_data = (uint8_t *)channel;
    uint16_t u16_crc;

    // put some data into the payload
    txBuffer[0] =  SYNC_NIBBLE | SYSTEM_ID;     // System ID
    txBuffer[1] =  DATA_LENGTH;                 // Number of data bytes
    
    for(u16_cnt=0; u16_cnt < DATA_LENGTH; u16_cnt++)
    {
        txBuffer[u16_cnt + 2] = *pu8_data++;
    }
    
    u16_crc = ModRTU_CRC((int8_t *)txBuffer, DATA_LENGTH + 2); //SYSTEM_ID + DATA_LENGTH + DATA
    txBuffer[26] = u16_crc & 0xFF;
    txBuffer[27] = u16_crc >> 8;
    
    // Transmit packet. The MCU does not exit this until the TX is complete.
    radio_send(txBuffer, trx_cfg->b_length);
    radio_wait_for_idle(0);         // 0 = no timeout, just wait

    return;
}


/******************************************************************************
 * @fn         rx_ber_single
 *
 * @brief      Perform RF burst receive function with frequency hopping
 *
 * input parameters
 *
 * @param            unsigned char *txBuffer
 *                   trx_cfg_struct *trx_cfg    -  structure for control information
 *
 * output parameters
 *
 * @return      void
 *
 */
int rx_ber_single(unsigned char *txBuffer, trx_cfg_struct *trx_cfg)
{
	uint16_t u16_rx_length;
	uint8_t u8_cc_status;
	int rssi, freq_error;

	/* Avoid compiler warning */
	(void)&rssi;
	(void)&freq_error;

	/* Initialize the RX chain, receive packet */
	radio_receive_on();

	// wait until end_of_packet is found, no timeout
	radio_wait_for_idle(0);

    /* Read as much data as possible */
	u16_rx_length = MAX_RX_BUFF_LENGTH - u16_rx_idx;
	u8_cc_status = radio_read(&au8_rx_buff[u16_rx_idx], (unsigned short *)&u16_rx_length);         // read content of FIFO

	if(u8_cc_status != 0)
	{
        /* Get the RSSI and check for the frequency error */
		rssi = radio_get_rssi();
		freq_error = radio_freq_error();
        
        /* New data has been appended to the rx buffer, increase data index to match the real data */
        u16_rx_idx += u16_rx_length;
	}
	else
	{
		// Check to see if we have lost the connection and we need to stop and hold
		radio_idle();                          // force idle and flush fifos
	}

	/* Initialize the RX chain, receive packet */
	radio_receive_on();

	return -1;
}


static uint16_t ModRTU_CRC(int8_t *ps8_buf, int16_t s16_len)
{
    /* Initialize value for CRC */
    uint16_t u16_crc = 0xFFFF;
    int16_t s16_pos, s16_cnt;

    for (s16_pos = 0; s16_pos < s16_len; s16_pos++)
    {
        u16_crc ^= (uint16_t)ps8_buf[s16_pos];          // XOR byte into least sig. byte of u16_crc

        for (s16_cnt = 8; s16_cnt > 0; s16_cnt--)
        {    
            // Loop over each bit
            if ((u16_crc & 0x0001) != 0)
            {      
                // If the LSB is set
                u16_crc >>= 1;                    // Shift right and XOR 0xA001
                u16_crc ^= 0xA001;
            }
            else                            // Else LSB is not set
                u16_crc >>= 1;                    // Just shift right
        }
    }
    // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
    return u16_crc;
}

int16_t s16_Validate_Package(void)
{
    uint16_t u16_crc, u16_pkg_crc;
    uint16_t u16_idx;
    uint16_t *pu16_data;
    uint16_t u16_cnt;
    uint8_t *pu8_src_buff, *pu8_dest_buff;
        
    if (u16_rx_idx < DATA_LENGTH + META_DATA_LENGTH)
    {
        return -1;
    }
        
    /* Validate data in the buffer */
    for (u16_idx = 0; u16_idx < u16_rx_idx; u16_idx++)
    {
        /* Find the synchronize byte */
        if ((au8_rx_buff[u16_idx] & SYNC_NIBBLE) == SYNC_NIBBLE)
        {
            /* Data length is matched */
            if (au8_rx_buff[u16_idx + 1] == DATA_LENGTH)
            {
                /* All the package has been received */
                if (u16_rx_idx >= u16_idx + DATA_LENGTH + META_DATA_LENGTH)
                {
                    u16_pkg_crc = *((uint16_t *)&au8_rx_buff[u16_idx + DATA_LENGTH + 2]);
                    u16_crc = ModRTU_CRC((int8_t *)&au8_rx_buff[u16_idx], DATA_LENGTH + 2); //SYSTEM_ID + DATA_LENGTH + DATA
                    
                    if (u16_pkg_crc == u16_crc)
                    {
                        pu16_data = (uint16_t *)&au8_rx_buff[u16_idx + 2];

                        for (u16_cnt = 0; u16_cnt < DATA_LENGTH / 2; u16_cnt++)
                        {
                            channel[u16_cnt] = 2 * pu16_data[u16_cnt];
                        }
                        
                        /* Moving remaining data */
                        u16_idx += DATA_LENGTH + META_DATA_LENGTH;
                        pu8_src_buff = &au8_rx_buff[u16_idx];
                        pu8_dest_buff = au8_rx_buff;
                        
                        for (; u16_idx < u16_rx_idx; u16_idx++)
                        {
                            *pu8_dest_buff++ = *pu8_src_buff++;
                        }
                        
                        if (u16_rx_idx > u16_idx)
                        {
                            u16_rx_idx -= u16_idx;
                        }
                        else
                        {
                            u16_rx_idx = 0;
                        }
                        return 0;
                    }
                }
            }
        }
    }
    
    if (u16_rx_idx > DATA_LENGTH + META_DATA_LENGTH)
    {
        /* Moving remaining data */
        u16_idx = u16_rx_idx - (DATA_LENGTH + META_DATA_LENGTH);
        pu8_src_buff = &au8_rx_buff[u16_idx];
        pu8_dest_buff = au8_rx_buff;
        
        for (; u16_idx < u16_rx_idx; u16_idx++)
        {
            *pu8_dest_buff++ = *pu8_src_buff++;
        }
        
        if (u16_rx_idx > u16_idx)
        {
            u16_rx_idx -= u16_idx;
        }
        else
        {
            u16_rx_idx = 0;
        }
    }
    
    return -1;
}
