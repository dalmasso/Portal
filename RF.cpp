
#include "RF.h"

// Functions RF

void Init_RF_TX(const int TX)
{
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_set_tx_pin(TX);
    vw_setup(2000);
}
 
 
void RF_send(char* order)
{
    vw_send((uint8_t *)order, strlen(order));
    vw_wait_tx(); // Wait until the whole message is go
}
 
 
 
void Init_RF_RX(const int RX)
{
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_set_rx_pin(RX);
    vw_setup(2000);
    vw_rx_start();
}
 
 
uint8_t RF_receive(void)
{
    uint8_t buf[1];
    uint8_t buflen = 1;
 
    if (!vw_wait_rx_max(500))
      return -1;
    else
    {
      if (vw_get_message(buf, &buflen))
        return buf[0]; // if msg good
      else
        return -1;  // if false msg    
    }
 
}
