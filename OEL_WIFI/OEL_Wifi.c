#include "TM4C123.h"

//Register definitions for ClockEnable
#define 	SYSCTL_RCGC_UART_R				(*((volatile unsigned long*)0x400FE618))
#define 	SYSCTL_RCGC_GPIO_R				(*((volatile unsigned long*)0x400FE608))

/***************************** GPIO Configuration *********************************/

//Register definitions for GPIOPortA
#define 	GPIO_PORTA_AFSEL_R				(*((volatile unsigned long*)0x40004420))
#define 	GPIO_PORTA_PCTL_R				(*((volatile unsigned long*)0x4000452C))
#define 	GPIO_PORTA_DEN_R				(*((volatile unsigned long*)0x4000451C))
#define 	GPIO_PORTA_DIR_R				(*((volatile unsigned long*)0x40004400))
#define 	GPIO_PORTA_DATA_R				(*((volatile unsigned long*)0x400043FC))

//Register definitions for GPIOPortD
#define 	GPIO_PORTD_AFSEL_R			    (*((volatile unsigned long*)0x40007420))
#define 	GPIO_PORTD_PCTL_R				(*((volatile unsigned long*)0x4000752C))	
#define 	GPIO_PORTD_DEN_R				(*((volatile unsigned long*)0x4000751C))	
#define 	GPIO_PORTD_DIR_R				(*((volatile unsigned long*)0x40007400))	

#define 	GPIO_PORTD_LOCK_R				(*((volatile unsigned long*)0x40007520))	
#define 	GPIO_PORTD_CR_R					(*((volatile unsigned long*)0x40007524))

/***************************** UART Configuration *********************************/

//Register definitions for UART0_ module (Base address: 0x4000C000)
#define 	UART0_CTL_R					    (*((volatile unsigned long*)0x4000C030))
#define  	UART0_IBRD_R					(*((volatile unsigned long*)0x4000C024))
#define  	UART0_FBRD_R					(*((volatile unsigned long*)0x4000C028))
#define 	UART0_LCRH_R					(*((volatile unsigned long*)0x4000C02C))
#define 	UART0_CC_R					    (*((volatile unsigned long*)0x4000CFC8))
#define 	UART0_FR_R					    (*((volatile unsigned long*)0x4000C018))
#define 	UART0_DR_R					    (*((volatile unsigned long*)0x4000C000))

//Register definitions for UART2_ module (Base address: 0x4000_E000)
#define 	UART2_CTL_R						(*((volatile unsigned long*)0x4000E030))
#define  	UART2_IBRD_R					(*((volatile unsigned long*)0x4000E024))
#define  	UART2_FBRD_R					(*((volatile unsigned long*)0x4000E028))
#define 	UART2_LCRH_R					(*((volatile unsigned long*)0x4000E02C))
#define 	UART2_CC_R						(*((volatile unsigned long*)0x4000EFC8))
#define 	UART2_FR_R						(*((volatile unsigned long*)0x4000E018))
#define 	UART2_DR_R						(*((volatile unsigned long*)0x4000E000))

/************************************ MACROS **************************************/

#define 	UART_FR_TX_FF		0x20					    //UART Transmit FIFO Full
#define 	UART_FR_RX_FE		0x10						//UART Receive FIFO Empty

// To enable floating point for __main that needs it
void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}

// Functions Definitions
//unsigned char UART0_Rx(void);
void UART0_Tx(unsigned char data);
void UART0_TxString(char*pt);
//void UART0_RxString(char*bufPt, unsigned short max);

unsigned char UART2_Rx(void);
void UART2_Tx(unsigned char data);
void UART2_TxString(char*pt);
void UART2_RxString(char*bufPt, unsigned short max);

void Delay(unsigned long value);

