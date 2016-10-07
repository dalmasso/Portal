/******* ARDUINO COMMONS INCLUDES *******/
#include <avr/io.h>
#include <avr/interrupt.h>
 
/******* USER ATTINY85 DEFINES *******/
#ifdef TINY85
#define LED_BIP 1
#define BP_POR  0 // PCINT0
#define BP_PIR  2 // PCINT2
#define BP_PHO  3 // PCINT3
#define RF_TX   4
 
/******* USER ATMEGA DEFINES *******/
#else
#define PIR         19
#define RF_RX       2
#define OPEN_PORTAL 3
#endif;
 
/******* USER INCLUDES *******/
#include "RF.h"

#ifndef TINY85
#include "Photo.h"
#endif;

/******* PORTAL FUNCTIONS *******/
 
void Toggle_BIP(int pin);
void MyDelay(int x);
void Manage_PIR(int BIP);
void Bip_sound(int pin, int x);
void Open_portal(int pin);
