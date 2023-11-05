/******************************************************************************
*	Project name:
*	File name:
*	Author:
*	Date:
*	Description: This is a template file for Interrupt configuration for 
*                User button connected to PF4. It turns on different LEDs
*                on falling edge of button press
 ******************************************************************************/


#include "TM4C123.h"
/******************************************************************************
*Macros for Register Addresses and Values
*******************************************************************************/

/* Register for GPIO clock */
#define SYSCTL_RCGCGPIO_R         (*((volatile unsigned long *)0x400FE608))
#define GPIO_PORTF_CLK            0x20
/* Enable Register for PortF Interrupt */
#define NVIC_EN0_R		     (*((volatile unsigned long *)0xE000E100))
/* Priority Register for PortF Interrupt */
#define NVIC_PRI7_R		     (*((volatile unsigned long *)0xE000E41C))
/* Port F GPIO Registers */

#define GPIOIM_PORTF_R         (*((volatile unsigned long *)0x40025410))
#define GPIOIS_PORTF_R         (*((volatile unsigned long *)0x40025404))
#define GPIOIBE_PORTF_R		     (*((volatile unsigned long *)0x40025408))
#define GPIOIEV_PORTF_R		     (*((volatile unsigned long *)0x4002540C))
#define GPIOICR_PORTF_R		     (*((volatile unsigned long *)0x4002541C))

#define GPIO_PORTF_DIR_R		   (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_DEN_R		   (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_PU_R		     (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DATA_R      (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_P4     0x10
#define GPIO_PORTF_P3     0x08
#define GPIO_PORTF_P2     0x04
#define GPIO_PORTF_P1     0x02

/* Function Declarations */	
/*-----These are functions added in startup file-----*/
/* Enable interrupts */
void EnableInterrupts(void);
/* Disableinterrupts */									
void DisableInterrupts(void);
/* Enable Priority interrupts */
void EnablePriorityInterrupts(void);
/* Implements WFI*/							
void WaitForInterrupt(void);

/*----These are C frunctions of this file-------*/
/* Initialize GPIO and Interrupts */
void Init_INT_GPIO(void);
/* Implements delay	*/
void Delay(unsigned long value);
/* Global Variable */
volatile unsigned long i=0;


void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}


void Init_INT_GPIO(void){
	volatile unsigned delay_clk;
	
	/* Enable clock for PORTF */
  SYSCTL_RCGCGPIO_R |= GPIO_PORTF_CLK ;
	/* Dummy read to stable the clock */	
	delay_clk = SYSCTL_RCGCGPIO_R 	;
	
	
	/* Enable digital I/O on PF4,PF3-PF1, 
	* Make PF4 input and PF3-PF1 output
	* Enable weak pullup on PF4 */
	
	GPIO_PORTF_DEN_R	|= GPIO_PORTF_P4 + GPIO_PORTF_P3 + GPIO_PORTF_P2 +GPIO_PORTF_P1 ;
	GPIO_PORTF_DIR_R	&= (~GPIO_PORTF_P4) ;
	GPIO_PORTF_DIR_R	|=  GPIO_PORTF_P3 + GPIO_PORTF_P2 +GPIO_PORTF_P1;
	GPIO_PORTF_PU_R		|=  GPIO_PORTF_P4;
	
	
	
	/* INTERRUPT Configuration */
	/* Globally disable all interrupts */
	DisableInterrupts();
	
	/* Disable all interrupts on PortF */
    GPIOIM_PORTF_R   &= 0x00;
	
	/* Enable PortF interrupt in NVIC */
   NVIC_EN0_R |= 0x40000000	;
	
	/* Set priority of PortF as 5 */
	 NVIC_PRI7_R	|= 0x00A00000 ;
	
	
	/* Configure PF4 as edge sensitive,
	* not both edges,
	* falling edge */
	
	GPIOIS_PORTF_R  &= (~GPIO_PORTF_P4)  ;
	GPIOIBE_PORTF_R &= (~GPIO_PORTF_P4);
	GPIOIEV_PORTF_R &= (~GPIO_PORTF_P4) ;
	
	
	/* Clear interrupt flag for PF4 */
	 GPIOICR_PORTF_R = GPIO_PORTF_P4;	
	
	/* Enable interrupt on PF4 */
	GPIOIM_PORTF_R |= GPIO_PORTF_P4 ;	

	/* Enable interrupts beyond the defined priority level */
	EnablePriorityInterrupts();
	/* Globally enable all interrupts */
	EnableInterrupts();
}


void Delay(unsigned long value){
	unsigned long i=0;
	for(i=0;i<value;i++);
}

/* Interrupt Service Routine for PortF */
void GPIOF_Handler(void){
		int j;
		/* Clear interrupt flag for PF4 */
		 GPIOICR_PORTF_R |= 0x10;
		
		/* Turn on different LEDs on each button press */
		if(i>=3)
		i=1;
		else
		i++;
		for(j=0;j<2;j++)
		{
			GPIO_PORTF_DATA_R^=1<<i;
			Delay(10000000);
		}
}

/* Main Function */
int main(){
	Init_INT_GPIO();
	while(1)
	{
		WaitForInterrupt();
	}
}