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
#include "DAC.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void P_Init(void);	//PORTE Initialization
void English(void);	//game in english
void Spanish(void);	//game in spanish
void Level1(void);	//Level 1
void Level2(void);	//Level 2
void DrawEnemies1(void);
void DrawEnemies2(void);
void VibeCheck(void);
void LaunchMissile(void);
void Game_Init(void);

uint32_t score=0, gameFlag=0, converted, posit2=0, posit1=0, i=0, j=0, globe=0, lifecheck=0, levelup=1;
int wait;
int horiz=0, vert=9, count=0, left=1, right=0, ammo=1, path=151, MissileHitEnemy=0, MissileHitPlayer=0, MissileHitBunker=0;
uint32_t MissileLaunchX[6] = {20, 40, 60, 80, 100, 120};
uint32_t MissileLaunchY[6] = {10, 20, 30, 40, 50, 60};

struct mailbox{
	uint32_t Mail;
	uint32_t Status;
	uint32_t previous;
};

typedef struct mailbox SType;
struct mailbox Player;
struct mailbox PMissile;

struct enemy{
	uint32_t ObjX;
	uint32_t ObjY;
	uint32_t Life;
};

typedef struct enemy EType;
struct enemy ESprite;

struct enemy Enemies[30] = {
	{0,0,1},//0
	{0,0,1},//1
	{0,0,1},//2
	{0,0,1},//3
	{0,0,1},//4
	{0,0,1},//5
	
	{0,0,1},//6
	{0,0,1},//7
	{0,0,1},//8
	{0,0,1},//9
	{0,0,1},//10
	{0,0,1},//11
	
	{0,0,1},//12
	{0,0,1},//13
	{0,0,1},//14
	{0,0,1},//15
	{0,0,1},//16
	{0,0,1},//17
	
	{0,0,1},//18
	{0,0,1},//19
	{0,0,1},//20
	{0,0,1},//21
	{0,0,1},//22
	{0,0,1},//23
	
	{0,0,1},//24
	{0,0,1},//25
	{0,0,1},//26
	{0,0,1},//27
	{0,0,1},//28
	{0,0,1},//29
	
};

void Game_Init(void){

	for(i=0;i<30;i++){
		if(i<6){
			if(i==0){
				while(j<30){
					Enemies[i+j].ObjX=horiz;
					j+=6;
				}
				j=0;
			}
			if(i==1){
				while(j<30){
					Enemies[i+j].ObjX=horiz+20;
					j+=6;
				}
				j=0;
			}
			if(i==2){
				while(j<30){
					Enemies[i+j].ObjX=horiz+40;
					j+=6;
				}
				j=0;
			}
			if(i==3){
				while(j<30){
					Enemies[i+j].ObjX=horiz+60;
					j+=6;
				}
				j=0;
			}
			if(i==4){
				while(j<30){
					Enemies[i+j].ObjX=horiz+80;
					j+=6;
				}
				j=0;
			}
			if(i==5){
				while(j<30){
					Enemies[i+j].ObjX=horiz+100;
					j+=6;
				}
				j=0;
			}
			Enemies[i].ObjY=vert;
		}
		else if(i<12){
			Enemies[i].ObjY=vert+10;
		}
		else if(i<18){
			Enemies[i].ObjY=vert+20;
		}
		else if(i<24){
			Enemies[i].ObjY=vert+30;
		}
		else if(i>23){
			Enemies[i].ObjY=vert+40;
		}
	}
}

void P_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x30;		//turn on PORTE & F
	wait++;
	wait++;
	GPIO_PORTE_DEN_R 	|= 0x03;		//digital enable PE0-1
	GPIO_PORTE_DIR_R	&= ~0x03;		//PE0-1 is input
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R = 0x01;
	GPIO_PORTF_DIR_R &= ~0x01;		//PF0 input
	GPIO_PORTF_PUR_R = 0x01;
	GPIO_PORTF_DEN_R = 0x01;
}

void SysTickInit(void){
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x0028B0AA;  			// reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = 0x00000007;
}

