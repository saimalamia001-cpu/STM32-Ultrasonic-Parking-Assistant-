#ifndef __LCD_I2C_H
#define __LCD_I2C_H

#include "stm32f4xx_hal.h"

void lcd_init(I2C_HandleTypeDef *hi2c);
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);
void lcd_clear(void);
void lcd_put_cur(int row, int col);
void lcd_send_string(char *str);

#endif
