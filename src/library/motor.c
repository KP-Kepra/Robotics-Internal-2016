#include "motor.h"

GPIO_TypeDef* motor_dir_port[3] = {GPIOB, GPIOC, GPIOC};
uc16 motor_dir_pin[3] = {GPIO_Pin_12, GPIO_Pin_11, GPIO_Pin_12};
#define MOTOR_TIM	TIM1

int m1_dir = 0;
int m2_dir = 0;
int ir_left = 0;
int ir_right = 0;
int move = 0;

void motor_init(u16 presc, u16 count, u16 init_val) {
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStructure.TIM_Prescaler = presc;
	TIM_TimeBaseStructure.TIM_Period = count;
	TIM_TimeBaseInit(MOTOR_TIM, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = init_val;
	
	TIM_OC1Init(MOTOR_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(MOTOR_TIM, ENABLE);
	
	TIM_OC2Init(MOTOR_TIM, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(MOTOR_TIM, ENABLE);

	TIM_OC3Init(MOTOR_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(MOTOR_TIM, ENABLE);
	
	TIM_ARRPreloadConfig(MOTOR_TIM, ENABLE);
	TIM_Cmd(MOTOR_TIM, ENABLE);	
	TIM_SetCounter(MOTOR_TIM,0);
	TIM_CtrlPWMOutputs(MOTOR_TIM, ENABLE);
}

void motor_control(MOTOR_ID motor_id, u8 dir, u16 magnitude){
	if (dir)
		GPIO_SetBits(motor_dir_port[motor_id], motor_dir_pin[motor_id]);
	else
		GPIO_ResetBits(motor_dir_port[motor_id], motor_dir_pin[motor_id]);
	
	if (motor_id == MOTOR1) {
		TIM_SetCompare1(MOTOR_TIM,magnitude);
	}
	if (motor_id == MOTOR2) {
		TIM_SetCompare2(MOTOR_TIM,magnitude);
	}
	if (motor_id == MOTOR3) {
		TIM_SetCompare3(MOTOR_TIM,magnitude);
	}
}

//Assume motor 1 is left motor, motor 2 is right motor
void move_forward(u16 magnitude) {

		motor_control(MOTOR1, 0, magnitude*1.05 ); //1.05 with 70 speed
		motor_control(MOTOR2, 1, magnitude);
		m1_dir = 0;
		m2_dir = 1;
}

void move_backward(u16 magnitude) {

		motor_control(MOTOR1, 1, magnitude);
		motor_control(MOTOR2, 0, magnitude*1.0267);
		m1_dir = 1;
		m2_dir = 0;
}

void turn_half(char turn, u16 magnitude) {
	motor_control(MOTOR1, (turn == 'R' ? 0 : 1), (turn == 'R' ? magnitude : 0));
	motor_control(MOTOR1, (turn == 'R' ? 0 : 1), (turn == 'R' ? 0 : magnitude));
	m1_dir = (turn == 'R' ? 0 : 1);
	m2_dir = m1_dir;
}

void turn_atplace(char turn, u16 magnitude) {
	motor_control(MOTOR1, (turn == 'R' ? 0 : 1), magnitude);
	motor_control(MOTOR2, (turn == 'R' ? 0 : 1), magnitude);
	m1_dir = (turn == 'R' ? 0 : 1);
	m2_dir = m1_dir;
}

void turn_90degree(char turn, u16 magnitude) {
	u32 time_speed = 13*2000;
	turn_atplace(turn, magnitude);
	if (turn == 'L') {_delay_ms(time_speed/magnitude);}
	else {_delay_ms(16.8*2000/magnitude);}
	motor_stop();
	m1_dir = (turn == 'R' ? 0 : 1);
	m2_dir = (turn == 'R' ? 0 : 1);	
}

void turn_180degree(char turn, u16 magnitude) {
	u32 time_speed = 20*2000;
	turn_atplace(turn, magnitude);
	_delay_ms(time_speed/magnitude);
	motor_stop();
	m1_dir = (turn == 'R' ? 0 : 1);
	m2_dir = (turn == 'R' ? 0 : 1);	
}

void motor_stop(void) {
	motor_control(MOTOR1, (m1_dir == 1 ? 0 : 1), 0);
	motor_control(MOTOR2, (m2_dir == 1 ? 0 : 1), 0);
	move = 0;
}