uint32_t Convert(uint32_t input){
	converted = (110*input)/4095;		//110 keeps the player model on the screen
  return converted; 
}

void VibeCheck(void){
	
}

void LaunchMissile(void){
	ST7735_DrawBitmap(PMissile.Mail, path, missile, 12,12);
	Sound_Shoot();
	ammo=0;
}

void pause(void){
    if((GPIO_PORTF_DATA_R&0x01) == 0){
        NVIC_ST_CTRL_R = 0x05;
    }
		while((GPIO_PORTF_DATA_R&0x01) == 0){}
			NVIC_ST_CTRL_R = 0x07;
}

void pause_init(void){
    //initialize port f
	NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  Timer1_Init(&pause,80000000/11000); //11 kHz
	
}

void SysTick_Handler(void){
	Player.Mail = ADC_In();
	if(Player.Mail!=Player.previous){
		Player.Status = 1;
		posit1 = Convert(Player.previous);
		Player.previous=Player.Mail;
	}
	if(count<=5){
		if(left){
			horiz+=2; count++;
		}
		else if(right){
			horiz-=2; count++;
		}
		
	}
	else{
			vert++;
			count=0;
			left^=1;
			right^=1;
	}
	Game_Init();
	if((GPIO_PORTE_DATA_R&0x01)==1){		//Read, Aim, FIRE! Check to see if button is pressed for player missile to fire
		if(ammo){													//do you have ammo?
			PMissile.Status=1;							
		}
		PMissile.Mail = posit2;
	}
	if(path<0){							//
		PMissile.Status=0;		//if missile goes off screen switch Status to 0
	}												//
	if(PMissile.Status==0){
		ammo=1;
		path=151;
	}
	else{
		path-=2;
	}
		
	for(i=0;i<30;i++){
		if((Enemies[i].Life==1)&((PMissile.Mail+4>Enemies[i].ObjX+2)&(PMissile.Mail+8<Enemies[i].ObjX+14))&(path+12>=Enemies[i].ObjY)&(path<=Enemies[i].ObjY+10)){
			Enemies[i].Life=0;
			PMissile.Status=0;
			MissileHitEnemy=1;
			score+=10;
			Sound_Killed();
			i=30;
		}
			
	}
	
}


