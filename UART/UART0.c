/******************************************************************************
*	Project name:
*	File name:
*	Author:
*	Date:
*	Description: This is a template file for UART0 configuration. It echoes back
*				 the character sent.

******************************************************************************/
#include "TM4C123.h"
void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}

/******************************************************************************
*Macros for Register Addresses and Values
*******************************************************************************/
//Register definitions for ClockEnable
#define 	SYSCTL_RCGC_UART_R				(*((volatile unsigned long*)0x400FE618))
#define 	SYSCTL_RCGC_GPIO_R				(*((volatile unsigned long*)0x400FE608))

//Register definitions for GPIOPortA
#define 	GPIO_PORT_A_AFSEL_R				(*((volatile unsigned long*)0x40004420))
#define 	GPIO_PORTA_PCTL_R				  (*((volatile unsigned long*)0x4000452C))
#define 	GPIO_PORTA_DEN_R				  (*((volatile unsigned long*)0x4000451C))
#define 	GPIO_PORTA_DIR_R				  (*((volatile unsigned long*)0x40004400))

//Register definitions for UART0_ module
#define 	UART0_CTL_R						(*((volatile unsigned long*)0x4000C030))
#define  	UART0_IBRD_R					(*((volatile unsigned long*)0x4000C024))
#define  	UART0_FBRD_R					(*((volatile unsigned long*)0x4000C028))
#define 	UART0_LCRH_R					(*((volatile unsigned long*)0x4000C02C))
#define 	UART0_CC_R						(*((volatile unsigned long*)0x4000CFC8))
#define 	UART0_FR_R						(*((volatile unsigned long*)0x4000C018))
#define 	UART0_DR_R						(*((volatile unsigned long*)0x4000C000))

//Macros
#define 	UART_FR_TX_FF				0x20								//UART Transmit FIFO Full
#define 	UART_FR_RX_FE				0x10								//UART Receive FIFO Empty

//Function definitions
unsigned char UARTRx(void);
void UARTTx(unsigned char data);
void UARTTxString(char*pt);
void UARTRxString(char*bufPt, unsigned short max);

//Intialize and configure UART
void UARTInit(void)
{
	//Enable clock for UART0_ and GPIO PortA
		SYSCTL_RCGC_GPIO_R	|= 0x01;  
	  SYSCTL_RCGC_UART_R	|= 0x01;		
 			
	
	// Configure GPIO as UART (AFSEL,PCTL,DEN)
		GPIO_PORT_A_AFSEL_R	|= 0x03; 
		GPIO_PORTA_PCTL_R |= 0x00000011;
		GPIO_PORTA_DEN_R    |= 0x03;
		GPIO_PORTA_DIR _R   |= 0x02;
	//Disable UART
	  UART0_CTL_R |= 0x300;
	
	//Select system clock/PIOSC as UART Baud clock
		UART0_CC_R |= 0x00;
	
	//Set Baud Rate
		UART0_IBRD_R |= 0x0008;
  	UART0_FBRD_R |= 0x2C;
	
	//8bit word length,no parity bit,one stop bit,FIFOs enable
     	UART0_LCRH_R |= 0x70;
	
	//Enable UART
		UART0_CTL_R |= 0x301;

}

//Wait for input and returns its ASCII value
unsigned char UARTRx(void)
{
	while((UART0_FR_R & UART_FR_RX_FE)!=0);
	return((unsigned char)(UART0_DR_R & 0x0FF));
}

/*Accepts ASCII characters from the serial port and
adds them to a string.It echoes each character as it
is inputted.*/
void UARTRxString (char *pt, unsigned short max)
{
	int length = 0;
		char character ;
	
		character = UARTRx();
		if(length<max)
		{
			*pt = character;
			pt++;
			length++;
			UARTTx(character+1);
		}
		*pt = 0;
		
}

//Output 8bit to serial port
void UARTTx(unsigned char data)
{
	while((UART0_FR_R & UART_FR_TX_FF)!=0);
	UART0_DR_R=data;
}


//Output a character string to serial port
void UARTTxString(char *pt)
{
	while (*pt)
	{
	  UARTTx(*pt);
		pt++;
	}
}

int main(void)
{

	char string[17];

	UARTInit();

	//The input given using keyboard is displayed on hyper terminal
	//.i.e.,data is echoed
	UARTTxString("EnterText:");

while(1)
	{
		UARTRxString(string,16);
	}
}
