/******************************************************************************
 *  Filename: cc1101_drv.c
 *
 *  Description: Radio driver abstraction layer, this uses the same concept
 *               as found in Contiki OS (https://github.com/contiki-os/contiki)
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
 ******************************************************************************/
#include "msp430.h"
#include "cc1101_def.h"
#include "radio_drv.h"
#include "hal_spi_rf.h"

#ifdef USE_CC1101

/* Select the correct XTAL frequency for the hardware */
#define SCALING_FREQ     (float)((RF_XTAL)/65.536)
#define SCALING_FREQEST  (unsigned long)((RF_XTAL)/16.384)

/*******************************************************************************
 * Globals used inside the physical layer
 *******************************************************************************/
unsigned char paTable[1];           
unsigned char rf_end_packet = 0;

// Address Config = No address check
// Base Frequency = 432.999817
// CRC Autoflush = false
// CRC Enable = false
// Carrier Frequency = 432.999817
// Channel Spacing = 199.951172
// Data Format = Synchronous serial mode
// Data Rate = 1.19948
// Deviation = 5.157471
// Device Address = 0
// Manchester Enable = false
// Modulated = true
// Modulation Format = GFSK
// Packet Length = 28
// Packet Length Mode = Infinite packet length mode
// Preamble Count = 4
// RX Filter BW = 58.035714
// Sync Word Qualifier Mode = No preamble/sync
// TX Power = 10
const registerSetting_t preferredSettings_433mhz[]=
{
     {0x0002, 0x06},
     {0x0003, 0x47},
     {0x0008, 0x05},
     {0x000B, 0x06},
     {0x000D, 0x10},
     {0x000E, 0xA7},
     {0x000F, 0x62},
     {0x0010, 0xC7},
     {0x0011, 0x83},
     {0x0012, 0x13},
     {0x0015, 0x40},
     {0x0018, 0x18},
     {0x0019, 0x16},
     {0x001B, 0x43},
     {0x0020, 0xFB},
     {0x0023, 0xE9},
     {0x0024, 0x2A},
     {0x0025, 0x00},
     {0x0026, 0x1F},
     {0x002C, 0x81},
     {0x002D, 0x35},
     {0x002E, 0x09},
};

/******************************************************************************
 * @fn         radio_init
 *
 * @brief      Initialize the radio hardware
 *
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 *
 *
 */
int i_Radio_Init(void) {

	unsigned char i, writeByte, preferredSettings_length;
	int bit_rate;
	registerSetting_t *preferredSettings;


	// Configure SPI port to communicate with transciever
	trxRfSpiInterfaceInit(2);

	// initialize a reset
	trxSpiCmdStrobe(RF_SRES);

	//give the tranciever time enough to complete reset cycle
	__delay_cycles(16000);
    
    preferredSettings_length = sizeof(preferredSettings_433mhz)/sizeof(registerSetting_t);
    preferredSettings = (registerSetting_t *)preferredSettings_433mhz;
    bit_rate = 12;

	// Write registers to radio
	for(i = 0; i < preferredSettings_length; i++) {
		writeByte = preferredSettings[i].data;
		trx8BitRegAccess(RADIO_WRITE_ACCESS, preferredSettings[i].addr, &writeByte, 1);
	}

	paTable[0] = 0xC0;                         // PATABLE (10 dBm output power)
	trx8BitRegAccess(RADIO_WRITE_ACCESS|RADIO_BURST_ACCESS, PATABLE, paTable, 1);

	return bit_rate;
}


/******************************************************************************
 * @fn         radio_prepare
 *
 * @brief      Prepare the radio with a packet to be sent, but do not send
 *
 * input parameters
 *
 * @param       unsigned char *payload     - pointer to payload
 *              unsigned short payload_len - payload length information
 *
 * output parameters
 *
 * @return      0
 *
 *
 */
int i_Radio_Prepare(unsigned char *payload, unsigned short payload_len) {

	trx8BitRegAccess(RADIO_WRITE_ACCESS+RADIO_BURST_ACCESS, TXFIFO, payload, payload_len);

	return 0;
}

