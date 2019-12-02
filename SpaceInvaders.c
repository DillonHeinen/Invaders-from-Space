// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 9/2/2019 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2019

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2019

 Copyright 2019 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void PEInit(void);	//PORTE Initialization
void English(void);	//game in english
void Spanish(void);	//game in spanish
void Level1(void);	//Level 1
void Level2(void);	//Level 2

uint32_t score=0;
uint32_t gameFlag=0;
int wait;

void PEInit(void){
	SYSCTL_RCGCGPIO_R |= 0x10;		//turn on PORTE
	wait++;
	wait++;
	GPIO_PORTE_DEN_R 	|= 0x01;		//digital enable PE0
	GPIO_PORTE_DIR_R	&= ~0x01;		//PE0 is input
}

int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
	ADC_Init();
  Output_Init();
	PEInit();
	
	
  ST7735_FillScreen(0x0000);            // set screen to black
  
  ST7735_DrawBitmap(52, 159, ns, 18,8); // player ship middle bottom
 // ST7735_DrawBitmap(53, 151, Bunker0, 18,5);

  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);

  Delay100ms(5);              // delay .5 sec at 80 MHz

//  ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(1, 2);
  ST7735_OutString("INVADERS FROM SPACE");
//  ST7735_SetCursor(1, 4);
//  ST7735_OutString("    CREATED BY:    ");
//  ST7735_SetCursor(1, 5);
//  ST7735_OutString("   JIM & DILLON   ");
  ST7735_SetCursor(7, 7);
	ST7735_OutString("Level 1");
  ST7735_SetCursor(7, 8);
	ST7735_OutString("Level 2");
  ST7735_SetCursor(7, 9);
	ST7735_OutString("English");
  ST7735_SetCursor(7, 10);
	ST7735_OutString("Espa\xA4ol");
  
  while(1){
		int cursor = ADC_In();
		if(cursor<1023){
			ST7735_DrawBitmap(0, 78, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				Level1();
			}
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
		}
		else if(cursor>1022 && cursor<2047){
			ST7735_DrawBitmap(0, 88, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				Level2();
			}
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
		}
		else if(cursor>2046 && cursor<3070){
			ST7735_DrawBitmap(0, 98, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				English();
			}
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
		}
		else{
			ST7735_DrawBitmap(0, 108, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				Spanish();
			}
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		if(gameFlag==1){
			//print out score and game over screen.
			ST7735_FillScreen(0x0000);	//set screen to black
			ST7735_SetCursor(6, 3);
			ST7735_OutString("GAME OVER!");
			ST7735_SetCursor(7, 5);
			ST7735_OutString("Score: ");	LCD_OutDec(score);
			Delay100ms(60);		//delay 6 seconds at 80MHz
			English();
			gameFlag=0;
		}
		
  }
		
}

void English(void){
	ST7735_FillScreen(0x0000);            // set screen to black
  
  ST7735_DrawBitmap(52, 159, ns, 18,8); // player ship middle bottom
 // ST7735_DrawBitmap(53, 151, Bunker0, 18,5);

  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);

  Delay100ms(5);              // delay .5 sec at 80 MHz

//  ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(1, 2);
  ST7735_OutString("INVADERS FROM SPACE");
//  ST7735_SetCursor(1, 4);
//  ST7735_OutString("    CREATED BY:    ");
//  ST7735_SetCursor(1, 5);
//  ST7735_OutString("   JIM & DILLON   ");
  ST7735_SetCursor(7, 7);
	ST7735_OutString("Level 1");
  ST7735_SetCursor(7, 8);
	ST7735_OutString("Level 2");
  ST7735_SetCursor(7, 9);
	ST7735_OutString("English");
  ST7735_SetCursor(7, 10);
	ST7735_OutString("Espa\xA4ol");
	while(1){
		int cursor = ADC_In();
		if(cursor<1023){
			ST7735_DrawBitmap(0, 78, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				Level1();
			}
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
		}
		else if(cursor>1022 && cursor<2047){
			ST7735_DrawBitmap(0, 88, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				Level2();
			}
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
		}
		else if(cursor>2046 && cursor<3070){
			ST7735_DrawBitmap(0, 98, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				English();
			}
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
		}
		else{
			ST7735_DrawBitmap(0, 108, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				Spanish();
			}
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		if(gameFlag==1){
			//print out score and game over screen.
			ST7735_FillScreen(0x0000);	//set screen to black
			ST7735_SetCursor(6, 3);
			ST7735_OutString("GAME OVER!");
			ST7735_SetCursor(7, 5);
			ST7735_OutString("Score: ");	LCD_OutDec(score);
			Delay100ms(60);		//delay 6 seconds at 80MHz
			English();
			gameFlag=0;
		}
		
  }
}

void Spanish(void){
	
		ST7735_FillScreen(0x0000);            // set screen to black
  
  ST7735_DrawBitmap(52, 159, ns, 18,8); // player ship middle bottom
 // ST7735_DrawBitmap(53, 151, Bunker0, 18,5);

  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);

  Delay100ms(5);              // delay .5 sec at 80 MHz

//  ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(0, 2);
  ST7735_OutString("INVASORES DEL  ESPACIO");
  ST7735_SetCursor(7, 7);
	ST7735_OutString("Nivel 1");
  ST7735_SetCursor(7, 8);
	ST7735_OutString("Nivel 2");
  ST7735_SetCursor(7, 9);
	ST7735_OutString("English");
  ST7735_SetCursor(7, 10);
	ST7735_OutString("Espa\xA4ol");
  
  while(1){
		int cursor = ADC_In();
		if(cursor<1023){
			ST7735_DrawBitmap(0, 78, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				Level1();
			}
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
		}
		else if(cursor>1022 && cursor<2047){
			ST7735_DrawBitmap(0, 88, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				Level2();
			}
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
		}
		else if(cursor>2046 && cursor<3070){
			ST7735_DrawBitmap(0, 98, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				English();
			}
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
		}
		else{
			ST7735_DrawBitmap(0, 108, SmallEnemy10pointA, 16,10);
			if(GPIO_PORTE_DATA_R==1){
				Spanish();
			}
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		
		if(gameFlag==1){
			//print out score and game over screen.
			ST7735_FillScreen(0x0000);	//set screen to black
			ST7735_SetCursor(7, 3);
			ST7735_OutString("\xADJUEGO TERMINADO!");
			ST7735_SetCursor(7, 5);
			ST7735_OutString("Puntuaci\xA2n: ");	LCD_OutDec(score);
			Delay100ms(60);		//delay 6 seconds at 80MHz
			English();
			gameFlag=0;
		}
	}
}

void Level1(void){
	while(1){
		
	}
}

void Level2(void){
	while(1){
		
	}
}
// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
