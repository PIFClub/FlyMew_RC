/******************************************************************************
 *  Filename: radio_drv.h
 *
 *  Description: Radio driver abstraction layer, this uses the same concept
 *               as found in Contiki OS.
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

/* Initialize the radio hardware */
int i_Radio_Init(void);

/* Prepare the radio with a packet to be sent */
int i_Radio_Prepare(unsigned char *payload, unsigned short payload_len);

/* Send the packet that has previously been prepared (used for exact timing)*/
int i_Radio_Transmit(void);

/* Enter recieve mode */
int i_Radio_Receive_On(void);

/* Prepare & transmit a packet in same call (slightly worse timing jitter) */
int i_Radio_Send(unsigned char *payload, unsigned short payload_len);

/* Read a received packet into a buffer */
int i_Radio_Read(unsigned char *buf, unsigned short *buf_len);

/* Perform a Clear-Channel Assessment (CCA) to find out if channel is clear */
int i_Radio_Channel_Clear(void);

/* Wait for radio to become idle (currently receiving or transmitting) */
int i_Radio_Wait_For_Idle(unsigned short max_hold);

/* Check if the radio driver has just received a packet */
int i_Radio_Pending_Packet(void);

/* Clear the flag that the driver has just received a packet */
int i_Radio_Clear_Pending_Packet(void);

/* Change rf transmit power of radio */
int i_Radio_Set_Pwr(int tx_pwr);

/* Change channel of radio */
int i_Radio_Set_Freq(unsigned long freq);

/* Idle the radio, used when leaving low power modes (below)*/
int i_Radio_Idle(void);

/* Put the radio into sleep mode */
int i_Radio_Sleep(void);

/* Wake the radio from sleep mode */
int i_Radio_Wakeup(void);

/* Force PLL calibration, used enabling manual calibration for ultra low power */
int i_Radio_Calibrate_On(void);

/* extract the frequency error estimate of the previous packet */
int i_Radio_Freq_Error(void);
