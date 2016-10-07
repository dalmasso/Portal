
/**
 ****************************************************
 *
 *	=============================================
 *				        PROJECT : PORTAL
 *	=============================================
 *	
 *	@author	DALMASSO LOIC
 *	@date	13.05.2016
 *	@brief	Portal Control System
 *
 *  - Using RF module (433MHz) to communicate with telecommand
 *  - Using relay module to activate portal command
 *  - Using OV7670 sensor to take photo
 *  - Using SD card to store image
 *  - Using IR module to detect person in front of portal
 *
 *
 ****************************************************
 */


/**
 *	*************
 *	Select Target
 *	*************
 */

//#define TINY85


// Global include
#include "Portal.h"


/**
 *	******************************************************
 * 					        TELECOMMAND - TINY85
 *	******************************************************
 */
#ifdef TINY85

// Flags interrupts : BP_POR    ENI/DIS PIR INT   BP_PHO
int INT_POR = 0;
int INT_PIR = 0;
int INT_PHO = 0;

void setup(void)
{  
  // Disable Interrupt
  GIMSK = 0b00000000;

  // GPIO Mode
  pinMode(LED_BIP, OUTPUT);
  pinMode(BP_POR, INPUT);
  pinMode(BP_PIR, INPUT);
  pinMode(BP_PHO, INPUT);

  // GPIO Init Value & PULL UP on inputs
  digitalWrite(LED_BIP, LOW);
  digitalWrite(BP_POR, HIGH);
  digitalWrite(BP_PIR, HIGH);
  digitalWrite(BP_PHO, HIGH);


  // Init TX RF
  Init_RF_TX(RF_TX);

  // Enable Pin Change Interrupts
  GIMSK = 0b00100000;

  // Select Pin Change Interrupts
  PCMSK = 0b00001101;
  
}


void loop(void)
{  
  // Select which interrupt set
  /*
   * INT_POR : Portal
   * INT_PIR : PIR INT (Enable / Disable)
   * INT_SND : Send
   */
  
  // Wait interrupts flags
  while( (!INT_POR) && (!INT_PIR) && (!INT_PHO) );

  // INT_POR : Portal
  if(INT_POR)
  {
    INT_POR = 0;
    Toggle_BIP(LED_BIP);
    RF_send("1");
  }

  // INT_PIR : PIR INT (Enable / Disable)
  if(INT_PIR)
  {
    INT_PIR = 0;
    Manage_PIR(LED_BIP); // 1: Enable PIR("ENI")    0: Disable PIR("DIS")
  }

  // INT_PHO : Take Photo
  if(INT_PHO)
  {
    INT_PHO = 0;
    Toggle_BIP(LED_BIP);
    RF_send("4");
  }     
}


ISR(PCINT0_vect)
{ 
  if(!digitalRead(BP_POR))
      INT_POR = 1;

  if(!digitalRead(BP_PIR))
      INT_PIR = 1;

  if(!digitalRead(BP_PHO))
      INT_PHO = 1;
}




/**
 *	******************************************************
 * 						        PORTAL - MEGA
 *	******************************************************
 */
#else

// Flag interrupt PIR :  1: Detec  0: No detect
int top_INT = 0;
int cpt_interrupt_PIR = 0;

void setup(void)
{  
  // Disable Interrupts
  detachInterrupt(digitalPinToInterrupt(PIR));
  
  // Init Arduino
  // GPIO Mode
  pinMode(LED, OUTPUT);
  pinMode(FLASH, OUTPUT);
  pinMode(OPEN_PORTAL, OUTPUT);
  pinMode(PIR, INPUT);
  pinMode(SD_CS,OUTPUT);
  
  // GPIO Init Value
  digitalWrite(LED, LOW);
  digitalWrite(FLASH, LOW);
  digitalWrite(OPEN_PORTAL, HIGH);
  digitalWrite(PIR, LOW);

  // Init Sensor
  Init_Sensor();
  
  // Init RX RF
  Init_RF_RX(RF_RX);

  // Enable External Interrupt PIR (INT2)
  attachInterrupt(digitalPinToInterrupt(PIR), Interrupt_PIR, RISING);
}


void loop(void)
{
  // Storage CMD RF
  uint8_t msg = 0;
  
  // CMD RF : Select Process
  /*
   * 1 : Portal   -> "1"
   * 2 : PIR INT  -> "2"  / Disable "3"
   * 3 : Send     -> "4"
   */
  
  // Receive CMD RF with timeout 500ms
  msg = RF_receive();
  
  // Start Interrupt Process
  if (top_INT)
  {
    // Process Interrupt
    Go_Photo(&top_INT,PIR);
    
    // Enable Interrupt
    attachInterrupt(digitalPinToInterrupt(PIR), Interrupt_PIR, RISING); 
  }

  // Start RF CMD
  switch(msg)
  {
    case '1':
         Open_portal(OPEN_PORTAL);
         break;
  
    case '2':
         attachInterrupt(digitalPinToInterrupt(PIR), Interrupt_PIR, RISING);
         break;
  
    case '3':
         Reset_INT(&cpt_interrupt_PIR,PIR);
         break;
  
    case '4':
         Go_Photo(&top_INT,PIR);
         attachInterrupt(digitalPinToInterrupt(PIR), Interrupt_PIR, RISING);
         break;
        
    // Reset msg
    msg = 0;
  }    
}



// ISR : PIR
void Interrupt_PIR(void)
{ 
  // Count number of detect
  cpt_interrupt_PIR++;

  if (cpt_interrupt_PIR == 4)
  {
    // Reset counter
    cpt_interrupt_PIR = 0;

    // Top interrupt
    top_INT = 1;
  }
}
#endif