/******************************************************************************
 * @fn         radio_transmit
 *
 * @brief      Send the packet that has previously been prepared (used for
 *             exact timing)
 *
 * input parameters
 *
 * @param       unsigned char *payload     - pointer to payload
 *              unsigned short payload_len - payload length information
 *
 * output parameters
 *
 * @return      0
 *
 *
 */
int i_Radio_Transmit(void) {
	trxSpiCmdStrobe(RF_STX);               // Change state to TX, initiating

	return(0);
}

/******************************************************************************
 * @fn         radio_receive_on
 *
 * @brief      Initiate the RX chain
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 *
 *
 */
int i_Radio_Receive_On(void) {
	trxSpiCmdStrobe(RF_SRX);                 // Change state to TX, initiating

	return(0);
}

/******************************************************************************
 * @fn         radio_send
 *
 * @brief      Prepare & transmit a packet in same call
 *
 *
 * input parameters
 *
 * @param       unsigned char *payload
 *              unsigned short payload_len
 *
 * output parameters
 *
 * @return      void
 *
 *
 */
int i_Radio_Send(unsigned char *payload, unsigned short payload_len) {

	trx8BitRegAccess(RADIO_WRITE_ACCESS|RADIO_BURST_ACCESS, TXFIFO, payload, payload_len);

	trxSpiCmdStrobe(RF_STX);               // Change state to TX, initiating
	return(0);
}

/******************************************************************************
 * @fn         radio_read
 *
 * @brief      Read a received packet into a buffer
 *
 *
 * input parameters
 *
 * @param       unsigned char *buf
 *              unsigned short buf_len
 *
 * output parameters
 *
 * @return      void
 *
 *
 */

int i_Radio_Read(unsigned char *buf, unsigned short *buf_len) {
	unsigned char status;
	unsigned char pktLen;

	/* Read number of bytes in RX FIFO */
	trx8BitRegAccess(RADIO_READ_ACCESS|RADIO_BURST_ACCESS, RXBYTES, &pktLen, 1);
	pktLen = pktLen  & NUM_RXBYTES;

	/* make sure the packet size is appropriate, that is 1 -> buffer_size */
	if (pktLen > 0) {
		/* retrieve the FIFO content */
        if (pktLen < *buf_len)
        {
            trx8BitRegAccess(RADIO_READ_ACCESS|RADIO_BURST_ACCESS, RXFIFO, buf, pktLen);
            
            /* return the actual length of the FIFO */
            *buf_len = pktLen;
        }
        else
            trx8BitRegAccess(RADIO_READ_ACCESS|RADIO_BURST_ACCESS, RXFIFO, buf, *buf_len);

		/* retrieve the CRC status information */
		trx8BitRegAccess(RADIO_READ_ACCESS+RADIO_BURST_ACCESS, PKTSTATUS, &status, 1);

	} else {
		/* if the length returned by the transciever does not make sense, flush it */
		*buf_len = 0;                                // 0
		status = 0;
		trxSpiCmdStrobe(RF_SFRX);	                 // Flush RXFIFO
	}

	/* return status information, CRC OK or NOT OK */
	return (status & CRC_OK);
}

/******************************************************************************
 * @fn         radio_channel_clear
 *
 * @brief      Perform a Clear-Channel Assessment (CCA) to find out if
 *             channel is clear
 *
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      0  - no carrier found
 *              >0 - carrier found
 *
 */
int i_Radio_Channel_Clear(void) {
	unsigned char status;

	/* get PKTSTATUS, and return the carrier sense signal */
	trx8BitRegAccess(RADIO_READ_ACCESS+RADIO_BURST_ACCESS, PKTSTATUS, &status, 1);

	/* return the carrier sense signal */
	return(status  & 0x40);
}

/******************************************************************************
 * @fn         radio_channel_clear
 *
 * @brief      Wait for end of packet interupt to happen
 *
 *             Timeout is controlled by TimerA running at 8MHz
 *             64000 = 128ms, 32000 = 64ms, 16000 = 32ms
 *             0 = no timeout.
 *
 * input parameters
 *
 * @param       max_hold   :  Watch dog timeout, no end of packet = 0;
 *
 * output parameters
 *
 * @return      timer value:  Interupt happened based on end_of_packet interupt
 *
 */
