/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iwdg.h"
#include "usart.h"
#include "gpio.h"
#include "sheduler.h"
#include "crc16.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
  union{
    uint16_t value;
    uint8_t byte[2];
  }crc; 
/* USER CODE END 0 */
extern uint8_t HeaderFirst    ;
extern uint8_t cmdReceive     ;
extern uint8_t cmdTransmite   ;
extern uint8_t latch[20];
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

extern tStateUsart stUsart1;
extern tStateTransmite mStateTransmite;
extern tStateReseive mStateReseive;
tStrZoomHot ZoomHot;
tStrMide    mide;
uint8_t tempzoom = 0x64;
extern uint8_t pinSt[];

tStateParam LuchParam;
/* USER CODE END PD */
  int8_t encarry[2];

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
  tStateReseive state = {
  .state_heating_1 = DISABLE,
  .state_heating_2 = ENABLE,
  .state_cooler = DISABLE,
  .state_wiper = DISABLE,
};
//TransmitData
  tStateTransmite TransmitData= {
        .temperature    =0,
        .temperature_1  =0, 
};
/* USER CODE END PM */
  int8_t temp = 0;
  int8_t tempmide = 0;
  int8_t countmide = 0;
/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* ZOOM */  
uint8_t Zoom1[13]={0x10,0x02,0xF4,0x02,0x00,0x20,0x00,0x00,0x64,0x10,0x03,0x7E,0x72};
uint8_t Zoom2[13]={0x10,0x02,0xF4,0x02,0x00,0x20,0x00,0x00,0xC8,0x10,0x03,0x0A,0x14};
uint8_t Zoom4[13]={0x10,0x02,0xF4,0x02,0x00,0x20,0x00,0x01,0x90,0x10,0x03,0xE2,0xD8};
uint8_t ZoomTest[18]={0x10,0x02,0xF4,0x02,0x01,0x20,0x00,0x03,0x00,0x01,0x90,0x80,0x00,0x00,0x10,0x03,0xC4,0xD9};
//uint8_t Calibration[18]={0x10,0x02,0xF4,0x02,0x01,0x20,0x00,0x03,0x00,0x01,0x90,0x80,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t Calibration[18]={0x10,0x02,0xF4,0x02,0x01,0x00,0x00,0x03,0x00,0x00,0x64,0x80,0x00,0x00,0x10,0x03,0xE2,0xD8};

