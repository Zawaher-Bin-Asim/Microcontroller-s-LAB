
#include <stdlib.h>

void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}	
	
	
// Timer 1 base address
# define TM_BASE 0x40031000
	


	
// Peripheral clock enabling for timer and GPIO
# define SYSCTL_RCGC_TIMER_R 		(*((volatile unsigned long *) 0x400FE604))
# define SYSCTL_RCGC_GPIO_R 	  	(*((volatile unsigned long *) 0x400FE608))
# define CLOCK_GPIOF 							0x00000020 // Port F clock control
# define SYS_CLOCK_FREQUENCY 			16000000
	
/* Enable Register for PortF Interrupt */
#define NVIC_EN0_R 				(*((volatile unsigned long*)0xE000E100))
/* Priority Register for PortF Interrupt */
#define NVIC_PRI7_R 			(*((volatile unsigned long*)0xE000E41C))
	
	
// General purpose timer register definitions
# define GPTM_CONFIG_R 			(*((volatile long *) 0x40031000))
# define GPTM_TA_MODE_R 		(*((volatile long *) 0x40031004))
# define GPTM_TB_MODE_R 		(*((volatile long *) 0x40031008))
	
# define GPTM_CONTROL_R 		(*((volatile long *) 0x4003100C))
# define GPTM_INT_MASK_R 		(*((volatile long *) 0x40031018))
# define GPTM_INT_CLEAR_R 	(*((volatile long *) 0x40031024))
# define GPTM_TA_IL_R 			(*((volatile long *) 0x40031028))
# define GPTM_TB_IL_R 			(*((volatile long *) 0x4003102C))
# define GPTM_TA_MATCH_R  	(*((volatile long *) 0x40031030))
# define GPTM_TB_MATCH_R  	(*((volatile long *) 0x40031034))

	
// GPIO PF2 alternate function configuration
# define GPIO_PORTF_AFSEL_R (*((volatile unsigned long *) 0x40025420))
# define GPIO_PORTF_PCTL_R 	(*((volatile unsigned long *) 0x4002552C))
# define GPIO_PORTF_DEN_R 	(*((volatile unsigned long *) 0x4002551C))
#define GPIO_PORTF_DIR_R    (*((volatile unsigned long *) 0x40025400))
#define GPIO_PORTF_DATA_R   (*((volatile unsigned long *) 0x400253FC))	
#define GPIO_PORTF_LOCK_R   (*((volatile unsigned long *) 0x40025520))
#define GPIO_PORTF_CR_R     (*((volatile unsigned long *) 0x40025524))
#define GPIO_PORTF_PUR_R    (*((volatile unsigned long *) 0x40025510))


# define GPIO_PORTF_IS_R  (*(( volatile unsigned long *) 0x40025404 ))
# define GPIO_PORTF_IBE_R (*(( volatile unsigned long *) 0x40025408 ))
# define GPIO_PORTF_IEV_R (*(( volatile unsigned long *) 0x4002540C ))
# define GPIO_PORTF_IM_R  (*(( volatile unsigned long *) 0x40025410 ))
# define GPIO_PORTF_ICR_R (*(( volatile unsigned long *) 0x4002541C ))

#define GPIO_PORTF_PIN0_EN 	0x01
#define GPIO_PORTF_PIN1_EN  0x02
#define GPIO_PORTF_PIN2_EN  0x04	
#define GPIO_PORTF_PIN4_EN 	0x10
#define NVIC_EN0_INT30 0x40000000

// Timer config and mode bit field definitions
# define TIM_16_BIT_CONFIG 		0x00000004 // 16 - bit timer
# define TIM_PERIODIC_MODE	  0x00000002 // Periodic timer mode
# define TIM_A_ENABLE 				0x00000001 // Timer A enable control
# define TIM_B_ENABLE 				0x00000100 // Timer B enable control
# define TIM_PWM_MODE 				0x0000000A // Timer in PWM mode
# define TIM_CAPTURE_MODE 		0x00000004 // Timer capture mode

// Timer1 A reload value for 1 kHz PWM frequency
# define TIM_A_INTERVAL 			16000 // Timer reload value for
# define TIM_B_INTERVAL 			16000 // Timer reload value for

// 1 kHz PWM frequency at
// clock frequency of 16 MHz
// 16 ,000 ,000/16000
// = 1 kHz
# define TIM_A_MATCH 					4000 // Timer match value for 75%
# define TIM_B_MATCH 					4000 // Timer match value for 75%
// duty cycle
volatile unsigned long i=0;


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



