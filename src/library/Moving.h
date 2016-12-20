#include "stm32f10x.h"
#include "motor.h"
#include "IR_sensors.h"
#include "pneumatic.h"
#include "ultrasonic.h"

void state_move(void);
void state1_move(void);
void state2_move(void);
void state3_move(void);
void state4_move(void);
void state5_move(void);
void state6_move(void);

extern float distance;
extern u32 ticks_img;
extern int state;

extern int side;
extern int flag_time;
extern int auto_speed;