#include <Wire.h>
#include "Arduino.h"
#include <grideye.h>
#include <GE_SoftUart.h>
#include <GE_SoftUartParse.h>


const int LedPin = LED_BUILTIN;
const int RelayPin = 33;  


/*******************************************************************************
  variable value definition
*******************************************************************************/
static uint16_t Main_Delay = 85;
       grideye  GE_GridEyeSensor;
       uint8_t  aucThsBuf[2];              /* thermistor temperature        */
       short    g_ashRawTemp[64];          /* temperature of 64 pixels      */
  

char buffer[32];


/******************************************************************************
  Function：GE_SentDatatoPC
  Description：Used to Sent data to PC
  Input：None
  Output：None
  Others：None
******************************************************************************/

int numOfPointsOverThreshold = 0;
bool lastLightStatus = LOW;


bool getLightStatus()
{
      numOfPointsOverThreshold = 0;

      for( int i = 0; i < 64; i++ )
      {
          float temp = (float)(g_ashRawTemp[i]) * 0.25;

          char c = ' ';

          if (temp > 25.0)
          {
            numOfPointsOverThreshold++;
          }

          if (numOfPointsOverThreshold > 12)
          {
            return HIGH;
          }
       }
       return LOW;
}

void setLight(bool status)
{
     digitalWrite(LedPin, status);  
     digitalWrite(RelayPin, status);  
}

void SendDataToPC( void )
{
      numOfPointsOverThreshold = 0;
      Serial.println("-----------------");
      for( int i = 0; i < 64; i++ )
      {
          //Serial.write(*((uint8_t *)(g_ashRawTemp)+i));

          float temp = (float)(g_ashRawTemp[i]) * 0.25;

          char c = ' ';

          if (temp > 23.0 && temp <= 30.0)
          {
            c = '.';
          }
          else if (temp > 30.0)
          {
            c = '*';
          }

          sprintf(buffer, "%c ", c);
          Serial.print(buffer);
          
          //sprintf(buffer, "%.2f ", (float)(g_ashRawTemp[i]) * 0.25);
          //sprintf(buffer, "%.2f ", (float)*((uint8_t *)(g_ashRawTemp)+i) * 0.25);
          //Serial.print(buffer);
          if ( (i+1) % 8 == 0)
          {
            Serial.print("|\r\n");
          }
       }
       Serial.println("-----------------");
}

/******************************************************************************
  Function：GE_SentDatatoPC
  Description：Used to Sent data to PC
  Input ：None
  Output：None
  Others：None 
******************************************************************************/
void GE_SourceDataInitialize( void )
{
  for ( int i = 0; i < 64; i++ )
  {
    g_ashRawTemp[i] = 0xAAAA;
  }
}
/******************************************************************************
  Function：GE_UpdateFerquency
  Description：Used to set the Grid-EYE update frequency
  Input：None
  Output：Grid-EYE frequency
  Return：None
  Others：None
******************************************************************************/
void GE_UpdateFerquency( uint8_t GE_SetFrequency )
{
    switch (GE_SetFrequency)
    {
        case 10:  /* set update frequency 10Hz */
        {
            Main_Delay = 85;
        }
        break;
      
        case 1:   /* set update frequency 1Hz */
        {       
            Main_Delay = 985;
        }
        break;
      
        default:
        break;
      }
}



void setup()
{
    /* Waiting for BLE Start to finish */
    PIOA->PIO_MDER = 0x00000200;
    delay(1000);

    /* start serial port at 57600 bps:*/
//    Serial.begin(57600);
  
    /* Initialize Grid-Eye data interface */
    GE_GridEyeSensor.init( 0 );

    /* Initialize variables  */ 
    GE_SourceDataInitialize( );

    /* Initialize software Software serial port UART1*/
    GE_SoftUartInit( );

    pinMode(LedPin, OUTPUT); 
    pinMode(RelayPin, OUTPUT); 

    setLight(LOW);
}

void loop()
{
    /* Parse of the latest PC sent command */
    GE_CmdParse();
    
    /* Get thermistor register value. */
    GE_GridEyeSensor.bAMG_PUB_I2C_Read(0x0E, 2, aucThsBuf );

    /* Get temperature register value. */
    for(int i=0;i<4;i++)
    {
        GE_GridEyeSensor.bAMG_PUB_I2C_Read(0x80+32*i, 32, (uint8_t *)g_ashRawTemp+i*32);
    }

     /* Send Grid-Eye sensor data to PC */   
//     SendDataToPC();

     bool status = getLightStatus();

     if (lastLightStatus != status)
     {
         setLight(status);
         lastLightStatus = status;

         unsigned long timeOutMs = (status==HIGH) ? 2500 : 1500;

         delay(timeOutMs); // avoid relay flickering
     }
 
     //Serial.println(status == LOW ? "LOW" : "HIGH");

     /* set update frequency */
     GE_UpdateFerquency(GE_UpdateFreGet());

     delay( Main_Delay );
}