void delay (unsigned long value) {
				unsigned long i;
				for ( i=0 ; i < value ; i++);
}


// Timer and GPIO intialization and configuration
void Timer1A_Init ( void )
{
	// Enable the clock for port F and Timer1
	SYSCTL_RCGC_GPIO_R  |= CLOCK_GPIOF ;
	SYSCTL_RCGC_TIMER_R |= 0x02 ;

	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R  |= 0x01;	
		
	GPIO_PORTF_AFSEL_R |= 0x0C ;
	GPIO_PORTF_PCTL_R |= 0x00007700 ; // Configure as timer CCP pin
	GPIO_PORTF_DEN_R |= 0x1F ;
	GPIO_PORTF_DIR_R &=  ~(GPIO_PORTF_PIN0_EN);
	GPIO_PORTF_DIR_R &=  ~(GPIO_PORTF_PIN4_EN);	
	GPIO_PORTF_PUR_R |= GPIO_PORTF_PIN0_EN; 
	GPIO_PORTF_PUR_R |= GPIO_PORTF_PIN4_EN;


	
	// Enable the clock for Timer 1
GPTM_CONTROL_R &= ~( TIM_A_ENABLE ); // disable timer 1 A
GPTM_CONTROL_R &= ~( TIM_B_ENABLE ); // disable timer 1 B
// Timer1 A configured as 16 - bit timer
GPTM_CONFIG_R |= TIM_16_BIT_CONFIG ;
// Timer1 A in periodic timer , edge count and PWM mode
GPTM_TA_MODE_R |= TIM_PWM_MODE ;
GPTM_TB_MODE_R |= TIM_PWM_MODE ;
		
// Make PWM frequency 1 kHz using reload value of 16000
GPTM_TA_IL_R = TIM_A_INTERVAL ;
GPTM_TB_IL_R = TIM_B_INTERVAL ;
// Configure PWM duty cycle value ( should be less than 16000)
GPTM_TA_MATCH_R = TIM_A_MATCH ;
GPTM_TB_MATCH_R = TIM_B_MATCH ;
// Enable timer1 A and B
GPTM_CONTROL_R |= TIM_A_ENABLE ;
GPTM_CONTROL_R |= TIM_B_ENABLE ;

	
	
	
	
	
	
	/* INTERRUPT Configuration */
	/* Globally disable all interrupts */
	DisableInterrupts();
	/* Disable all interrupts on PortF */
	GPIO_PORTF_IM_R &= 0x00;               /*#####################################*/	
	/* Enable PortF interrupt in NVIC */
	
	NVIC_EN0_R = NVIC_EN0_INT30;	
	/* Set priority of PortF as 5 */
	NVIC_PRI7_R |= 0x00A00000;	
	

	
	/* Configure PF4 and PF0 as edge sensitive,
	* not both edges,
	* falling edge */
	GPIO_PORTF_IS_R  &= ~(GPIO_PORTF_PIN0_EN);
	GPIO_PORTF_IBE_R &= ~(GPIO_PORTF_PIN0_EN); 
	GPIO_PORTF_IEV_R &= ~(GPIO_PORTF_PIN0_EN);
	
	GPIO_PORTF_IS_R  &= 0x00;
	GPIO_PORTF_IBE_R &= 0x00; 
	GPIO_PORTF_IEV_R &= 0x00;
	
	/* Clear interrupt flag for PF4  and PF0*/
	GPIO_PORTF_ICR_R |= GPIO_PORTF_PIN0_EN;	
	GPIO_PORTF_ICR_R |= GPIO_PORTF_PIN4_EN;	
	
	/* Enable interrupt on PF4 and PF0 */
	GPIO_PORTF_IM_R |= GPIO_PORTF_PIN0_EN;
	GPIO_PORTF_IM_R |= GPIO_PORTF_PIN4_EN;

	/* Enable interrupts beyond the defined priority level */
	EnablePriorityInterrupts();
	/* Globally enable all interrupts */
	EnableInterrupts();

	
	
	

}


/* Interrupt Service Routine for PortF */
void GPIOF_Handler(void){
		int j;
		/* Clear interrupt flag for PF4 */
		GPIO_PORTF_ICR_R |= GPIO_PORTF_PIN0_EN;
		
		/* Turn on different LEDs on each button press */
		if(i>=3)
		i=1;
		else
		i++;
		for(j=0;j<2;j++)
		{
			GPIO_PORTF_DATA_R^=1<<i;
			delay(1000000);
}
}

// Application main function
int main(){
	Timer1A_Init();
	while(1)
	{
		WaitForInterrupt();
	}
}
