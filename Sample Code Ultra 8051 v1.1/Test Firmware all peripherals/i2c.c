#include "delay.h"
#include "i2c.h"
#include "stdutils.h"
//#include "delay.c"


/***************************************************************************************************
                          Local Function declaration
***************************************************************************************************/						  
static void i2c_Clock(void);
static void i2c_Ack();
static void i2c_NoAck();
/**************************************************************************************************/


void I2C_Init()
 {
    
 
 }

void I2C_Start()
{

    //    I2C_PORT &= ~(1<<SCL);
	util_BitClear(I2C_Port, SCL);        // Pull SCL low
	util_BitSet(I2C_Port, SDA);       // Pull SDA High
	DELAY_us(1);
	util_BitSet(I2C_Port, SCL);     //Pull SCL high
	DELAY_us(1);
	util_BitClear(I2C_Port, SDA);      //Now Pull SDA LOW, to generate the Start Condition
	DELAY_us(1);
	util_BitClear(I2C_Port, SCL);        //Finally Clear the SCL to complete the cycle
	

}





/***************************************************************************************************
                         void I2C_Stop()
****************************************************************************************************
 * I/P Arguments: none.
 * Return value  : none

 * description  :This function is used to generate I2C Stop Condition.
                 Stop Condition: SDA goes High when SCL is High.

                               ____________
                SCL:          |            |
                      ________|            |______
                                 _________________
                SDA:            |
                      __________|

***************************************************************************************************/

void I2C_Stop(void)
{

	util_BitClear(I2C_Port, SCL);            // Pull SCL low
	DELAY_us(1);
	util_BitClear(I2C_Port, SDA);          // Pull SDA  low
	DELAY_us(1);
	util_BitSet(I2C_Port, SCL);         // Pull SCL High
	DELAY_us(1);
	util_BitSet(I2C_Port, SDA);           // Now Pull SDA High, to generate the Stop Condition
}








/***************************************************************************************************
                         void I2C_Write(uint8_t var_i2cData_u8)
****************************************************************************************************
 * I/P Arguments: uint8_t-->8bit data to be sent.
 * Return value  : none

 * description  :This function is used to send a byte on SDA line using I2C protocol
                 8bit data is sent bit-by-bit on each clock cycle.
                 MSB(bit) is sent first and LSB(bit) is sent at last.
                 Data is sent when SCL is low.

         ___     ___     ___     ___     ___     ___     ___     ___     ___     ___
 SCL:   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
      __|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___

 SDA:    D8       D7     D6      D5      D4       D3      D2      D1      D0     ACK


***************************************************************************************************/
void I2C_Write(uint8_t var_i2cData_u8)
{
	uint8_t i;
	util_BitClear(I2C_Port,SDA); //output

	for(i=0;i<8;i++)                   // loop 8 times to send 1-byte of data
	{
		if(util_IsBitSet(var_i2cData_u8,7))
		{
		  	util_BitSet(I2C_Port,SDA);
		}	
		else	
		{
		util_BitClear(I2C_Port,SDA);
		}
			

		i2c_Clock();                   // Generate Clock at SCL
		var_i2cData_u8 = var_i2cData_u8<<1;// Bring the next bit to be transmitted to MSB position
	}
                             
	i2c_Clock();
}






/***************************************************************************************************
                         uint8_t I2C_Read(uint8_t var_ackOption_u8)
****************************************************************************************************
 * I/P Arguments: none.
 * Return value  : uint8_t(received byte)

 * description :This fun is used to receive a byte on SDA line using I2C protocol.
               8bit data is received bit-by-bit each clock and finally packed into Byte.
               MSB(bit) is received first and LSB(bit) is received at last.


         ___     ___     ___     ___     ___     ___     ___     ___     ___     ___
SCL:    |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
      __|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |__

 SDA:    D8       D7     D6      D5       D4     D3       D2      D1     D0      ACK


***************************************************************************************************/
uint8_t I2C_Read(uint8_t var_ackOption_u8)
{
	uint8_t i, var_i2cData_u8=0x00;

	util_BitSet(I2C_Port,SDA);              //Make SDA as I/P
	for(i=0;i<8;i++)     // loop 8times read 1-byte of data
	{
		DELAY_us(1);
		util_BitSet(I2C_Port, SCL);      // Pull SCL High
		DELAY_us(1);

		var_i2cData_u8 = var_i2cData_u8<<1;    //var_i2cData_u8 is Shifted each time and
		var_i2cData_u8 = var_i2cData_u8 | util_GetBitStatus(I2C_Port, SDA); //ORed with the received bit to pack into byte

		util_BitClear(I2C_Port, SCL);         // Clear SCL to complete the Clock
	}
	if(var_ackOption_u8==1)  /*Send the Ack/NoAck depending on the user option*/
	{
		i2c_Ack();
	}
	else
	{
		i2c_NoAck();
	}

	return var_i2cData_u8;           // Finally return the received Byte*
}






/***************************************************************************************************
                                Local functions
***************************************************************************************************/
								

/***************************************************************************************************
                         static void i2c_Clock()
****************************************************************************************************
 * I/P Arguments: none.
 * Return value  : none

 * description  :This function is used to generate a clock pulse on SCL line.
***************************************************************************************************/
static void i2c_Clock(void)
{
	DELAY_us(1);
	util_BitSet(I2C_Port, SCL);         // Wait for Some time and Pull the SCL line High
	DELAY_us(1);        // Wait for Some time
	util_BitClear(I2C_Port, SCL);            // Pull back the SCL line low to Generate a clock pulse
}





/***************************************************************************************************
                         static void i2c_Ack()
 ***************************************************************************************************
 * I/P Arguments: none.
 * Return value  : none

 * description  :This function is used to generate a the Positive ACK
                 pulse on SDA after receiving a byte.
***************************************************************************************************/
static void i2c_Ack()
{
	util_BitClear(I2C_Port, SDA);      //Pull SDA low to indicate Positive ACK
	i2c_Clock();    //Generate the Clock
	util_BitSet(I2C_Port, SDA);       // Pull SDA back to High(IDLE state)
}





/***************************************************************************************************
                        static void i2c_NoAck()
 ***************************************************************************************************
 * I/P Arguments: none.
 * Return value  : none

 * description  :This function is used to generate a the Negative/NO ACK
                 pulse on SDA after receiving all bytes.
***************************************************************************************************/
static void i2c_NoAck()
{
	util_BitSet(I2C_Port, SDA);        //Pull SDA high to indicate Negative/NO ACK
	i2c_Clock();     // Generate the Clock  
	util_BitSet(I2C_Port, SCL);      // Set SCL 
}


