#include "ST7735.h"
#include <stdarg.h>
/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

//static void I2CBufferEventHandler(uintptr_t contextHandle);

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
unsigned char myData[100]={0};
unsigned char hello[]="Hello";

void delay(int ms){
    CORETIMER_DelayMs(ms);
}

void SPI_Write(unsigned char b){
//    while(SPI1_IsBusy()){}
    SPI1_Write(&b,1);
}

void LCDCommand(unsigned char cmd,...){
va_list ap;
va_start(ap, cmd);  
TFT_CS_Clear();
TFT_DC_Clear();
SPI_Write(cmd);
//SPI.transfer(cmd);
TFT_DC_Set();
int cnt=va_arg(ap, int);
for(int i=0;i<cnt;i++){
  unsigned char data=(unsigned char)va_arg(ap,int);
  SPI_Write(data);
}
TFT_CS_Set();
va_end(ap);
}

void LCDinit(){
LCDCommand(ST77XX_SWRESET,0);
delay(150);
LCDCommand(ST77XX_SLPOUT,0);
delay(500);
LCDCommand(ST7735_FRMCTR1,3,0x01, 0x2C, 0x2D);
LCDCommand(ST7735_FRMCTR2,3,0x01, 0x2C, 0x2D);
LCDCommand(ST7735_FRMCTR3,6,0x01, 0x2C, 0x2D,0x01, 0x2C, 0x2D);
LCDCommand(ST7735_INVCTR,1,0x07);
LCDCommand(ST7735_PWCTR1,3,0xA2,0x02,0x84);
LCDCommand(ST7735_PWCTR2,1,0xC5);
LCDCommand(ST7735_PWCTR3,2,0x0A,0x00);
LCDCommand(ST7735_PWCTR4,2,0x8A,0x2A);
LCDCommand(ST7735_PWCTR5,2,0x8A,0xEE);
LCDCommand(ST7735_VMCTR1,1,0x0E);
LCDCommand(ST77XX_INVOFF,0);
LCDCommand(ST77XX_MADCTL,1,0x48); //RGB+mirror
LCDCommand(ST77XX_COLMOD,1,TFT_COLOR); //0x05=16bit mode 0x06=18bit mode
LCDCommand(    ST77XX_CASET,   4,              //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x7F);
LCDCommand(ST77XX_RASET,   4,              //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x9F);     
LCDCommand(ST7735_GMCTRP1, 16      ,       //  1: Gamma Adjustments (pos. polarity), 16 args + delay:
      0x02, 0x1c, 0x07, 0x12,       //     (Not entirely necessary, but provides
      0x37, 0x32, 0x29, 0x2d,       //      accurate colors)
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10);

LCDCommand(ST7735_GMCTRN1, 16      ,       //  2: Gamma Adjustments (neg. polarity), 16 args + delay:
      0x03, 0x1d, 0x07, 0x06,       //     (Not entirely necessary, but provides
      0x2E, 0x2C, 0x29, 0x2D,       //      accurate colors)
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10);
    
LCDCommand(ST77XX_NORON,0);
delay(10);
LCDCommand(ST77XX_DISPON,0);   
}


void LCDFill(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char r,unsigned char g,unsigned char b){
    unsigned char byte[3];
LCDCommand(ST77XX_CASET,4,0,x1,0,x2);    
LCDCommand(ST77XX_RASET,4,0,y1,0,y2); 
TFT_CS_Clear();
TFT_DC_Clear();
//unsigned char 
SPI_Write(ST77XX_RAMWR);
TFT_DC_Set();
  if(TFT_COLOR == TFT_16bit){
      
      byte[0]=RGB16h(r,g,b);
      byte[1]=RGB16l(r,g,b);
//  SPI1_Write(byte,2);
  }else{
      byte[0]=r;
      byte[1]=g;
      byte[2]=b;
 //     SPI1_Write(byte,3);
  }
for(long i=0;i<(long)(x2-x1+1)*(long)(y2-y1+1);i++){
  if(TFT_COLOR == TFT_16bit){
  SPI1_Write(byte,2);
  }else{
      SPI1_Write(byte,3);
  }

}
TFT_CS_Set();
}

void LCDimage(){
        int pos=0;


        LCDCommand(ST77XX_CASET,4,0,0,0,127);    
        LCDCommand(ST77XX_RASET,4,0,0,0,159); 
        TFT_CS_Clear();
        TFT_DC_Clear();
         SPI_Write(ST77XX_RAMWR);
        TFT_DC_Set();  
        I2C1_Write(0x57,(unsigned char*)&pos,2);
        while(I2C1_IsBusy( )){}
for(int j=0;j<160;j++){  
    for(int i=0;i<128;i++){

        I2C1_Read( 0x57,&myData[0], 3 );
       while(I2C1_IsBusy( )){}
        //unsigned char 
        unsigned char buf[3];
        buf[0]=myData[2];
        buf[1]=myData[1];
        buf[2]=myData[0];
        
        SPI1_Write(buf,3);

    }
}
        TFT_CS_Set();
}


void set_register(unsigned char addr,unsigned char data){
unsigned char buf[2]={0};
buf[0]=addr;
buf[1]=data;
I2C1_Write( 0x08, &buf[0], 2 );
while(I2C1_IsBusy( )){}
}

void SetFrequency(int ch, long freq) {
  long cal_freqency = 0;
  if (freq != 0) {
    cal_freqency = 125000 / freq;
  }
  cal_freqency &= 0b0000111111111111;
  set_register(0x00 + (ch * 2), cal_freqency & 0xff);
  set_register(0x01 + (ch * 2), (cal_freqency >> 8) & 0xff);
}


int main ( void )
{


    /* Initialize all modules */
    SYS_Initialize ( NULL );
 //   I2C1_CallbackRegister(I2CBufferEventHandler, 0);

    TFT_RST_Set();
    LED_Set();
    delay(100);
    TFT_RST_Clear();
    LED_Clear();
    delay(100);
    TFT_RST_Set();   
    LED_Set();
    
//    while(I2C1_IsBusy( )){}
//   while(I2C2_IsBusy( )){}
//    I2C2_Write( 0x08, &myData[0], 32 );
    while(I2C1_IsBusy( )){}
    set_register(0xff, 0);


    LCDinit();
    LCDFill(0,0,128,160,0,0,0);
    delay(1000);
    LCDimage();
 //   LED_Set();
        delay(1000);
//???LCD????????????? 
set_register(0x07, 0b00111000);
  set_register(0x08, 0);
  set_register(0x09, 0);
  set_register(0x0a, 0);
  SetFrequency(0, 0);
  SetFrequency(1, 0);
  SetFrequency(2, 0);

  delay(100);

    
    while ( true )
    {
  SetFrequency(0, 440);
    set_register(0x08,15);  
    delay(1000);
    set_register(0x08,0);  
    delay(1000);
  SetFrequency(0, 220);
    set_register(0x08,15);  
    delay(1000);
    set_register(0x08,0);  
    delay(1000);
    
    //  LCDFill(0,0,128,159,0,0,0);
  //  delay(1000);
  //  LCDFill(0,0,128,159,255,0,0);
  //  delay(1000);
  //  LCDFill(0,0,128,159,0,255,0);
  //  delay(1000);
  //  LCDFill(0,0,128,159,0,0,255);
  //  delay(1000);

    /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

/*******************************************************************************
 End of File
*/

