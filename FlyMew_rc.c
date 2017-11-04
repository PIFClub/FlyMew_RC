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
 *  Module      : FlyMew_rc.c
 *  Description : RC for FLyMew platform
 *  Version     : 1.0
 *
 *  Author      : FW.Team
 *  Notes       :
 *
 *****************************************************************************/

/******************************************************************************
 * IMPORT
 *****************************************************************************/
#include <stdint.h>
#include <cc1x_utils.h>
#include "stdio.h"
#include "stdlib.h"
#include "msp430.h"
#include "FlyMew_rc.h"
#include "hal_spi_rf.h"
#include "radio_drv.h"
#include "bsp.h"

/******************************************************************************
 * DEFINE
 * Define the system ID
 *****************************************************************************/
#ifdef TXBOARD
#define MAX_RX_BUFF_LENGTH      (8)
#else
#define MAX_RX_BUFF_LENGTH      (255)
#endif
#define SYNC_NIBBLE             (0xA0)
#define SYSTEM_ID               0       /*Valid IDs are 0-15*/
#define DATA_LENGTH             (12*2)
#define META_DATA_LENGTH        (4)     /* SYNC + DATA LENGTH + 2 byte-CRC */

/******************************************************************************
 * INTERNAL FUNCTIONS
 *****************************************************************************/
static uint16_t ModRTU_CRC(int8_t *ps8_buf, int16_t s16_len);

/******************************************************************************
 * INTERNAL VARIABLES
 * State machine control
 *****************************************************************************/
static uint8_t au8_rx_buff[MAX_RX_BUFF_LENGTH];
static uint16_t u16_rx_idx = 0;
extern uint16_t u16_channel[];

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
void v_RF_Default_Setup(trx_cfg_struct *trx_cfg)
{
	trx_cfg->trx_mode   = MENU_RESTART;
	trx_cfg->cc_state   = CC_IDLE;
	trx_cfg->no_burst   = 1;
	trx_cfg->rf_channel = RF_CHANNEL;
	trx_cfg->smartrf_selector = 1;
	trx_cfg->packet_timer = 16000;
	trx_cfg->normal_wait_timer = 8000;
	trx_cfg->max_wait_timer = 32000;
	trx_cfg->sync_timer = 6432;
	trx_cfg->xtal_wait_timer = 32;
	trx_cfg->b_length = 28;
	trx_cfg->start_freq = 432999;
	trx_cfg->ch_spc = 200;
	trx_cfg->cc_device_id = c_Get_Device_ID();

	// Set the packet length to a fixed packet size
	u8_Set_RF_Packet_Length(trx_cfg->b_length);

	/* configure the rx frequency to perform packet sniffing on */
	i_Radio_Set_Freq(trx_cfg->start_freq+(trx_cfg->ch_spc*trx_cfg->rf_channel));
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
void v_TX_Ber_Single(unsigned char *txBuffer, trx_cfg_struct *trx_cfg)
{
    uint16_t u16_cnt;
    uint8_t *pu8_data = (uint8_t *)u16_channel;
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
    i_Radio_Send(txBuffer, trx_cfg->b_length);
    i_Radio_Wait_For_Idle(0);         // 0 = no timeout, just wait

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
int v_RX_Ber_Single(unsigned char *txBuffer, trx_cfg_struct *trx_cfg)
{
	uint16_t u16_rx_length;
	uint8_t u8_cc_status;
	int rssi, freq_error;

	/* Avoid compiler warning */
	(void)&rssi;
	(void)&freq_error;

	/* Initialize the RX chain, receive packet */
	i_Radio_Receive_On();

	// wait until end_of_packet is found, no timeout
	i_Radio_Wait_For_Idle(0);

    /* Read as much data as possible */
	u16_rx_length = MAX_RX_BUFF_LENGTH - u16_rx_idx;
	u8_cc_status = i_Radio_Read(&au8_rx_buff[u16_rx_idx], (unsigned short *)&u16_rx_length);         // read content of FIFO

	if(u8_cc_status != 0)
	{
        /* Get the RSSI and check for the frequency error */
		rssi = i_Radio_Get_RSSI();
		freq_error = i_Radio_Freq_Error();
        
        /* New data has been appended to the rx buffer, increase data index to match the real data */
        u16_rx_idx += u16_rx_length;
	}
	else
	{
		// Check to see if we have lost the connection and we need to stop and hold
		i_Radio_Idle();                          // force idle and flush fifos
	}

	/* Initialize the RX chain, receive packet */
	i_Radio_Receive_On();

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
    volatile uint16_t u16_crc, u16_pkg_crc;
    volatile uint16_t u16_idx;
    volatile uint16_t *pu16_data;
    volatile uint16_t u16_cnt;
    volatile uint8_t *pu8_src_buff, *pu8_dest_buff;
        
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
                            u16_channel[u16_cnt] = pu16_data[u16_cnt];
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

/******************************************************************************
 * END OF FlyMew_rc.c
 *****************************************************************************/
