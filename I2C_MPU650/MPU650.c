#include "TM4C123.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
/***************************************************************************************
UART0 Addresses
****************************************************************************************/
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
/*********************************************************************************************************
I2C3 Addresses
*********************************************************************************************************/
// Clock gating control related register definitions
# define RCGC_I2C_R 	(*(( volatile unsigned long *) 0x400FE620 ))
# define CLOCK_GPIOD 		0x00000008 // Port D clock control
# define CLOCK_I2C3  		0x00000008 // I2C3 clock control
// I2C base address and register definitions
# define I2C_BASE 				0x40023000
# define I2CM_SLAVE_ADDR_R   		(*( volatile long *)( I2C_BASE + 0x000 ))
# define I2CM_CONTROL_R 				(*( volatile long *)( I2C_BASE + 0x004 ))
# define I2CM_STATUS_R 					(*( volatile long *)( I2C_BASE + 0x004 ))
# define I2CM_DATA_R 						(*( volatile long *)( I2C_BASE + 0x008 ))
# define I2CM_TIME_PERIOD_R 		(*( volatile long *)( I2C_BASE + 0x00C ))
# define I2CM_CONFIG_R 					(*( volatile long *)( I2C_BASE + 0x020 ))
# define I2CM_BUS_MONITOR_R 		(*( volatile long *)( I2C_BASE + 0x02C ))
# define I2CM_CONFIG2_R 				(*( volatile long *)( I2C_BASE + 0x038 ))
// GPIO Port D alternate function configuration
# define GPIO_PORTD_AFSEL_R 	(*(( volatile unsigned long *)0x40007420 ))
# define GPIO_PORTD_PCTL_R 		(*(( volatile unsigned long *)0x4000752C ))
# define GPIO_PORTD_DEN_R 		(*(( volatile unsigned long *)0x4000751C ))
# define GPIO_PORTD_OD_R 			(*(( volatile unsigned long *)0x4000750C ))
// I2C bit field definitions
# define I2C_ENABLE 				0x01
# define BUSY_STATUS_FLAG 			0x01
// I2C command definitions for control register
# define CMD_SINGLE_SEND 			0x00000007
# define CMD_SINGLE_RECEIVE 		0x00000007
# define CMD_BURST_SEND_START 		0x00000003
# define CMD_BURST_SEND_CONT 		0x00000001
# define CMD_BURST_SEND_FINISH 		0x00000005
/**********************************************************************************************************/
#define XG_OFFS_TC          0x00
#define YG_OFFS_TC          0x01
#define ZG_OFFS_TC          0x02
#define X_FINE_GAIN         0x03
#define Y_FINE_GAIN         0x04
#define Z_FINE_GAIN         0x05
#define XA_OFFS_H           0x06 
#define XA_OFFS_L_TC        0x07
#define YA_OFFS_H           0x08 
#define YA_OFFS_L_TC        0x09
#define ZA_OFFS_H           0x0A 
#define ZA_OFFS_L_TC        0x0B
#define XG_OFFS_USRH        0x13
#define XG_OFFS_USRL        0x14
#define YG_OFFS_USRH        0x15
#define YG_OFFS_USRL        0x16
#define ZG_OFFS_USRH        0x17
#define ZG_OFFS_USRL        0x18
#define SMPLRT_DIV          0x19
#define CONFIG              0x1A
#define GYRO_CONFIG         0x1B
#define ACCEL_CONFIG        0x1C
#define FF_THR              0x1D
#define FF_DUR              0x1E
#define MOT_THR             0x1F
#define MOT_DUR             0x20
#define ZRMOT_THR           0x21
#define ZRMOT_DUR           0x22
#define FIFO_EN             0x23
#define I2C_MST_CTRL        0x24
#define I2C_SLV0_ADDR       0x25
#define I2C_SLV0_REG        0x26
#define I2C_SLV0_CTRL       0x27
#define I2C_SLV1_ADDR       0x28
#define I2C_SLV1_REG        0x29
#define I2C_SLV1_CTRL       0x2A
#define I2C_SLV2_ADDR       0x2B
#define I2C_SLV2_REG        0x2C
#define I2C_SLV2_CTRL       0x2D
#define I2C_SLV3_ADDR       0x2E
#define I2C_SLV3_REG        0x2F
#define I2C_SLV3_CTRL       0x30
#define I2C_SLV4_ADDR       0x31
#define I2C_SLV4_REG        0x32
#define I2C_SLV4_DO         0x33
#define I2C_SLV4_CTRL       0x34
#define I2C_SLV4_DI         0x35
#define I2C_MST_STATUS      0x36
#define INT_PIN_CFG         0x37
#define INT_ENABLE          0x38
#define DMP_INT_STATUS      0x39
#define INT_STATUS          0x3A
#define ACCEL_XOUT_H        0x3B
#define ACCEL_XOUT_L        0x3C
#define ACCEL_YOUT_H        0x3D
#define ACCEL_YOUT_L        0x3E
#define ACCEL_ZOUT_H        0x3F
#define ACCEL_ZOUT_L        0x40
#define TEMP_OUT_H          0x41
#define TEMP_OUT_L          0x42
#define GYRO_XOUT_H         0x43
#define GYRO_XOUT_L         0x44
#define GYRO_YOUT_H         0x45
#define GYRO_YOUT_L         0x46
#define GYRO_ZOUT_H         0x47
#define GYRO_ZOUT_L         0x48
#define I2C_MST_DELAY_CTRL  0x67
#define SIGNAL_PATH_RESET   0x68
#define MOT_DETECT_CTRL     0x69
#define USER_CTRL           0x6A
#define PWR_MGMT_1          0x6B
#define PWR_MGMT_2          0x6C
#define BANK_SEL            0x6D
#define MEM_START_ADDR      0x6E
#define MEM_R_W             0x6F
#define DMP_CFG_1           0x70
#define DMP_CFG_2           0x71
#define FIFO_COUNTH         0x72
#define FIFO_COUNTL         0x73
#define FIFO_R_W            0x74
#define WHO_AM_I            0x75

