#include "ultrasonic.h"

GPIO_TypeDef* us_port = {GPIOA};
uc16 trig_pin = GPIO_Pin_11;
uc16 echo_pin = GPIO_Pin_12;
u32 start_time = 0;

/** Initialization  */
void ultrasonic_init() {
	//Peripheral Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//TRIG Pin Init
	GPIO_InitStructure.GPIO_Pin = trig_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//ECHO Pin Init
	GPIO_InitStructure.GPIO_Pin = echo_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void delay_read(void){
	for(int i = 0;i <4;++i){
		__asm("nop");
	}
}

/** Emit signal and return distance */
float ultrasonic_emit() {
	int internal_delay=get_real_ticks();
	
	/* Stop 2ms, emit 10ms, stop 2ms, use delay if needed */

		GPIO_ResetBits(us_port, trig_pin);
	_delay_us(2);
		GPIO_SetBits(us_port, trig_pin);
	_delay_us(10);
		GPIO_ResetBits(us_port, trig_pin);
	_delay_us(2);
	
	int timeout = 1000000;
	while (!GPIO_ReadInputDataBit(us_port, echo_pin) ) {
		if (timeout-- == 0) {
			return -1;
		}
	}
		start_time = 0;
	while(GPIO_ReadInputDataBit(us_port, echo_pin)) {
		start_time++;
		_delay_us(1);
	}
	
	/* Calculate distance to cm, do adjustment if needed */
	float distance = start_time;
	return distance / 17.18;
	//Constant : 17.18
}

int ultrasonic_check(distance) {
	int result = (distance <= 10 ? 1 : 0);
	return result;
}