uint8_t WhHot[13]={0x10,0x02,0xF4,0x02,0x20,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t BlHot[13]={0x10,0x02,0xF4,0x02,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};

//New protocol
uint8_t FocusPls[14]={0x10,0x02,0x41,0x02,0x04,0x00,0x1F,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t FocusMns[14]={0x10,0x02,0x41,0x02,0x04,0x00,0x20,0xFF,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t FocusStp[14]={0x10,0x02,0x41,0x02,0x04,0x00,0x20,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t AutoFocus[14]={0x10,0x02,0x41,0x02,0x04,0x00,0x40,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};



////Old protocol
//uint8_t FocusPls[14]={0x10,0x02,0x41,0x02,0x00,0x00,0x00,0x60,0x00,0x00,0x10,0x03,0xE2,0xD8};
//uint8_t FocusMns[14]={0x10,0x02,0x41,0x02,0x00,0x00,0x00,0x20,0x00,0x00,0x10,0x03,0xE2,0xD8};
//uint8_t FocusStp[14]={0x10,0x02,0x41,0x02,0x00,0x00,0x00,0x4A,0x00,0x00,0x10,0x03,0xE2,0xD8};

uint8_t MIDE00[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE01[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE02[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE03[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE05[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE06[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE04[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE07[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE08[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE09[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE10[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE11[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x0B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE12[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE13[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x0D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE14[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t MIDE15[22]={0x10,0x02,0xF7,0x02,0x00,0x00,0x00,0x02,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};

/* BRIGHIT */
uint8_t BRIGHIT00[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t BRIGHIT01[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x22,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t BRIGHIT02[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t BRIGHIT03[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t BRIGHIT04[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x88,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t BRIGHIT05[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAA,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t BRIGHIT06[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t BRIGHIT07[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEE,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
//uint8_t BRIGHIT08[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xB0,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
//uint8_t BRIGHIT09[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xC6,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
//uint8_t BRIGHIT10[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xDC,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
//uint8_t BRIGHIT11[22]={0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xF2,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};

/* CONTRAST */
uint8_t CONTR00[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t CONTR01[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x22,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t CONTR02[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8}; 
uint8_t CONTR03[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t CONTR04[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x88,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t CONTR05[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAA,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t CONTR06[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
uint8_t CONTR07[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEE,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
//uint8_t CONTR08[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xB0,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
//uint8_t CONTR09[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
//uint8_t CONTR10[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDC,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};
//uint8_t CONTR11[22]={0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF2,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8};


/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
typedef enum
{
  UART_RX_IDLE,

  UART_RX_HEADER,
  UART_RX_DATA0,
  UART_RX_DATA1,
  UART_RX_DATA2,
  UART_RX_DATA3,
  UART_RX_DATA4,
  UART_RX_DATA5,
  UART_RX_DATA6,
  UART_RX_DATA7,   
  UART_RX_CRC,
} uart_rx_state_t;


typedef enum
{
  UART_START,
  UART_END,
} uart_bug_state_t;

/* USER CODE END PFP */
tStateReseive g_state ;//= {0}
uart_bug_state_t bug_state = UART_START;
uart_rx_state_t rx_state = UART_RX_IDLE;

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint8_t uart_rx_byte = 0;
/* USER CODE END 0 */

void initZoomHotstr(tStrZoomHot *str)
{
  str->dle1     = 0x10;
  str->stx      = 0x02;
  str->mid      = 0xf4;
  str->stm      = 0x02;
  str->act1     = 0x00;
  str->act2     = 0x00;
  str->rsrv     = 0x00;
  str->fovh     = 0x00;
  str->fovl     = 0x00;
  str->dle2     = 0x10;
  str->etx      = 0x03;
  str->crc1     = 0x00;
  str->crc2     = 0x00;
}



void initMidestr(tStrMide *str)
{
  str->dle1  = 0x10;
  str->stx   = 0x02;
  str->mid   = 0xF7;
  str->stm   = 0x02;
  str->act1  = 0x00;
  str->act2  = 0x00;
  str->act3  = 0x00;
  str->act4  = 0x02;
  str->act5  = 0x00;
  str->act6  = 0x00;
  str->act7  = 0x00;
  str->act8  = 0x00;
  str->act9  = 0x00;
  str->act10 = 0x00;
  str->act11 = 0x00;
  str->act12 = 0x00;
  str->act13 = 0x00;
  str->act14 = 0x00;
  str->dle2  = 0x10;
  str->etx   = 0x03;
  str->crc1  = 0x00;
  str->crc2  = 0x00;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  switch(rx_state)
  {
  case UART_RX_HEADER:
    if(uart_rx_byte == HeaderFirst)
    {
      rx_state = UART_RX_DATA0;
      mStateTransmite.header = uart_rx_byte;
      uart_rx_byte = 0;
    }
    break;
    
    case UART_RX_DATA0:
      
      rx_state = UART_RX_DATA1;
      mStateTransmite.temperature = uart_rx_byte;
      uart_rx_byte = 0;      

    break;
    
    case UART_RX_DATA1:

      rx_state = UART_RX_DATA2;
      mStateTransmite.temperature |= uart_rx_byte<<8;
      uart_rx_byte = 0;
      
    break;       

    case UART_RX_DATA2:

      rx_state = UART_RX_DATA3;
      mStateTransmite.temperature_1 = uart_rx_byte;
      uart_rx_byte = 0;
    break;
    
    case UART_RX_DATA3:

      rx_state = UART_RX_DATA4;
      mStateTransmite.temperature_1 |= uart_rx_byte<<8;
      uart_rx_byte = 0;
    break;       

    
    case UART_RX_DATA4:
         if((uart_rx_byte == 0x33)||(uart_rx_byte == 0x81))
    {
      rx_state = UART_RX_DATA5;
      mStateTransmite.state_heating_1 = uart_rx_byte;
      uart_rx_byte = 0;
    }

    break;

    case UART_RX_DATA5:
        if((uart_rx_byte == 0x33)||(uart_rx_byte == 0x81))
    {
      rx_state = UART_RX_DATA6;
      mStateTransmite.state_heating_2 = uart_rx_byte;
      uart_rx_byte = 0;      
    }
    
    break;

    case UART_RX_DATA6:
     if((uart_rx_byte == 0x33)||(uart_rx_byte == 0x81))
    {
      rx_state = UART_RX_DATA7;
      mStateTransmite.state_cooler = uart_rx_byte;
      uart_rx_byte = 0;
    }

    break;
    
    case UART_RX_DATA7:
     if((uart_rx_byte == 0x33)||(uart_rx_byte == 0x81))
    {      
      rx_state = UART_RX_CRC;
      mStateTransmite.state_wiper = uart_rx_byte;
      uart_rx_byte = 0;
    }
    break;    

    case UART_RX_CRC:
      rx_state = UART_RX_HEADER;
      mStateTransmite.crc = uart_rx_byte;
      uart_rx_byte = 0;
    break;

    default:
    rx_state = UART_RX_HEADER;

    break;
 
  }
  HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart_rx_byte, 1);
}

void FocusEnc(void)
{
  if(encarry[1]>temp)
  {
       crc.value=crc16_ccitt( (uint8_t *)&FocusPls + 2, 8 );
       FocusPls[12] = crc.byte[1];
       FocusPls[13] = crc.byte[0];          
       HAL_UART_Transmit(&huart1, (uint8_t*)&FocusPls, 14, 50); 
       temp = encarry[1];
       latch[15]=HI;
  }else if(encarry[1]<temp)
  {
       crc.value=crc16_ccitt( (uint8_t *)&FocusMns + 2, 8 );
       FocusMns[12] = crc.byte[1];
       FocusMns[13] = crc.byte[0];        
       HAL_UART_Transmit(&huart1, (uint8_t*)&FocusMns, 14, 50); 
       temp = encarry[1];
       latch[15]=HI;       
  }
  if((encarry[1]==temp)&&(latch[15]==HI))
  {
       crc.value=crc16_ccitt( (uint8_t *)&FocusStp + 2, 8 );
       FocusStp[12] = crc.byte[1];
       FocusStp[13] = crc.byte[0];        
       HAL_UART_Transmit(&huart1, (uint8_t*)&FocusStp, 14, 50);      
       temp = encarry[1];       
       latch[15]=LOW;
  }  
}





void MIDEEnc(void)
{
    if(encarry[0]>tempmide)
  {
       mide.act5 = countmide;
       crc.value=crc16_ccitt( (uint8_t *)&mide + 2, 16 );
       mide.crc1 = crc.byte[1];
       mide.crc2 = crc.byte[0];        
       HAL_UART_Transmit(&huart1, (uint8_t*)&mide, sizeof(mide), 50);   
       tempmide = encarry[0];
       if(countmide<0x0F) countmide++;
  }else if(encarry[0]<tempmide)
  {
       mide.act5 = countmide;    
       crc.value=crc16_ccitt( (uint8_t *)&mide + 2, 16 );
       mide.crc1 = crc.byte[1];
       mide.crc2 = crc.byte[0];        
       HAL_UART_Transmit(&huart1, (uint8_t*)&mide, sizeof(mide), 50); 
       tempmide = encarry[0];
       if(countmide>0) countmide--;
    
  }

}

void KnobFunk(void){
      if((pinSt[2] == ON) & (latch[1]==LOW))
    {
      if(tempzoom == 0x90){
       ZoomHot.act2 = 0x20;
       ZoomHot.fovh = 0x01;       
       ZoomHot.fovl = 0x90;
       crc.value=crc16_ccitt( (uint8_t*)&ZoomHot + 2, 7 );
       ZoomHot.crc1 = crc.byte[1];
       ZoomHot.crc2 = crc.byte[0];
       HAL_UART_Transmit(&huart1, (uint8_t*)&ZoomHot, sizeof(ZoomHot), 50);         
       LuchParam.Zoom = Zoom4x;
       tempzoom = 0x90;
       latch[1]=HI;      
    HAL_Delay(10);              
      }       
      
      if(tempzoom == 0xC8){
       ZoomHot.act2 = 0x20;
       ZoomHot.fovh = 0x01;       
       ZoomHot.fovl = 0x90;
       crc.value=crc16_ccitt( (uint8_t*)&ZoomHot + 2, 7 );
       ZoomHot.crc1 = crc.byte[1];
       ZoomHot.crc2 = crc.byte[0];
       HAL_UART_Transmit(&huart1, (uint8_t*)&ZoomHot, sizeof(ZoomHot), 50);         
       LuchParam.Zoom = Zoom4x;
       tempzoom = 0x90;
       latch[1]=HI;
    HAL_Delay(10);       
      }
      if(tempzoom == 0x64){
       ZoomHot.act2 = 0x20;
       ZoomHot.fovh = 0x00;       
       ZoomHot.fovl = 0xC8;  
       crc.value=crc16_ccitt( (uint8_t*)&ZoomHot + 2, 7 );
       ZoomHot.crc1 = crc.byte[1];
       ZoomHot.crc2 = crc.byte[0];
       HAL_UART_Transmit(&huart1, (uint8_t*)&ZoomHot, sizeof(ZoomHot), 50);         
       LuchParam.Zoom = Zoom4x;
       tempzoom = 0xC8;
       latch[1]=HI;       
    HAL_Delay(10);              
      }            
    
      if(tempzoom == 0x00){
       ZoomHot.act2 = 0x20;
       ZoomHot.fovh = 0x00;       
       ZoomHot.fovl = 0x64;
       crc.value=crc16_ccitt( (uint8_t*)&ZoomHot + 2, 7 );
       ZoomHot.crc1 = crc.byte[1];
       ZoomHot.crc2 = crc.byte[0];
       HAL_UART_Transmit(&huart1, (uint8_t*)&ZoomHot, sizeof(ZoomHot), 50);         
       LuchParam.Zoom = Zoom4x;
       tempzoom = 0x64;       
       latch[1]=HI;       
    HAL_Delay(10);              
      }        
       latch[1]=HI;
       latch[2]=HI;
    }

    if((pinSt[10] == ON) & (latch[10]==LOW))
    {
      if(tempzoom == 0x64){
       ZoomHot.act2 = 0x20;
       ZoomHot.fovh = 0x00;       
       ZoomHot.fovl = 0x64;  
       crc.value=crc16_ccitt( (uint8_t*)&ZoomHot + 2, 7 );
       ZoomHot.crc1 = crc.byte[1];
       ZoomHot.crc2 = crc.byte[0];
       HAL_UART_Transmit(&huart1, (uint8_t*)&ZoomHot, sizeof(ZoomHot), 50);         
       LuchParam.Zoom = Zoom4x;
       tempzoom = 0x64;
       latch[1]=HI;       
           HAL_Delay(10);       
      }       
      
      if(tempzoom == 0xC8){
       ZoomHot.act2 = 0x20;
       ZoomHot.fovh = 0x00;       
       ZoomHot.fovl = 0x64;
       crc.value=crc16_ccitt( (uint8_t*)&ZoomHot + 2, 7 );
       ZoomHot.crc1 = crc.byte[1];
       ZoomHot.crc2 = crc.byte[0];
       HAL_UART_Transmit(&huart1, (uint8_t*)&ZoomHot, sizeof(ZoomHot), 50);         
       LuchParam.Zoom = Zoom4x;
       tempzoom = 0x64;       
       latch[1]=HI;       
           HAL_Delay(10);       
      }
           
      if(tempzoom == 0x90){
       ZoomHot.act2 = 0x20;
       ZoomHot.fovh = 0x00;       
       ZoomHot.fovl = 0xC8;
       crc.value=crc16_ccitt( (uint8_t*)&ZoomHot + 2, 7 );
       ZoomHot.crc1 = crc.byte[1];
       ZoomHot.crc2 = crc.byte[0];
       HAL_UART_Transmit(&huart1, (uint8_t*)&ZoomHot, sizeof(ZoomHot), 50);         
       LuchParam.Zoom = Zoom4x;
       tempzoom = 0xC8;
       latch[1]=HI;       
           HAL_Delay(10);       
      }     
      if(tempzoom == 0x00){
       ZoomHot.act2 = 0x20;
       ZoomHot.fovh = 0x00;       
       ZoomHot.fovl = 0x64;
       crc.value=crc16_ccitt( (uint8_t*)&ZoomHot + 2, 7 );
       ZoomHot.crc1 = crc.byte[1];
       ZoomHot.crc2 = crc.byte[0];
       HAL_UART_Transmit(&huart1, (uint8_t*)&ZoomHot, sizeof(ZoomHot), 50);         
       LuchParam.Zoom = Zoom4x;
       tempzoom = 0x64;       
       latch[1]=HI;       
           HAL_Delay(10);       
      }      
       latch[10]=HI;
       latch[2]=HI;
    }

//////////////////////////////////////////////////////////////////////////////    
        if((pinSt[12] == ON) & (latch[12]==LOW))
    {
      if((latch[12]==LOW)){
//       TransmitT((void*)&FocusPls);
       crc.value=crc16_ccitt( (uint8_t *)&AutoFocus + 2, 8 );
       AutoFocus[12] = crc.byte[1];
       AutoFocus[13] = crc.byte[0];          
       HAL_UART_Transmit(&huart1, (uint8_t*)&AutoFocus, 14, 50);        
       latch[12]=HI;    
           HAL_Delay(10);       
      }
       latch[12]=HI;
    }
        if((pinSt[1] == ON) & (latch[3]==LOW))
    {
      if((latch[3]==LOW)){
//       TransmitT((void*)&FocusPls);
       crc.value=crc16_ccitt( (uint8_t *)&FocusPls + 2, 8 );
       FocusPls[12] = crc.byte[1];
       FocusPls[13] = crc.byte[0];          
       HAL_UART_Transmit(&huart1, (uint8_t*)&FocusPls, 14, 50);        
       latch[3]=HI;     
           HAL_Delay(10);       
      }
//       latch[3]=HI;
       latch[14]=HI;
    }
    
        if((pinSt[1] == OFF) & (latch[14]==HI))
    {
//       TransmitT((void*)&FocusStp);
       crc.value=crc16_ccitt( (uint8_t *)&FocusStp + 2, 8 );
       FocusStp[12] = crc.byte[1];
       FocusStp[13] = crc.byte[0];        
       HAL_UART_Transmit(&huart1, (uint8_t*)&FocusStp, 14, 50);      
       latch[14]=LOW;
    }
//////////////////////////////////////////////////////////////////////////////    
        if((pinSt[5] == ON) & (latch[5]==LOW))
    {
      if((latch[5]==LOW)){
//       TransmitT((void*)&FocusMns);  
       crc.value=crc16_ccitt( (uint8_t *)&FocusMns + 2, 8 );
       FocusMns[12] = crc.byte[1];
       FocusMns[13] = crc.byte[0];        
       HAL_UART_Transmit(&huart1, (uint8_t*)&FocusMns, 14, 50);        
       latch[5]=HI;     
           HAL_Delay(10);       
      }
//       latch[5]=HI;
       latch[6]=HI;
    }
    
        if((pinSt[5] == OFF) & (latch[6]==HI))
    {
//       TransmitT((void*)&FocusStp);
       crc.value=crc16_ccitt( (uint8_t *)&FocusStp + 2, 8 );
       FocusStp[12] = crc.byte[1];
       FocusStp[13] = crc.byte[0];      
       HAL_UART_Transmit(&huart1, (uint8_t*)&FocusStp, 14, 50);       
       latch[6]=LOW;
    }
    
        if((pinSt[3] == ON) & (latch[9]==LOW))
    {
       crc.value=crc16_ccitt( (uint8_t *)&Calibration + 2, 12 );
       Calibration[16] = crc.byte[1];
       Calibration[17] = crc.byte[0];      
       HAL_UART_Transmit(&huart1, (uint8_t*)&Calibration, 18, 50);       
       latch[9]=HI;
    }    
    
    
    
      /*MIDE*/
    
     if((pinSt[7] == ON) & (latch[7]==LOW))
    {
      if((LuchParam.Mide == MIDE_0)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE00 + 2, 16 );
       MIDE00[20] = crc.byte[1];
       MIDE00[21] = crc.byte[0];        
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE00, sizeof(MIDE00), 50);         
       LuchParam.Mide = MIDE_1;
       latch[7]=HI;      
           HAL_Delay(10);       
      }
      if((LuchParam.Mide == MIDE_1)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE01 + 2, 16 );
       MIDE01[20] = crc.byte[1];
       MIDE01[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE01, sizeof(MIDE01), 50);         
       LuchParam.Mide = MIDE_2;
       latch[7]=HI;
           HAL_Delay(10);       
      }
      if((LuchParam.Mide == MIDE_2)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE02 + 2, 16 );
       MIDE02[20] = crc.byte[1];
       MIDE02[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE02, sizeof(MIDE02), 50);         
       LuchParam.Mide = MIDE_3;
       latch[7]=HI;
           HAL_Delay(10);       
      }      
      if((LuchParam.Mide == MIDE_3)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE03 + 2, 16 );
       MIDE03[20] = crc.byte[1];
       MIDE03[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE03, sizeof(MIDE03), 50);         
       LuchParam.Mide = MIDE_4;
       latch[7]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Mide == MIDE_4)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE04 + 2, 16 );
       MIDE04[20] = crc.byte[1];
       MIDE04[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE04, sizeof(MIDE04), 50);         
       LuchParam.Mide = MIDE_5;
       latch[7]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Mide == MIDE_5)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE05 + 2, 16 );
       MIDE05[20] = crc.byte[1];
       MIDE05[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE05, sizeof(MIDE05), 50);         
       LuchParam.Mide = MIDE_6;
       latch[7]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Mide == MIDE_6)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE06 + 2, 16 );
       MIDE06[20] = crc.byte[1];
       MIDE06[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE06, sizeof(MIDE06), 50);         
       LuchParam.Mide = MIDE_7;
       latch[7]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Mide == MIDE_7)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE07 + 2, 16 );
       MIDE07[20] = crc.byte[1];
       MIDE07[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE07, sizeof(MIDE07), 50);         
       LuchParam.Mide = MIDE_8;
       latch[7]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Mide == MIDE_8)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE08 + 2, 16 );
       MIDE08[20] = crc.byte[1];
       MIDE08[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE08, sizeof(MIDE08), 50);         
       LuchParam.Mide = MIDE_9;
       latch[7]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Mide == MIDE_9)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE09 + 2, 16 );
       MIDE09[20] = crc.byte[1];
       MIDE09[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE09, sizeof(MIDE09), 50);         
       LuchParam.Mide = MIDE_10;
       latch[7]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Mide == MIDE_10)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE10 + 2, 16 );
       MIDE10[20] = crc.byte[1];
       MIDE10[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE10, sizeof(MIDE10), 50);         
       LuchParam.Mide = MIDE_11;
       latch[7]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Mide == MIDE_11)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE11 + 2, 16 );
       MIDE11[20] = crc.byte[1];
       MIDE11[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE11, sizeof(MIDE11), 50);         
       LuchParam.Mide = MIDE_12;
       latch[7]=HI;
           HAL_Delay(10);       
      }
      if((LuchParam.Mide == MIDE_12)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE12 + 2, 16 );
       MIDE12[20] = crc.byte[1];
       MIDE12[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE12, sizeof(MIDE12), 50);         
       LuchParam.Mide = MIDE_13;
       latch[7]=HI;
           HAL_Delay(10);       
      }
      if((LuchParam.Mide == MIDE_13)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE13 + 2, 16 );
       MIDE13[20] = crc.byte[1];
       MIDE13[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE13, sizeof(MIDE13), 50);         
       LuchParam.Mide = MIDE_14;
       latch[7]=HI;
           HAL_Delay(10);       
      }
      if((LuchParam.Mide == MIDE_14)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE14 + 2, 16 );
       MIDE14[20] = crc.byte[1];
       MIDE14[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE14, sizeof(MIDE14), 50);         
       LuchParam.Mide = MIDE_15;
       latch[7]=HI;
           HAL_Delay(10);       
      }
      if((LuchParam.Mide == MIDE_15)&(latch[7]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&MIDE15 + 2, 16 );
       MIDE15[20] = crc.byte[1];
       MIDE15[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&MIDE15, sizeof(MIDE15), 50);         
       LuchParam.Mide = MIDE_0;
       latch[7]=HI;
           HAL_Delay(10);       
      }       
       latch[7]=HI;
          HAL_Delay(10);       
    }
 

      /*BRIGHTNESS*/

     if((pinSt[6] == ON) & (latch[16]==LOW))
    {
      if((LuchParam.Brightness == BRTN_0)&(latch[16]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT00 + 2, 16 );
       BRIGHIT00[20] = crc.byte[1];
       BRIGHIT00[21] = crc.byte[0];        
       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT00, sizeof(BRIGHIT00), 50);         
       LuchParam.Brightness = BRTN_1;
       latch[16]=HI;
           HAL_Delay(10);       
      }
      if((LuchParam.Brightness == BRTN_1)&(latch[16]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT01 + 2, 16 );
       BRIGHIT01[20] = crc.byte[1];
       BRIGHIT01[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT01, sizeof(BRIGHIT01), 50);         
       LuchParam.Brightness = BRTN_2;

       latch[16]=HI;
           HAL_Delay(10);       
      }
      if((LuchParam.Brightness == BRTN_2)&(latch[16]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT02 + 2, 16 );
       BRIGHIT02[20] = crc.byte[1];
       BRIGHIT02[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT02, sizeof(BRIGHIT02), 50);         
       LuchParam.Brightness = BRTN_3;

       latch[16]=HI;
           HAL_Delay(10);       
      }      
      if((LuchParam.Brightness == BRTN_3)&(latch[16]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT03 + 2, 16 );
       BRIGHIT03[20] = crc.byte[1];
       BRIGHIT03[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT03, sizeof(BRIGHIT03), 50);         
       LuchParam.Brightness = BRTN_4;

       latch[16]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Brightness == BRTN_4)&(latch[16]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT04 + 2, 16 );
       BRIGHIT04[20] = crc.byte[1];
       BRIGHIT04[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT04, sizeof(BRIGHIT04), 50);         
       LuchParam.Brightness = BRTN_5;

       latch[16]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Brightness == BRTN_5)&(latch[16]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT05 + 2, 16 );
       BRIGHIT05[20] = crc.byte[1];
       BRIGHIT05[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT05, sizeof(BRIGHIT05), 50);         
       LuchParam.Brightness = BRTN_6;

       latch[16]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Brightness == BRTN_6)&(latch[16]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT06 + 2, 16 );
       BRIGHIT06[20] = crc.byte[1];
       BRIGHIT06[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT06, sizeof(BRIGHIT06), 50);         
       LuchParam.Brightness = BRTN_7;

       latch[16]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Brightness == BRTN_7)&(latch[16]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT07 + 2, 16 );
       BRIGHIT07[20] = crc.byte[1];
       BRIGHIT07[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT07, sizeof(BRIGHIT07), 50);         
       LuchParam.Brightness = BRTN_0;
       latch[16]=HI;
           HAL_Delay(10);       
      }  
//      if((LuchParam.Brightness == BRTN_8)&(latch[2]==LOW)){
//       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT08 + 2, 16 );
//       BRIGHIT08[20] = crc.byte[1];
//       BRIGHIT08[21] = crc.byte[0];           
//       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT08, sizeof(BRIGHIT08), 50);         
//       LuchParam.Brightness = BRTN_9;
//       latch[2]=HI;
//      }  
//      if((LuchParam.Brightness == BRTN_9)&(latch[2]==LOW)){
//       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT09 + 2, 16 );
//       BRIGHIT09[20] = crc.byte[1];
//       BRIGHIT09[21] = crc.byte[0];           
//       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT09, sizeof(BRIGHIT09), 50);         
//       LuchParam.Brightness = BRTN_10;
//       latch[2]=HI;
//      }  
//      if((LuchParam.Brightness == BRTN_10)&(latch[2]==LOW)){
//       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT10 + 2, 16 );
//       BRIGHIT10[20] = crc.byte[1];
//       BRIGHIT10[21] = crc.byte[0];           
//       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT10, sizeof(BRIGHIT10), 50);         
//       LuchParam.Brightness = BRTN_11;
//       latch[2]=HI;
//      }  
//      if((LuchParam.Brightness == BRTN_11)&(latch[2]==LOW)){
//       crc.value=crc16_ccitt( (uint8_t *)&BRIGHIT11 + 2, 16 );
//       BRIGHIT11[20] = crc.byte[1];
//       BRIGHIT11[21] = crc.byte[0];           
//       HAL_UART_Transmit(&huart1, (uint8_t*)&BRIGHIT11, sizeof(BRIGHIT11), 50);         
//       LuchParam.Brightness = BRTN_0;
//       latch[2]=HI;
//      }

       latch[16]=HI;
    }
    
      /*CONTRAST*/

     if((pinSt[9] == ON) & (latch[13]==LOW))
    {
      if((LuchParam.Contrast == CONTR_0)&(latch[13]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&CONTR00 + 2, 16 );
       CONTR00[20] = crc.byte[1];
       CONTR00[21] = crc.byte[0];        
       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR00, sizeof(CONTR00), 50);         
       LuchParam.Contrast = CONTR_1;
       latch[13]=HI;       
           HAL_Delay(10);       
      }
      if((LuchParam.Contrast == CONTR_1)&(latch[13]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&CONTR01 + 2, 16 );
       CONTR01[20] = crc.byte[1];
       CONTR01[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR01, sizeof(CONTR01), 50);         
       LuchParam.Contrast = CONTR_2;
       latch[13]=HI;
           HAL_Delay(10);       
      }
      if((LuchParam.Contrast == CONTR_2)&(latch[13]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&CONTR02 + 2, 16 );
       CONTR02[20] = crc.byte[1];
       CONTR02[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR02, sizeof(CONTR02), 50);         
       LuchParam.Contrast = CONTR_3;
       latch[13]=HI;
           HAL_Delay(10);       
      }      
      if((LuchParam.Contrast == CONTR_3)&(latch[13]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&CONTR03 + 2, 16 );
       CONTR03[20] = crc.byte[1];
       CONTR03[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR03, sizeof(CONTR03), 50);         
       LuchParam.Contrast = CONTR_4;
       latch[13]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Contrast == CONTR_4)&(latch[13]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&CONTR04 + 2, 16 );
       CONTR04[20] = crc.byte[1];
       CONTR04[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR04, sizeof(CONTR04), 50);         
       LuchParam.Contrast = CONTR_5;
       latch[13]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Contrast == CONTR_5)&(latch[13]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&CONTR05 + 2, 16 );
       CONTR05[20] = crc.byte[1];
       CONTR05[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR05, sizeof(CONTR05), 50);         
       LuchParam.Contrast = CONTR_6;
       latch[13]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Contrast == CONTR_6)&(latch[13]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&CONTR06 + 2, 16 );
       CONTR06[20] = crc.byte[1];
       CONTR06[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR06, sizeof(CONTR06), 50);         
       LuchParam.Contrast = CONTR_7;
       latch[13]=HI;
           HAL_Delay(10);       
      }  
      if((LuchParam.Contrast == CONTR_7)&(latch[13]==LOW)){
       crc.value=crc16_ccitt( (uint8_t *)&CONTR07 + 2, 16 );
       CONTR07[20] = crc.byte[1];
       CONTR07[21] = crc.byte[0];           
       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR07, sizeof(CONTR07), 50);         
       LuchParam.Contrast = CONTR_0;
       latch[13]=HI;
           HAL_Delay(10);       
      }  
//      if((LuchParam.Contrast == CONTR_8)&(latch[13]==LOW)){
//       crc.value=crc16_ccitt( (uint8_t *)&CONTR08 + 2, 16 );
//       CONTR08[20] = crc.byte[1];
//       CONTR08[21] = crc.byte[0];           
//       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR08, sizeof(CONTR08), 50);         
//       LuchParam.Contrast = CONTR_9;
//       latch[13]=HI;
//      }  
//      if((LuchParam.Contrast == CONTR_9)&(latch[13]==LOW)){
//       crc.value=crc16_ccitt( (uint8_t *)&CONTR09 + 2, 16 );
//       CONTR09[20] = crc.byte[1];
//       CONTR09[21] = crc.byte[0];           
//       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR09, sizeof(CONTR09), 50);         
//       LuchParam.Contrast = CONTR_10;
//       latch[13]=HI;
//      }  
//      if((LuchParam.Contrast == CONTR_10)&(latch[13]==LOW)){
//       crc.value=crc16_ccitt( (uint8_t *)&CONTR10 + 2, 16 );
//       CONTR10[20] = crc.byte[1];
//       CONTR10[21] = crc.byte[0];           
//       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR10, sizeof(CONTR10), 50);         
//       LuchParam.Contrast = CONTR_11;
//       latch[13]=HI;
//      }  
//      if((LuchParam.Contrast == CONTR_11)&(latch[13]==LOW)){
//       crc.value=crc16_ccitt( (uint8_t *)&CONTR11 + 2, 16 );
//       CONTR11[20] = crc.byte[1];
//       CONTR11[21] = crc.byte[0];           
//       HAL_UART_Transmit(&huart1, (uint8_t*)&CONTR11, sizeof(CONTR11), 50);         
//       LuchParam.Contrast = CONTR_0;
//       latch[13]=HI;
//      }

       latch[13]=HI;
    
    }
    
      /*ZOOM*/ 
    
    if((pinSt[8] == ON) & (latch[8]==LOW))
    {
      if((LuchParam.HoBl == WhiteHot)&(latch[8]==LOW)){
       ZoomHot.act1 = 0x20;
       ZoomHot.act2 = 0x00;
       ZoomHot.rsrv = 0x00;
       ZoomHot.fovh = 0x00;
       ZoomHot.fovl = 0x00;       
       crc.value=crc16_ccitt( (uint8_t *)&ZoomHot + 2, 7 );
       ZoomHot.crc1 = crc.byte[1];
       ZoomHot.crc2 = crc.byte[0];       
       HAL_UART_Transmit(&huart1, (uint8_t*)&ZoomHot, sizeof(ZoomHot), 50);         
       LuchParam.HoBl = BlackHot;
       latch[8]=HI;      
           HAL_Delay(10);       
      }
      if((LuchParam.HoBl == BlackHot)&(latch[8]==LOW)){
       ZoomHot.act1 = 0x00;
       ZoomHot.act2 = 0x00;
       ZoomHot.rsrv = 0x00;
       ZoomHot.fovh = 0x00;
       ZoomHot.fovl = 0x00;       
       crc.value=crc16_ccitt( (uint8_t *)&ZoomHot + 2, 7 );
       ZoomHot.crc1 = crc.byte[1];
       ZoomHot.crc2 = crc.byte[0];          
       HAL_UART_Transmit(&huart1, (uint8_t*)&ZoomHot, sizeof(ZoomHot), 50);         
       LuchParam.HoBl = WhiteHot;
       latch[8]=HI;
           HAL_Delay(10);       
      }      
       latch[8]=HI;
          HAL_Delay(10);       
    }    
}
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  
  
  for(uint8_t i=0;i<20;i++)
  {
   latch[i]= LOW ;
  }
  /* USER CODE BEGIN 1 */
  SetTime(100,TICK_TIMER);
  SetTime(100,BUTTONS_TIMER);
  SetTime(10,ENC_TIMER);
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
// MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
    rx_state = UART_RX_HEADER;
  HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart_rx_byte, 1);//sizeof(state)
  initZoomHotstr(&ZoomHot);
  initMidestr(&mide);
  LuchParam.HoBl = WhiteHot;
  scannerPin();
  encarry[0] = temp;
  encarry[1] = temp;  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    /**************************************************************************************
    * Мигаем светодиодом с периодом 0,5 сек
    **************************************************************************************/

    
    /*************************************************************************************
    * Управление выводами 
    **************************************************************************************/
   if(!GetTime(BUTTONS_TIMER)){ 
      scannerPin();
      SetTime(100,BUTTONS_TIMER);
   }
//////////////////////////////////////////////////////////////////////////////
    KnobFunk();
   if(!GetTime(ENC_TIMER)){
    FocusEnc();
    MIDEEnc();
    SetTime(10,ENC_TIMER);
  }
  
   if(!GetTime(TICK_TIMER)){ 
       ReadPinEnc();
      EncoderCapture();
      SetTime(20,TICK_TIMER);
   }  

///////////////////////////////////////////////////////////////////////////////    
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef   RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef   RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