int i_Radio_Wait_For_Idle(unsigned short max_hold) {

	unsigned int status;
	unsigned char reg_status;

	/* check that we are still in RX mode before entering wait for RX end */
	trx8BitRegAccess(RADIO_READ_ACCESS+RADIO_BURST_ACCESS, MARCSTATE, &reg_status, 1);

	/* filter out only the status section of the register values */
	reg_status  = (reg_status & 0x1F);

	/* check for not idle mode */
	if(!(reg_status == MARCSTATE_IDLE)) {

		rf_end_packet = 0;  // initialize global variable for use in this function

		RF_GDO_PxIES |= RF_GDO_PIN;       // Int on falling edge (end of pkt)
		RF_GDO_PxIFG &= ~RF_GDO_PIN;      // Clear flag
		RF_GDO_PxIE |= RF_GDO_PIN;        // Enable int on end of packet

		/* wait for idle */
        // wait for radio to interupt us to continue processing
        status = 0;
        _BIS_SR(LPM0_bits + GIE);             // Enter LPM0

		/********  Setup the GDO ports to not interupts ****************************/
		RF_GDO_PxIE &= ~RF_GDO_PIN;              // Disable int on end of packet

	}
	/* Get timer values, however if we did not get a packet in time use 0      */
	if(rf_end_packet == 0) {
		status = max_hold;
	}

	return status;
}

/******************************************************************************
 * @fn         radio_is_busy
 *
 * @brief      Wait for radio to become idle
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
int i_Radio_Is_Busy(void) {

	// Wait GDO0 to go hi -> sync TX'ed
	while (!(RF_GDO_IN & RF_GDO_PIN));

	// Wait GDO0 to go low again -> sync TX'ed
	while (RF_GDO_IN & RF_GDO_PIN);

	// Wait GDO0 to clear -> end of pkt
	RF_GDO_PxIFG &= ~RF_GDO_PIN;          // After pkt TX, this flag is set.

	return(0);
}

/******************************************************************************
 * @fn         radio_pending_packet
 *
 * @brief      Check if the radio driver has just received a packet
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      rf_end_packet - 1 if packet is available, 0 if no packet
 *
 */
int i_Radio_Pending_Packet(void) {

	RF_GDO_PxIES |= RF_GDO_PIN;       // Int on falling edge (end of pkt)
	RF_GDO_PxIE |= RF_GDO_PIN;        // Enable int on end of packet

	return rf_end_packet;
}

/******************************************************************************
 * @fn         radio_clear_pending_packet
 *
 * @brief      Clear pending packet indicator
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
int i_Radio_Clear_Pending_Packet(void) {

	RF_GDO_PxIES &= ~RF_GDO_PIN;       // Int on falling edge (end of pkt)
	RF_GDO_PxIE &= ~RF_GDO_PIN;        // Enable int on end of packet

	rf_end_packet = 0;

	return 0;
}

/******************************************************************************
 * @fn         radio_set_pwr
 *
 * @brief      Set the output power of the CC1101 by looking up in table
 *
 * input parameters
 *
 * @param       int tx_pwr - wanted power
 *
 * output parameters
 *
 * @return      int tx_pwr - actual power chosen from lookup table
 *
 */
int i_Radio_Set_Pwr(int tx_pwr) {
	int actual_pwr, ee;
	unsigned char reg_access;

	/* lookup table for PA power codes */
	const unsigned char paTable_CC1101[10] =
	{0x03, 0x0E, 0x1E, 0x27, 0x38, 0x8E, 0x84, 0xCC, 0xC3, 0xC0};

	/* lookup table for PA power values */
	const int paOut_CC1101[10] = {-30, -20, -15, -10, -6, 0, 5, 7, 10, 12};

	/* for each entry in the power table loop */
	for(ee=0; ee<10;ee++){
		actual_pwr = paOut_CC1101[ee];

		/* check if requested value is less than */
		if(tx_pwr <= actual_pwr){

			/* update the PA table entry to the newly requested power value */
			reg_access = paTable_CC1101[ee];
			trx8BitRegAccess(RADIO_WRITE_ACCESS | RADIO_BURST_ACCESS, PATABLE, &reg_access, 1);

			/* terminate for loop */
			ee=10;
		}
	}

	return actual_pwr;
}