/************************************* CODE ***************************************/
//Intialize and configure UART
void UARTInit(void)
{
    volatile unsigned delay_clk;

	// Enable clock for UART0_/UART2_ and GPIO PortA/PORTD
	SYSCTL_RCGC_UART_R  |= 0x05;
    delay_clk = SYSCTL_RCGC_GPIO_R;

	SYSCTL_RCGC_GPIO_R  |= 0x09;
	delay_clk = SYSCTL_RCGC_GPIO_R;

    // Unlocking CR to enable PD7 alternate functionality (U2Tx)
	GPIO_PORTD_LOCK_R = 0x4C4F434B; 
	GPIO_PORTD_CR_R |= 0xFF;
	
	// Configure GPIO as UART0 (AFSEL,PCTL,DEN)
	GPIO_PORTA_AFSEL_R |= 0x03;
	GPIO_PORTA_PCTL_R  |= 0x00000011;
	GPIO_PORTA_DEN_R   |= 0x03;
	
	GPIO_PORTA_DEN_R   |= 0x04;
	GPIO_PORTA_DIR_R   |= 0x04;
	GPIO_PORTA_DATA_R   |= 0x04;

    // Configure GPIO as UART2 (AFSEL,PCTL,DEN)
	GPIO_PORTD_DEN_R   |= 0xC0;
	GPIO_PORTD_AFSEL_R |= 0xC0;
	GPIO_PORTD_PCTL_R  |= 0x11000000;

	//Disable UART
	UART0_CTL_R &= 0x0300;
    UART2_CTL_R &= 0x0300;
	
	//Select system clock/PIOSC as UART Baud clock
	UART0_CC_R &= 0x0;
    UART2_CC_R &= 0x0;
	
	//Set Baud Rate 115200
	UART0_IBRD_R |= 0x0008; //9600 -> 00 68, 0b
	UART0_FBRD_R |= 0x2C;
	
    UART2_IBRD_R |= 0x0008;
	UART2_FBRD_R |= 0x2C; 
	
	//8bit word length,no parity bit,one stop bit,FIFOs enable
	UART0_LCRH_R |= 0x70;
    UART2_LCRH_R &= ~(0x2);
	UART2_LCRH_R &= ~(0x8);
	UART2_LCRH_R |= 0x10;
	UART2_LCRH_R |= 0x60;
	
	//Enable UART
	UART0_CTL_R |= 0x0301;
    UART2_CTL_R |= 0x0301;
}

void Delay(unsigned long value){
	unsigned long i=0;
	for(i=0;i<value;i++);
}


//Output 8bit to serial port
void UART0_Tx(unsigned char data)
{
	while((UART0_FR_R & UART_FR_TX_FF)!=0);
	UART0_DR_R=data;
}


//Output a character string to serial port
void UART0_TxString(char *pt)
{
	while(*pt)
    {
        UART0_Tx(*pt);
        pt++;
    }
}

//Wait for input and returns its ASCII value
unsigned char UART2_Rx(void)
{
	while((UART2_FR_R & UART_FR_RX_FE)!=0);
	return((unsigned char)(UART2_DR_R & 0xFF));
}

//Output 8bit to serial port
void UART2_Tx(unsigned char data)
{
	while((UART2_FR_R & UART_FR_TX_FF)!=0);
	UART2_DR_R=data;
  //UART0_Tx(data);
}

//Output a character string to serial port
void UART2_TxString(char *pt)
{
	while(*pt){
		UART2_Tx(*pt);
		pt++;
	}
}

void wifi_module(void)
{	
  char char_;
	UART2_TxString("AT+RST\r\n");
	char_ = UART2_Rx();
	while(char_ != 'y'){ //char_ != 'y'
		char_ = UART2_Rx();
		UART0_Tx(char_);
	}
	
	UART2_TxString("AT+CWMODE=3\r\n");
	char_ = UART2_Rx();
	while(char_ != 'K'){ //char_ != 'K'
		char_ = UART2_Rx();
		UART0_Tx(char_);
	}
	
	UART2_TxString("AT+CWJAP=\"fazi\",\"98765432\"\r\n");
	char_ = UART2_Rx();
	while(char_ != 'K'){ //char_ != 'K'
		char_ = UART2_Rx();
		UART0_Tx(char_);
	}
	
	UART2_TxString("AT+CIFSR\r\n");
	char_ = UART2_Rx();
	while(char_ != 'K'){ //char_ != 'K'
		char_ = UART2_Rx();
		UART0_Tx(char_);
	}
	
	UART2_TxString("AT+CIPMUX=1\r\n");
	char_ = UART2_Rx();
	while(char_ != 'K'){ //char_ != 'K'
		char_ = UART2_Rx();
		UART0_Tx(char_);
	}
	
	UART2_TxString("AT+CIPSERVER=1,80\r\n");
	char_ = UART2_Rx();
	while(char_ != 'K'){ //char_ != 'K'
		char_ = UART2_Rx();
		UART0_Tx(char_);
	}
	Delay(100000000);
	
	UART2_TxString("AT+CIPSTATUS\r\n");
	char_ = UART2_Rx();
	while(char_ != 'K'){ //char_ != 'K'
		char_ = UART2_Rx();
		UART0_Tx(char_);
	}
	
	UART2_TxString("AT+CIPSEND=0,6\r\n");
	char_ = UART2_Rx();
	while(char_ != 'x'){ //char_ != 'K'
		char_ = UART2_Rx();
		UART0_Tx(char_);
		if (char_ == '>'){
				UART2_TxString("FAZAIL:\r\n");
				UART2_TxString("AT+CIPCLOSE=0\r\n");
				}
	}
	//UART2_TxString("exit\r\n");
}

int main(void)
{

	char string[17];
	int u;
	UARTInit();

    UART0_TxString("\r\nWifi Initialization\r\n");
    UART0_TxString("Execute\r\n");
		Delay(1000);
    wifi_module();

while(1)
	{
		//UART0_TxString("Hello FAZAIL ALI:\r\n");
		
	}
}
