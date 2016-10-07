
#include "Portal.h"

// Functions Portal

void Toggle_BIP(int pin)
{
  digitalWrite(pin, HIGH);    // turn the LED
  MyDelay(100);              // wait for a second
  digitalWrite(pin, LOW);
}
 
 
 
void MyDelay(int x)
{
  for(int i=0; i<=x; i++)   
  {
    delayMicroseconds(1000);
  }
}
 
 
void Manage_PIR(int pin)
{
  static char state_PIR = 0;
 
  // Disable -> Enable PIR
  if(state_PIR == 0)
  {
    RF_send("2");
    state_PIR = 1;
    Bip_sound(pin, state_PIR);
  }
 
  // Enable -> Disable PIR
  else
  {
    RF_send("3");
    state_PIR = 0;
    Bip_sound(pin, state_PIR);
  }
}
 
 
void Bip_sound(int pin, int state)
{
   // 1 : Normal BIP : Enable PIR INT
   if(state)
      Toggle_BIP(pin);
 
   // 0 : 3 Bip : Disable PIR INT
   else
   {
      Toggle_BIP(pin);
      MyDelay(100);
      Toggle_BIP(pin);
      MyDelay(100);
      Toggle_BIP(pin);
   }
}
 
 
void Open_portal(int pin)
{
  digitalWrite(pin,LOW);
  MyDelay(2000);
  digitalWrite(pin,HIGH);
}