int main(void){
	Player.previous=5000;
  DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
	Game_Init();							//set up array of data structs to correct values
 // Output_Init();
	ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(0); 
	ADC_Init();
	P_Init();
	SysTickInit();
	Sound_Init();
	pause_init();
	
//	Spanish();			//here for debug reasons
//	Level1();					//debug reasons
  ST7735_FillScreen(0x0000);            // set screen to black
	
  ST7735_DrawBitmap(52, 159, PlayerShip0, 18,8); // player ship middle bottom
 // ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
//	Level1();
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
	for(int x=0; x<5000; x++){
		Sound_Fastinvader2();
	}
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
			if((GPIO_PORTE_DATA_R&0x01)==1){
				Level1();
			}
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		else if(cursor>1022 && cursor<2047){
			ST7735_DrawBitmap(0, 88, SmallEnemy10pointA, 16,10);
			if((GPIO_PORTE_DATA_R&0x01)==1){
				Level2();
			}
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		else if(cursor>2046 && cursor<3070){
			ST7735_DrawBitmap(0, 98, SmallEnemy10pointA, 16,10);
			if((GPIO_PORTE_DATA_R&0x01)==1){
				English();
			}
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		else{
			ST7735_DrawBitmap(0, 108, SmallEnemy10pointA, 16,10);
			if((GPIO_PORTE_DATA_R&0x01)==1){
				Spanish();
			}
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
		}
		
		
  }
		
}

void English(void){
	globe=0;
	ST7735_FillScreen(0x0000);            // set screen to black
  
  ST7735_DrawBitmap(52, 159, PlayerShip0, 18,8); // player ship middle bottom
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
			if((GPIO_PORTE_DATA_R&0x01)==1){
				Level1();
			}
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		else if(cursor>1022 && cursor<2047){
			ST7735_DrawBitmap(0, 88, SmallEnemy10pointA, 16,10);
			if((GPIO_PORTE_DATA_R&0x01)==1){
				Level2();
			}
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		else if(cursor>2046 && cursor<3070){
			ST7735_DrawBitmap(0, 98, SmallEnemy10pointA, 16,10);
			if((GPIO_PORTE_DATA_R&0x01)==1){
				English();
			}
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		else{
			ST7735_DrawBitmap(0, 108, SmallEnemy10pointA, 16,10);
			if((GPIO_PORTE_DATA_R&0x01)==1){
				Spanish();
			}
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
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
		globe=1;
	
		ST7735_FillScreen(0x0000);            // set screen to black
  
  ST7735_DrawBitmap(52, 159, PlayerShip0, 18,8); // player ship middle bottom
 // ST7735_DrawBitmap(53, 151, Bunker0, 18,5);

  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);

  Delay100ms(5);              // delay .5 sec at 80 MHz

//  ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(4, 2);
  ST7735_OutString("INVASORES DEL");
	ST7735_SetCursor(7, 3);
	ST7735_OutString("ESPACIO");
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
			if((GPIO_PORTE_DATA_R&0x01)==1){
				Level1();
			}
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		else if(cursor>1022 && cursor<2047){
			ST7735_DrawBitmap(0, 88, SmallEnemy10pointA, 16,10);
			if((GPIO_PORTE_DATA_R&0x01)==1){
				Level2();
			}
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		else if(cursor>2046 && cursor<3070){
			ST7735_DrawBitmap(0, 98, SmallEnemy10pointA, 16,10);
			if((GPIO_PORTE_DATA_R&0x01)==1){
				English();
			}
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
			ST7735_DrawBitmap(0, 108, Empty, 16,10);
		}
		else{
			ST7735_DrawBitmap(0, 108, SmallEnemy10pointA, 16,10);
			if((GPIO_PORTE_DATA_R&0x01)==1){
				Spanish();
			}
			ST7735_DrawBitmap(0, 88, Empty, 16,10);
			ST7735_DrawBitmap(0, 98, Empty, 16,10);
			ST7735_DrawBitmap(0, 78, Empty, 16,10);
		}
		
		if(gameFlag==1){
			
		}
	}
}

void Level1(void){
		ST7735_FillScreen(0x0000);            // set screen to black
		ST7735_DrawBitmap(2, 140, Bunker0, 16,10);
		ST7735_DrawBitmap(110, 140, Bunker0, 16,10);
	EnableInterrupts();
	while(1){	
		

		DrawEnemies1();
		ST7735_DrawBitmap(2, 140, Bunker0, 16,10);
		ST7735_DrawBitmap(110, 140, Bunker0, 16,10);
		
		if(MissileHitEnemy==1){
			for(i=0;i<30;i++){
				if(Enemies[i].Life==0){
					ST7735_DrawBitmap(Enemies[i].ObjX, Enemies[i].ObjY, EmptyEnemy, 16,10);
				}
			}
		}
		
		if(Player.Status==1){
			posit2 = Convert(Player.Mail);
			ST7735_DrawBitmap(posit1, 159, Empty, 18,8);
			ST7735_DrawBitmap(posit2, 159, PlayerShip0, 18,8);
			Player.Status=0;
		}
		
		if(PMissile.Status==1){
			LaunchMissile();
		}
		
		for(i=0;i<30;i++){
			if(Enemies[i].Life){
				if(Enemies[i].ObjY>130){
					gameFlag = 1;
				}
			}
			if(Enemies[i].Life==0){
				lifecheck+=1;
			}
			
		}
		if(lifecheck==30){
			gameFlag=2;
		}
		lifecheck = 0;
		
		
		if(gameFlag>0){
			if(globe==0){
				//print out score and game over screen.
				ST7735_FillScreen(0x0000);	//set screen to black
				ST7735_SetCursor(6, 3);
				ST7735_OutString("GAME OVER!");
				ST7735_SetCursor(7, 5);
				ST7735_OutString("Score: ");	LCD_OutDec(score);
				if(gameFlag==1){
				Sound_Explosion();
				}
				if(gameFlag==2){
				Sound_Highpitch();	
				}
				Delay100ms(60);		//delay 6 seconds at 80MHz
				gameFlag=0;
				score=0;
				main();
			}
			if(globe==1){
				//print out score and game over screen.
				ST7735_FillScreen(0x0000);	//set screen to black
				ST7735_SetCursor(2, 3);
				ST7735_OutString("\xADJUEGO TERMINADO!");
				ST7735_SetCursor(3, 5);
				ST7735_OutString("Puntuaci\xA2n: ");	LCD_OutDec(score);
				if(gameFlag==1){
				Sound_Explosion();
				}
				if(gameFlag==2){
				Sound_Highpitch();	
				}
				Delay100ms(60);		//delay 6 seconds at 80MHz
				gameFlag=0;
				score=0;
				main();
			}
		}
	}
	
}

void Level2(void){
		levelup=2;
		ST7735_FillScreen(0x0000);            // set screen to black
		ST7735_DrawBitmap(2, 140, Bunker0, 16,10);
		ST7735_DrawBitmap(110, 140, Bunker0, 16,10);
	EnableInterrupts();
	while(1){	
		

		DrawEnemies1();
		ST7735_DrawBitmap(2, 140, Bunker0, 16,10);
		ST7735_DrawBitmap(110, 140, Bunker0, 16,10);
		
		if(MissileHitEnemy==1){
			for(i=0;i<30;i++){
				if(Enemies[i].Life==0){
					ST7735_DrawBitmap(Enemies[i].ObjX, Enemies[i].ObjY, EmptyEnemy, 16,10);
				}
			}
		}
		
		if(Player.Status==1){
			posit2 = Convert(Player.Mail);
			ST7735_DrawBitmap(posit1, 159, Empty, 18,8);
			ST7735_DrawBitmap(posit2, 159, PlayerShip0, 18,8);
			Player.Status=0;
		}
		
		if(PMissile.Status==1){
			LaunchMissile();
		}
		
		for(i=0;i<30;i++){
			if(Enemies[i].Life){
				if(Enemies[i].ObjY>130){
					gameFlag = 1;
				}
			}
			if(Enemies[i].Life==0){
				lifecheck+=1;
			}
			
		}
		if(lifecheck==30){
			gameFlag=2;
		}
		lifecheck = 0;
		
		
		if(gameFlag>0){
			if(globe==0){
				//print out score and game over screen.
				ST7735_FillScreen(0x0000);	//set screen to black
				ST7735_SetCursor(6, 3);
				ST7735_OutString("GAME OVER!");
				ST7735_SetCursor(7, 5);
				ST7735_OutString("Score: ");	LCD_OutDec(score);
				if(gameFlag==1){
				Sound_Explosion();
				}
				if(gameFlag==2){
				Sound_Highpitch();	
				}
				Delay100ms(60);		//delay 6 seconds at 80MHz
				gameFlag=0;
				score=0;
				main();
			}
			if(globe==1){
				//print out score and game over screen.
				ST7735_FillScreen(0x0000);	//set screen to black
				ST7735_SetCursor(2, 3);
				ST7735_OutString("\xADJUEGO TERMINADO!");
				ST7735_SetCursor(3, 5);
				ST7735_OutString("Puntuaci\xA2n: ");	LCD_OutDec(score);
				if(gameFlag==1){
				Sound_Explosion();
				}
				if(gameFlag==2){
				Sound_Highpitch();	
				}
				Delay100ms(60);		//delay 6 seconds at 80MHz
				gameFlag=0;
				score=0;
				main();
			}
		}
	}
}

void DrawEnemies1(void){
		for(i=0;i<30;i++){
			if(Enemies[i].Life!=0){
				ST7735_DrawBitmap(Enemies[i].ObjX, Enemies[i].ObjY, SmallEnemy10pointA, 16,10);
			}
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

int mainSoundTest(void){
	Sound_Init();
	while(1){
		for(i=0;i<128;i++){
			Sound_Shoot();
			wait++;
			wait++;
		}
	}
}