
#include "TM4C123.h"
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


#define SW1 (*((volatile unsigned long *)0x40025040))
#define SW2 (*((volatile unsigned long *)0x40025004))


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
#define SWITCH1_PIN             0x01 // Assuming PF0 is used for switch 1
#define SWITCH2_PIN             0x10 // Assuming PF4 is used for switch 2
unsigned int duty;

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
	
// Configure PortF pin 2 as Timer1 A output
GPIO_PORTF_AFSEL_R |= 0x0000000C ;
GPIO_PORTF_PCTL_R |= 0x00007700 ; // Configure as timer CCP pin
GPIO_PORTF_DEN_R |= 0x0000000C ;
GPIO_PORTF_DIR_R &= ~0x11; // Set PF4 and PF0 as input
GPIO_PORTF_DEN_R |= 0x11;  // Enable digital I/O on PF4 and PF0
GPIO_PORTF_PUR_R |= 0x11;  // Enable pull-up resistors on PF4 and PF0
	
// Enable the clock for Timer 1
GPTM_CONTROL_R &= ~( TIM_A_ENABLE ); // disable timer 1 A
GPTM_CONTROL_R &= ~( TIM_B_ENABLE ); // disable timer 1 B
// Timer1 A configured as 16 - bit timer
GPTM_CONFIG_R |= TIM_16_BIT_CONFIG ;
// Timer1 A in periodic timer , edge count and PWM mode
GPTM_TA_MODE_R |= TIM_PWM_MODE ;
GPTM_TB_MODE_R |= TIM_PWM_MODE ;
	
GPTM_TA_MODE_R &= ~( TIM_CAPTURE_MODE );
GPTM_TB_MODE_R &= ~( TIM_CAPTURE_MODE );
	// Make PWM frequency 1 kHz using reload value of 16000
GPTM_TA_IL_R = TIM_A_INTERVAL ;
GPTM_TB_IL_R = TIM_B_INTERVAL ;
	
// Configure PWM duty cycle value ( should be less than 16000)
GPTM_TA_MATCH_R = TIM_A_MATCH ;
GPTM_TB_MATCH_R = TIM_B_MATCH ;
// Enable timer1 A
GPTM_CONTROL_R |= TIM_A_ENABLE ;
GPTM_CONTROL_R |= TIM_B_ENABLE ;
}
// Application main function
int main(void) {
int dutyblue =0;
int dutygreen =0;	

    Timer1A_Init();
   

    while (1) {
        // Check the state of Switch 1
        if ((SW1 & 0x10) == 0) {
            // Blue light (GPTM_TA_MATCH_R)
            GPIO_PORTF_AFSEL_R = 0x00000004;  // Set AFSEL for pin 2
            GPIO_PORTF_PCTL_R = 0x00000700;  // Configure as timer CCP pin
            GPIO_PORTF_DEN_R |= 0x00000004;   // Enable digital I/O on pin 2
            // Switch 1 is pressed, generate saw-tooth on PF2
            for (dutyblue = 0; dutyblue < 16000; dutyblue = dutyblue + 1)
            {
                GPTM_TA_MATCH_R = dutyblue;
                delay(300);
            }
        }
			
        //Check the state of Switch 2
        else if ((SW2 & 0x01) == 0) {
            // Green light (GPTM_TB_MATCH_R)
            GPIO_PORTF_AFSEL_R = 0x00000008; // Clear AFSEL for pin 2
            GPIO_PORTF_PCTL_R = 0x00007000; // Reset PCTL for pin 2
            GPIO_PORTF_DEN_R |= 0x00000008;  // Disable digital I/O on pin 2

            // Enable the clock for Timer 1B before setting GPTM_TB_MATCH_R
            SYSCTL_RCGC_TIMER_R |= 0x04;
            // Switch 2 is pressed, generate saw-tooth on PF3
            for (dutygreen = 0; dutygreen < 16000; dutygreen = dutygreen + 1)
            {
                GPTM_TB_MATCH_R = dutygreen;
                delay(300);
            }

        }
			
        else
        {
            // Neither switch is pressed, perform other tasks or wait
        }
        
        delay(1000);
        
        // Rest of the code remains the same for the decreasing part
        for (duty = 15999; duty > 1 ; duty = duty - 1)
        {
            GPTM_TA_MATCH_R = duty;
            delay(300);
					 GPTM_TB_MATCH_R = duty;
					 delay(300);
        }
    }

	}
	