/**********************************************************************************************************
Functions Definitions of Uart
***********************************************************************************************************/
//Function definitions
void I2C3_Init(void);
char I2C_Send_Data ( int Slave_addr ,char Data_addr ,  char Data );
char I2C_Receive_Data ( int Slave_addr ,char Data_addr , int byteCount, char* data );
void Delay(unsigned long counter);
void UARTInit(void);	
void UARTTx(unsigned char data);
void UARTTxString(char *pt);
void MPU6050_Init(void);
/*********************************************************************************************************/


void MPU6050_Init(void)
{
 I2C_Send_Data(0x68,SMPLRT_DIV, 0x07);
 I2C_Send_Data(0x68,PWR_MGMT_1,  0x01);
 I2C_Send_Data(0x68,CONFIG, 0x00);
 I2C_Send_Data(0x68,ACCEL_CONFIG,0x00); 
 I2C_Send_Data(0x68,GYRO_CONFIG,0x00);
 I2C_Send_Data(0x68,INT_ENABLE, 0x01);

}



/*******************************************************************************************************/
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
		GPIO_PORTA_DIR_R   |= 0x02;
		
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
/*********************************************************************************************/
// I2C intialization and GPIO alternate function configuration
void I2C3_Init ( void )
{
	SYSCTL_RCGC_GPIO_R |= CLOCK_GPIOD ; // Enable the clock for port D
	RCGC_I2C_R |= CLOCK_I2C3 ; // Enable the clock for I2C 3
	GPIO_PORTD_DEN_R |= 0x03 ; // Assert DEN for port D
	// Configure Port D pins 0 and 1 as I2C 3
	GPIO_PORTD_AFSEL_R |= 0x00000003 ;
	GPIO_PORTD_PCTL_R |= 0x00000033 ;
	GPIO_PORTD_OD_R |= 0x00000002 ; // SDA ( PD1 ) pin as open darin
	I2CM_CONFIG_R = 0x0010 ; // Enable I2C 3 master function
	/* Configure I2C 3 clock frequency
	(1 + TIME_PERIOD ) = SYS_CLK /(2*( SCL_LP + SCL_HP ) * I2C_CLK_Freq )
	TIME_PERIOD = 16 ,000 ,000/(2(6+4) *100000) - 1 = 7 */
	I2CM_TIME_PERIOD_R = 0x07 ;
}
/***********************************************************************************************/
//UART's Functions
/************************************************************************************************/
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

/*********************************************************************************************************/
/*****************************Delay Function*************************************************************/
void Delay(unsigned long counter)
{
	unsigned long i = 0;
	
	for(i=0; i< counter*1000; i++);
}
/********************************************************************************************************/

/**************************************************************************************************/
/* Wait until I2C master is not busy and return error code */
/* If there is no error, return 0 */

static int I2C_wait_till_done(void)
{
    while(I2CM_STATUS_R & BUSY_STATUS_FLAG);   /* wait until I2C master is not busy */
    return I2CM_STATUS_R & 0xE; /* return I2C error code */
}
/***************************************************************************************************/
/* byte write: S-(saddr+w)-ACK-maddr-ACK-data-ACK-P */
// This function writes one data byte to Data_addr of I2C slave
char I2C_Send_Data ( int Slave_addr ,char Data_addr , char Data )
{ char error;

	// Configure slave address for write operation , data and control
	I2CM_SLAVE_ADDR_R = ( Slave_addr << 1) ;
	I2CM_DATA_R = Data_addr ; // Data to be written
	I2CM_CONTROL_R = CMD_BURST_SEND_START ; // Command to be performed

	error = I2C_wait_till_done();       /* wait until write is complete */
			if (error) return error;

	// Configure data to be written and send command
	I2CM_DATA_R = Data ;
	I2CM_CONTROL_R = CMD_BURST_SEND_FINISH ;
	error = I2C_wait_till_done();       /* wait until write is complete */
	while(I2CM_STATUS_R & 0x40){}           /* wait until bus is not busy */
			error = I2CM_STATUS_R & 0xE;
			if (error) return error;

			return 0;       /* no error */
}