/******************************************************************************
 * @fn          radio_set_freq
 *
 * @brief       Sets the rf center frequency based on a frequency value to the
 *              nearest 1KHz. The Equation used is the following:
 *
 *              f_c = (f_xosc/2^16) * FREQ (control word)
 *
 *              FREQ = (f_c * 2^16)/f_osc;
 *
 * output parameters
 *
 * @return      void
 *
 */
int i_Radio_Set_Freq(unsigned long freq) {

	unsigned long freq_word;
	unsigned char freq_byte[3];
	float freq_float;

	// calculate the frequency word

	freq_float = freq*1000;
	freq_word = (unsigned long) (freq_float * 1/(float)SCALING_FREQ);

	/* return the frequency word */
	freq_byte[2] = ((uint8_t *)&freq_word)[0];
	freq_byte[1] = ((uint8_t *)&freq_word)[1];
	freq_byte[0] = ((uint8_t *)&freq_word)[2];

	// upload the frequency word to the transciver using a 3 byte write
	trx8BitRegAccess(RADIO_WRITE_ACCESS | RADIO_BURST_ACCESS , FREQ2, freq_byte, 3);

	return 0;
}


/******************************************************************************
 * @fn         radio_idle
 *
 * @brief      Idle the radio and flush buffers
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
int i_Radio_Idle(void) {
	/* Idle range extender */
	trxSpiCmdStrobe(RF_SIDLE);

	/* Flush the FIFO's */
	trxSpiCmdStrobe(RF_SFRX);
	trxSpiCmdStrobe(RF_SFTX);
	return(0);
}

/******************************************************************************
 * @fn         radio_sleep
 *
 * @brief      Enter sleep mode
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
int i_Radio_Sleep(void) {
	/* Force transciever idle state */
	trxSpiCmdStrobe(RF_SIDLE);

	/* Enter sleep state on exit */
	trxSpiCmdStrobe(RF_SPWD);

	return(0);
}

/******************************************************************************
 * @fn         radio_wake
 *
 * @brief      Exit sleep mode
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
int i_Radio_Wake(void) {

	/* Force transciever idle state */
	trxSpiCmdStrobe(RF_SIDLE);

	/* 1 ms delay for letting RX settle */
	__delay_cycles(1000);


	return(0);
}


/******************************************************************************
 * @fn          radio_freq_error
 *
 * @brief       Estimate the frequency error from two complement coded
 *              data to frequency error in Hertz
 *
 * input parameters
 *
 * @param       freq_reg_error -  two complement formatted data from tranceiver
 *
 * output parameters
 *
 * @return      freq_error     - 32 bit signed integer value representing
 *                                frequency error in Hertz
 *
 */
int i_Radio_Freq_Error(void) {

	long freq_error_est;
	unsigned char regState;

    /* Read marcstate to check for frequency error estimate */
	trx8BitRegAccess(RADIO_READ_ACCESS | RADIO_BURST_ACCESS, FREQEST, &regState, 1);

    /* Calculate the frequency error in Hz */
	freq_error_est = regState;

	/* the incoming data is 8 bit two complement format, separate "sign" */
	if (freq_error_est > 128) {
		freq_error_est = freq_error_est - 256;
	}

	/* convert the data to hertz format in two steps to avoid integer overuns */
	freq_error_est = freq_error_est * (long)SCALING_FREQEST;

	return ((int)freq_error_est);
}

/******************************************************************************
 * @fn         radio_isr
 *
 * @brief      Interrupt service routine used by transciever
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
#pragma vector=RF_PORT_VECTOR
__interrupt void RADIO_ISR(void) {

	if(RF_GDO_PxIFG & RF_GDO_PIN) {

		// Clear LPM0 bits from 0(SR)
		__bic_SR_register_on_exit(LPM3_bits);

		// clear the interrupt flag
		RF_GDO_PxIFG &= ~RF_GDO_PIN;

		// indicate that end of packet has been found
		rf_end_packet = 1;
	}
}

#endif
