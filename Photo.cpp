
#include "Photo.h"

// Functions Photo
 
// Toggle LED Error (ON)
int Error(void)
{
  digitalWrite(LED,HIGH);
  return -1;
}
 
// Toggle LED Error (OFF)
void NoError(void)
{
  digitalWrite(LED,LOW);
}
 
// Toggle Flash LED
void Flash(int state)
{
  digitalWrite(FLASH,state);
}
 
// Comm OV7670 Protocol
void twiStart(void){
  TWCR=_BV(TWINT)| _BV(TWSTA)| _BV(TWEN);//send start
  while(!(TWCR & (1<<TWINT)));//wait for start to be transmitted
  if((TWSR & 0xF8)!=TW_START)
    Error();
}
 
void twiWriteByte(uint8_t DATA, uint8_t type){
  TWDR = DATA;
  TWCR = _BV(TWINT) | _BV(TWEN);    /* clear interrupt to start transmission */
  while (!(TWCR & (1<<TWINT)));   /* wait for transmission */
  if ((TWSR & 0xF8) != type)
    Error();
}
 
 
void wrReg(uint8_t reg, uint8_t dat){
  //send start condition
  twiStart();
  twiWriteByte(OV7670_I2C_ADDRESS<<1,TW_MT_SLA_ACK);
  twiWriteByte(reg,TW_MT_DATA_ACK);
  twiWriteByte(dat,TW_MT_DATA_ACK);
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
  _delay_ms(1);
}
 
static uint8_t twiRd(uint8_t nack){
  if (nack){
    TWCR=_BV(TWINT) | _BV(TWEN);
    while ((TWCR & _BV(TWINT)) == 0); /* wait for transmission */
    if ((TWSR & 0xF8) != TW_MR_DATA_NACK)
      Error();
  }else{
    TWCR=_BV(TWINT) | _BV(TWEN) | _BV(TWEA);
    while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
    if ((TWSR & 0xF8) != TW_MR_DATA_ACK)
      Error();
  }
  return TWDR;
}
 
uint8_t rdReg(uint8_t reg){
  uint8_t dat;
  twiStart();
  twiWriteByte(OV7670_I2C_ADDRESS<<1,TW_MT_SLA_ACK);
  twiWriteByte(reg,TW_MT_DATA_ACK);
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
  _delay_ms(1);
  twiStart();
  twiWriteByte((OV7670_I2C_ADDRESS<<1)|1,TW_MR_SLA_ACK);
  dat=twiRd(1);
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
  _delay_ms(1);
  return dat;
}
 
void wrSensorRegs8_8(const struct config reglist[]){
  const struct config *next = reglist;
  for(;;){
    uint8_t reg_addr = pgm_read_byte(&next->reg);
    uint8_t reg_val = pgm_read_byte(&next->data);
    if((reg_addr==255)&&(reg_val==255))
      break;
    wrReg(reg_addr, reg_val);
    next++;
  }
}
 
 
// Reset FIFO
void Reset_FIFO(void)
{
  digitalWrite(FIFO_WREN, LOW);  // Disable Write
  digitalWrite(FIFO_WRST, LOW);  // Reset Write pointer
  digitalWrite(FIFO_RRST, LOW);  // Reset Read pointer
  digitalWrite(FIFO_RCLK, LOW);  // Init CLK Read LOW
  delayMicroseconds(31);         // Wait
  digitalWrite(FIFO_RCLK,HIGH);  // Rising edge RCLK
  delayMicroseconds(6);          // Wait
  digitalWrite(FIFO_RRST, HIGH); // No Reset Read pointer (Enable Read on RCLK)
  digitalWrite(FIFO_RCLK,LOW);   // Falling edge RCLK
}
 
 
// Enable Write FIFO
void Enable_Write_FIFO(void)
{
  digitalWrite(FIFO_WREN, HIGH);   // Enable Write
  digitalWrite(FIFO_WRST, HIGH);   // No Reset Write pointer
}
 
