
#include <VirtualWire.h>

/******* EMETTEUR_RECEPTEUR RF *******/
 
// Prototypes
void Init_RF_TX(const int TX);
void RF_send(char* order);
void Init_RF_RX(const int RX);
uint8_t RF_receive(void);
