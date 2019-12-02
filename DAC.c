// put implementations for functions, explain how it works
// Jim Foster & Dillon Heinen 12/1/2019
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>


//Initialize an 8-bit DAC with PB7-0
void DAC_Init(void){
    SYSCTL_RCGCGPIO_R |= 0x02;
    while((SYSCTL_PRGPIO_R&0x02) == 0){};
 //   GPIO_PORTB_AMSEL_R &= ~0x0F;      // no analog
    //GPIO_PORTB_PCTL_R &= ~0x00000FFF; // regular GPIO function  //is this right?
    GPIO_PORTB_DIR_R |= 0xFF;      // make PB7-0 out
    //GPIO_PORTB_AFSEL_R &= ~0x0F;   // disable alt funct on PB3-0
    GPIO_PORTB_DEN_R |= 0xFF;      // enable digital I/O on PB7-0

}

void DAC_Out(uint32_t data){
    GPIO_PORTB_DATA_R = data;
}