// Disable Write FIFO
void Disable_Write_FIFO(void)
{
  digitalWrite(FIFO_WREN, LOW);   // Disable Write
  delay(100);
}
 
// Init OV7670
void Init_Sensor(void)
{
  // GPIO Mode
  pinMode(CAM_HREF,INPUT);
  pinMode(CAM_VSYNC,INPUT);
  pinMode(CAM_DOUT0,INPUT);
  pinMode(CAM_DOUT1,INPUT);
  pinMode(CAM_DOUT2,INPUT);
  pinMode(CAM_DOUT3,INPUT);
  pinMode(CAM_DOUT4,INPUT);
  pinMode(CAM_DOUT5,INPUT);
  pinMode(CAM_DOUT6,INPUT);
  pinMode(CAM_DOUT7,INPUT);
  pinMode(FIFO_RCLK,OUTPUT);
  pinMode(FIFO_WREN,OUTPUT);
  pinMode(FIFO_WRST,OUTPUT);
  pinMode(FIFO_RRST,OUTPUT);
 
  // Reset FIFO
  Reset_FIFO();
 
  // Setup TWI communication (100KHz)
  TWBR = 72;
   
  // Init capteur
  wrReg(OV_REG_COM7, 0x80);
  _delay_ms(100);
  wrSensorRegs8_8(Config_OV7670);
  wrReg(OV_REG_COM10, 0);//PCLK toggle at any time.
 
  // Set Resolution
  wrReg(OV_REG_COM3, 0);
  wrSensorRegs8_8(VGA_OV7670);
 
  // Set Color
  wrSensorRegs8_8(BayerRGB_OV7670);
  wrReg(OV_REG_CLKRC, 38);
}
 
 
// Reset Interruption
void Reset_INT(int* cpt_INT, int pin_INT)
{
  detachInterrupt(digitalPinToInterrupt(pin_INT));
  *cpt_INT = 0;
}
 
 
// Capture IMG
int captureImg(File myFile)
{ 
  byte data;
   
  while (!(PINC & 128));  // Wait Vsync High : star of image
  while (PINC & 128);     // wait Vsync Low
 
  Enable_Write_FIFO();    // Enable Write FIFO to store image
 
  while (!(PINC & 128));  // Wait Vsync High : end of image
 
  Disable_Write_FIFO();   // Disable FIFO to read image
  
  // Read FIFO & store in SD card
  for(long pxl=0;pxl<307200;pxl++)
  {     
    // Generate RCLK
    digitalWrite(FIFO_RCLK,HIGH);
    data = PINA;
    digitalWrite(FIFO_RCLK,LOW);
 
    for(int i=0;i<3;i++)
    {
      if ( !(myFile.write(data) >0) )
        return -1;
    }
 
  }
  return 0;
}
 
 
// Function Launch Photo
int Take_Photo(void)
{
    File myFile;
    static int Numb_img = 0;
    char SD_File[15];
     
     // Init & Open SD File
    if(!SD.begin(SD_CS))
      return Error();
 
    sprintf(SD_File, "CAM/image%d.bmp", Numb_img);
    if( !(myFile = SD.open(SD_File, FILE_WRITE)) )
      return Error();
 
    // Write Header BMP Image
    if( !(myFile.write(header,sizeof(header)) > 0) )
      return Error();
      
    // Start & Storage Photo
    if(captureImg(myFile) == -1)
      return Error();
 
    // Close SD File
    myFile.close();
    // Increment File Name
    Numb_img++;
 
    // reset Error
    NoError();
}
 
 
// Function Photo All Process
void Go_Photo(int* flag_INT, int pin_INT)
{
   // Disable Interrupt & Reset interrupt flag
   detachInterrupt(digitalPinToInterrupt(pin_INT));
   *flag_INT = 0;
     
   // Enable Flash
   //Flash(HIGH);
   
   // Take Photo
   if(!Take_Photo())
     Error();
      
   // Disable Flash
   //Flash(LOW);
}
