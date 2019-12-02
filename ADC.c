// put implementations for functions, explain how it works
// Jim Foster & Dillon Heinen 12/1/2019
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
uint32_t result;
void ADC_Init(void){ 
	SYSCTL_RCGCGPIO_R |= 0x08;
	volatile int wait=0;
	wait++;
	GPIO_PORTD_DIR_R &= ~0x04;
	GPIO_PORTD_DEN_R &= ~0x04;
	GPIO_PORTD_AFSEL_R |= 0x04;
	GPIO_PORTD_AMSEL_R |= 0x04;
	
	SYSCTL_RCGCADC_R |= 0x01;			//activate ADC0
	wait++;
	wait++;
	ADC0_PC_R |= 0x01;						//set sampling rate to 125kHz
	ADC0_SSPRI_R = 0x0123;				//sequencer3 is highest priority
	ADC0_ACTSS_R &= 0x08;					//disable sample sequencer3
	ADC0_EMUX_R &= ~0xF000;				//sequence 3 is s/w trigger
	ADC0_SSMUX3_R =(ADC0_SSMUX3_R & 0xFFFFFFF0)+5;	//AIN5 (PD2)
	ADC0_SSCTL3_R = 0x0006;				//no TS0,D0,yes IE0 END0
	ADC0_IM_R &= ~0x0008;					//disable SS3 interrupts
	ADC0_ACTSS_R |= 0x08;					//enable SS3
	ADC0_SAC_R =0x06;							//avg samples to get better distance values near ends of potentiometer
	//comment out line above when working in simulation

}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
	ADC0_PSSI_R = 0x08;					//initialize SS3
	while((ADC0_RIS_R&0x08)==0){};
	result = ADC0_SSFIFO3_R&0xFFF;
	ADC0_ISC_R = 0x0008;
	return result;
}
