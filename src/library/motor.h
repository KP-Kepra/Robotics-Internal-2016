#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"	  
#include "stm32f10x_tim.h"
#include "IR_sensors.h"
#include "delay.h"
#include "ticks.h"

typedef enum{
	MOTOR1,
	MOTOR2,
	MOTOR3
}MOTOR_ID;

void motor_init(u16 presc, u16 count, u16 init_val);
void motor_control(MOTOR_ID motor_id, u8 dir, u16 magnitude);

void move_forward(u16 magnitude);
void move_backward(u16 magnitude);

void turn_half(char turn, u16 magnitude);
void turn_atplace(char turn, u16 magnitude);
void turn_90degree(char turn, u16 magnitude);
void turn_180degree(char turn, u16 magnitude);

void motor_stop(void);

extern int dir;
extern int ir_left;
extern int ir_right;
extern int lean;
#endif
