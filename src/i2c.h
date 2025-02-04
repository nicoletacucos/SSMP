#include "MKL25Z4.h"

void i2c_write_byte (uint8_t dev, uint8_t reg, uint8_t data);
uint8_t i2c_read_byte(uint8_t dev, uint8_t reg);
int i2c_read_bytes(uint8_t dev_adx, uint8_t reg_adx, uint8_t * data, int8_t data_count);
void i2c_init(void);
uint8_t init_sensor(void);
int16_t read_full_x();
int16_t read_full_y();
void calibrate_x();
