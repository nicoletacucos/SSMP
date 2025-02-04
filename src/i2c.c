#include "i2c.h"
#include "Uart.h"
#include <stdlib.h>
#include <string.h>

//sensor registers
#define MMA_ADDR 0x1D<<1
#define REG_WHOAMI 0x0D 
#define REG_SYSMOD 0x0B
#define REG_CTRL1 0x2A

#define DEVICE_ID 0x1A
#define REG_XHI 0x01
#define REG_YHI 0x03

//I2C macros
#define I2C_M_START   I2C0->C1 |= I2C_C1_MST_MASK
#define I2C_M_STOP    I2C0->C1 &= ~I2C_C1_MST_MASK
#define I2C_M_RSTART  I2C0->C1 |= I2C_C1_RSTA_MASK

#define I2C_TRAN      I2C0->C1 |= I2C_C1_TX_MASK
#define I2C_REC       I2C0->C1 &= ~I2C_C1_TX_MASK

#define I2C_WAIT      while((I2C0->S & I2C_S_IICIF_MASK)==0) {} \
                      I2C0->S |= I2C_S_IICIF_MASK;

#define NACK          I2C0->C1 |= I2C_C1_TXAK_MASK
#define ACK           I2C0->C1 &= ~I2C_C1_TXAK_MASK
 
int16_t x_offset = 0;
int16_t y_offset = 0;

void i2c_init(void)
{
  //clock i2c peripheral and port E
  SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

  //set pins to I2C function
  PORTE->PCR[24] |= PORT_PCR_MUX(5);
  PORTE->PCR[25] |= PORT_PCR_MUX(5);
	
  // set to 400k baud
  // I2C baud = bus freq/(scl_div*mul) 
  // 48MHz/400kHz = 60; icr=0x11 sets scl_div to 56
  I2C0->F = I2C_F_ICR(0x11) | I2C_F_MULT(0);    //mul=1
	
  //enable i2c and set to master mode
  I2C0->C1 = (I2C_C1_IICEN_MASK);

  //select high drive mode
  I2C0->C2 = (I2C_C2_HDRS_MASK); 
	
}

void Delay(unsigned int time_del) {
    time_del = 6000 * time_del; //48.000.000 cycles per sec
    while (time_del--) {
         __asm__("nop");
    }
}


uint8_t init_sensor() {
  //check for device	
	
		uint8_t whoiam = i2c_read_byte(MMA_ADDR, REG_WHOAMI);
		//UART0_TransmitDecimalNr(whoiam);
		 
		//UART0_TransmitDecimalNr(i2c_read_byte(MMA_ADDR, 0X0B));
	
    if (whoiam == DEVICE_ID) { 
				Delay(40);
        //set active mode, 14 bit samples and 800 Hz ODR
        i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x01);
		

        return 1; 
    } else { 
        //else error
        return 0;
    } 
}

void i2c_write_byte (uint8_t dev, uint8_t reg, uint8_t data)
{
  I2C_TRAN;     /*set to transmit mode*/
  I2C_M_START;  /*send start */

  I2C0->D = dev;   /*send dev address (write)*/
  I2C_WAIT      /*wait for ack*/
  I2C0->D = reg;   /*send register address*/
  I2C_WAIT

  I2C0->D = data;  /*send data*/
  I2C_WAIT

  I2C_M_STOP; 
}

uint8_t i2c_read_byte(uint8_t dev, uint8_t reg) {
  uint8_t data;

  I2C_TRAN;             /*set to transmit mode */
  I2C_M_START;          /*send start */

  I2C0->D = dev;         /*send dev address (write)*/
  I2C_WAIT              /*wait for completion */

  I2C0->D = reg;        /*send register address */
  I2C_WAIT              /*wait for completion */

  I2C_M_RSTART;        /*repeated start */
  I2C0->D = (dev|0x1);  /*send dev address (read) */
  I2C_WAIT              /*wait for completion */

  I2C_REC;              /*set to receive mode */
  NACK;                 /*set NACK after read to read only one byte*/

  data = I2C0->D;         /*dummy read */
  I2C_WAIT              /*wait for completion */

  I2C_M_STOP;           /*send stop */
  data = I2C0->D;       /*read data */

  return data;
}

