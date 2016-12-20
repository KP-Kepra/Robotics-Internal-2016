#include "IR_sensors.h"
/*
 * For our shooter robot:
 * B6 is for #left# IR sensor
 * B7 is for #right# IR sensor
 * MOTOR1 is for #left# motor
 * MOTOR2 is for #right# motor
 * Dir 1 is forward
 * Dir 0 is backward
 * 0 is for white line (IPU)
 * 1 is for black line
 */

/** Initialization */
void IR_init() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
 * @brief:Read data(0/1)from IR sensors
 * @param: 0 means left IR sensor, 1 means right IR sensor
 * @retval: return 0/1
 * */
int read_IR(int pos) {
	if (pos == 0) {
		return (int)(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6));
	}
	else if (pos == 1) {
		return (int)(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7));
	}
}