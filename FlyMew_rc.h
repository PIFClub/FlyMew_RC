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
 *  Module      : FlyMew_rc.h
 *  Description : Header's RC for FLyMew platform
 *  Version     : 1.0
 *
 *  Author    : FW.Team
 *  Notes   :
 *
 *****************************************************************************/

/******************************************************************************
 * DEFINITIONS
 *****************************************************************************/
#define DCO_MULT_1MHZ           30
#define DCO_MULT_4MHZ           122
#define DCO_MULT_8MHZ           244
#define DCO_MULT_12MHZ          366
#define DCO_MULT_16MHZ          488
#define DCO_MULT_20MHZ          610
#define DCO_MULT_25MHZ          763

#define DCORSEL_1MHZ            DCORSEL_2
#define DCORSEL_4MHZ            DCORSEL_4
#define DCORSEL_8MHZ            DCORSEL_4
#define DCORSEL_12MHZ           DCORSEL_5
#define DCORSEL_16MHZ           DCORSEL_5
#define DCORSEL_20MHZ           DCORSEL_6
#define DCORSEL_25MHZ           DCORSEL_7

// CC State machine definition
#define CC_BOOT        1
#define CC_IDLE        2
#define CC_SETUP       3
#define CC_TX_ACTIVE   4
#define CC_RX_ACTIVE   5
#define CC_RF_ACTIVE   6

// Memu system state machine variables
#define MENU_SETUP           20
#define MENU_RESTART         21
#define MENU_IDLE            22
#define MENU_RF_IDLE         23
#define MENU_TX_UNMOD        24
#define MENU_TX_nMCU         25
#define MENU_TX_wMCU         26
#define MENU_RX_STATIC       27
#define MENU_RX_SINGLE       28
#define MENU_RX_SNIFF        29
#define MENU_TX_BER          30
#define MENU_RX_BER          31
#define MENU_TX_FHSS_BER     32
#define MENU_RX_FHSS_BER     33


// Buffer sizes
#define UART_BUFF_SIZE 80
#define TX_BUFF_SIZE   64
#define RSSI_OFFSET    72

typedef struct trx_cfg {
  unsigned char trx_mode;            // mode of operation in progress
  unsigned char cc_state;            // lastest state of CC device
  unsigned char cc_device_id;        // ID from CC device (is it a CC1101 or CC2500)
  unsigned char smartrf_selector;    // Select Low, Mid or high data rate pre-config
  unsigned long start_freq;          // start frequency for all tasks
  unsigned char rf_channel;          // current RF channel
  unsigned int  ch_spc;              // channel spacing to be used by CC device
  unsigned int  no_burst;            // number of TX burst requested
  unsigned int  tx_pwr;              // actual TX power to be used
  unsigned long  bit_rate;            // TRX bitrate
  unsigned char b_length;            // burst length in bytes.
  unsigned int  packet_timer;
  unsigned int  normal_wait_timer;
  unsigned int  max_wait_timer;
  unsigned int  sync_timer;
  unsigned int  xtal_wait_timer;
} trx_cfg_struct;

/******************************************************************************
 * PROTOTYPES
 *****************************************************************************/
void v_RF_Default_Setup(trx_cfg_struct *trx_cfg);
void v_TX_Ber_Single(unsigned char *txBuffer, trx_cfg_struct *trx_cfg);
int v_RX_Ber_Single(unsigned char *txBuffer, trx_cfg_struct *trx_cfg);
int16_t s16_Validate_Package(void);

/******************************************************************************
 * END OF FlyMew_rc.h
 *****************************************************************************/