int i2c_read_bytes(uint8_t dev_adx, uint8_t reg_adx, uint8_t * data, int8_t data_count) {
  uint8_t dummy;
  int8_t num_bytes_read=0;

  I2C_TRAN;             /* set to transmit mode */
  I2C_M_START;          /* send start */

  I2C0->D = dev_adx;     /* send dev address (write) */
  I2C_WAIT              /* wait for completion */

  I2C0->D = reg_adx;     /* send register address */
  I2C_WAIT              /* wait for completion */

  I2C_M_RSTART;        /* repeated start */
  I2C0->D = dev_adx|0x01; /* send dev address (read) */
  I2C_WAIT              /* wait for completion */

  I2C_REC;              /* set to receive mode */

  ACK;                  /* tell HW to send ACK after read */
  dummy = I2C0->D;      /* dummy read to start I2C read */
  I2C_WAIT              /* wait for completion */

  do {
    data[num_bytes_read++] = I2C0->D; /* read data */
    ACK;                            /* tell HW to send ACK after read */ 
    I2C_WAIT                        /* wait for completion */
  } while (num_bytes_read < data_count-1); 

  NACK;                              /* tell HW to send NACK after read */
  data[num_bytes_read++] = I2C0->D; /* read data */
  I2C_WAIT                          /* wait for completion */
  I2C_M_STOP;                       /* send stop */

  return 1;
}


int16_t read_full_x() {
  uint8_t data[2];
  int16_t x;
  float x_g;

  i2c_read_bytes(MMA_ADDR, REG_XHI, data, 2);

  //UART0_Transmit(0x0D);
  //UART0_Transmit(0x0A);

  x = (int16_t)((data[0] << 8) | data[1]);

  UART0_TransmitString("Raw X: ");
  UART0_TransmitInt16(x);
  UART0_Transmit(0x0D);
  UART0_Transmit(0x0A);

  x -= x_offset;

  // Normalizare - impartire la sensibilitate ( ±2g range)
  x_g = (float)x / 4096.0f;
  

  // Afisare valoare in g
  UART0_TransmitString("X (g): ");
  int16_t x_g_int = (int16_t)(x_g * 100);
  //UART0_TransmitInt16(x_g_int);
  UART0_TransmitInt16(x_g_int / 100);  // Partea intreaga
  UART0_Transmit('.');                
  int16_t fractional_part = abs(x_g_int % 100); //parte zecimala
  if (fractional_part < 10) {
      UART0_Transmit('0');
  }
  UART0_TransmitInt16(fractional_part);

  UART0_Transmit(0x0D);
  UART0_Transmit(0x0A);

  return x_g_int; 
}

int16_t read_full_y() {
  uint8_t data[2];
  int16_t y;
  float y_g;

  i2c_read_bytes(MMA_ADDR, REG_YHI, data, 2);

  y = (int16_t)((data[0] << 8) | data[1]);

  UART0_TransmitString("Raw Y: ");
  UART0_TransmitInt16(y);
  UART0_Transmit(0x0D);
  UART0_Transmit(0x0A);

  y -= y_offset;

  // Normalizare - impartire la sensibilitate ( ±2g range)
  y_g = (float)y / 4096.0f;

  // Afisare valoare in g
  UART0_TransmitString("Y (g): ");
  int16_t y_g_int = (int16_t)(y_g * 100);
  UART0_TransmitInt16(y_g_int);

  UART0_TransmitInt16(y_g_int / 100);  // Partea intreaga
  UART0_Transmit('.');                
  int16_t fractional_part = abs(y_g_int % 100); //parte zecimala
  if (fractional_part < 10) {
      UART0_Transmit('0');
  }
  UART0_TransmitInt16(fractional_part);

  UART0_Transmit(0x0D);
  UART0_Transmit(0x0A);

  return y; 
}

void calibrate_x() {
  uint8_t data[2];
  
  UART0_TransmitString("Calibrating...\r\n");
  Delay(1000);

  i2c_read_bytes(MMA_ADDR, REG_XHI, data, 2);
  x_offset = (int16_t)((data[0] << 8) | data[1]);

  UART0_TransmitString("X Offset: ");
  UART0_TransmitInt16(x_offset);
  UART0_Transmit(0x0D);
  UART0_Transmit(0x0A);

  
  
  // i2c_read_bytes(MMA_ADDR, REG_YHI, data, 2);
  // y_offset = (int16_t)((data[0] << 8) | data[1]);

  // UART0_TransmitString("Y Offset: ");
  // UART0_TransmitInt16(y_offset);
  // UART0_Transmit(0x0D);
  // UART0_Transmit(0x0A);
}