/**************************************************************************************************************/
char I2C_Receive_Data ( int Slave_addr ,  char Data_addr , int byteCount, char* data )
{

	 char error;
    
    if (byteCount <= 0)
        return 1;         /* no read was performed */

    /* send slave address and starting address */
	// Configure slave address for write operation , data and control
	I2CM_SLAVE_ADDR_R = ( Slave_addr << 1) ;
	I2CM_DATA_R = Data_addr ; // Data to be written
	I2CM_CONTROL_R = CMD_BURST_SEND_START ; // Command to be performed
	// Wait until master is done
	error = I2C_wait_till_done();
    if (error)
        return error;
	
	// Configure the slave address for read operation
	/* to change bus from write to read, send restart with slave addr */
	I2CM_SLAVE_ADDR_R = (( Slave_addr << 1) | 0x01 );
	if (byteCount == 1)             /* if last byte, don't ack */
        I2CM_CONTROL_R = CMD_SINGLE_RECEIVE;              /* -data-NACK-P */
    else                            /* else ack */
        I2CM_CONTROL_R = 0xB;            /* -data-ACK- */
    error = I2C_wait_till_done();
    if (error) return error;
	
	*data++ = I2CM_DATA_R;            /* store the data received */

    if (--byteCount == 0)           /* if single byte read, done */
    {
        while(I2CM_STATUS_R & 0x40);    /* wait until bus is not busy */
        return 0;       /* no error */
    }
	
	
	/* read the rest of the bytes */
    while (byteCount > 1)
    {
        I2CM_CONTROL_R = 9;              /* -data-ACK- */
        error = I2C_wait_till_done();
        if (error) return error;
        byteCount--;
        *data++ = I2CM_DATA_R;        /* store data received */
    }

    I2CM_CONTROL_R = CMD_BURST_SEND_FINISH 	;                  /* -data-NACK-P */
    error = I2C_wait_till_done();
    *data = I2CM_DATA_R;              /* store data received */
    while(I2CM_STATUS_R & 0x40);        /* wait until bus is not busy */
    
    return 0;       /* no error */
}


/*************************MAIN FUNCTION********************************************************/
char msg[20];
int main(void)
{
  short int  accX, accY, accZ, GyroX, GyroY, GyroZ; 
	float AX, AY, AZ,  GX, GY, GZ;
	char sensordata[14];
	I2C3_Init();
	Delay(1000);
	MPU6050_Init();
	Delay(1000);
	UARTInit();
	while(1)
	{	 
		I2C_Receive_Data(0x68, ACCEL_XOUT_H, 14, sensordata);
	 accX = (short int)( (sensordata[0] << 8 ) |sensordata[1] );
	 accY = (short int)( (sensordata[2] << 8 ) |sensordata[3] );
	 accZ = (short int)( (sensordata[4] << 8 ) |sensordata[5] );
	 //Temper = (int)( (sensordata[6] << 8 ) |sensordata[7] );
	 GyroX = (short int)( (sensordata[8] << 8 ) |sensordata[9] );
	 GyroY = (short int)( (sensordata[10] << 8 ) |sensordata[11] );
	 GyroZ = (short int)( (sensordata[12] << 8 ) |sensordata[13] );

		
			
	   // Convert The Readings
			AX = (float)accX/16384.0f;
			AY = (float)accY/16384.0f;
			AZ = (float)accZ/16384.0f;
			GX = (float)GyroX/131.0f;
			GY = (float)GyroY/131.0f;
			GZ = (float)GyroZ/131.0f;
	 // t = ((float)Temper/340.00f)+36.53f;
		 sprintf(msg,"Gx = %.2f \t",GX);
		 UARTTxString(msg);
			 sprintf(msg,"Gy = %.2f \t",GY);
		 UARTTxString(msg);
			 sprintf(msg,"Gz  = %.2f \t",GZ);
		 UARTTxString(msg);
			 sprintf(msg,"Ax  = %.2f \t",AX);
		 UARTTxString(msg);
			 sprintf(msg,"Ay  = %.2f \t",AY);
		 UARTTxString(msg);
			 sprintf(msg,"Az  = %.2f \r\n",AZ);
		 UARTTxString(msg);
			// sprintf(msg,"Temp  = %f \r\n",t);
		 //UARTTxString(msg);
		 Delay(1000);
	}
}
