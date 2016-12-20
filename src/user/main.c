#include "main.h"

#define led_port GPIOA
#define LED_L  GPIO_Pin_4
#define LED_M  GPIO_Pin_3
#define LED_R  GPIO_Pin_15

static int t = 0;
static int time_stop = 0;
static int lift = 0;
static int grab = 0;
static int manual_move = 0;
static int speed = 70;
	
void manual_mode(const u8 byte) { //Manual Mode Function
	if ((byte == ' ') && (t == 0)) {
		uart_tx(COM3, "Switched to manual mode\n");
		t = 1;
	}
	
	if (t == 1) {
		manual_move = 1;
		time_stop = get_real_ticks();
		switch(byte) {
			case 'W' :
			case 'w'	:
				uart_tx(COM3, "Move forward\n\n");
				move_forward(speed);
				break;
			
			case 'Q'	:
			case 'q'	:
				uart_tx(COM3, "Turn left 90 degree\n");
				turn_90degree('L', speed-25);
				break;
			
			case 'A' :
			case 'a'	:
				uart_tx(COM3, "Turn left\n");
				turn_atplace('L', speed-25);
				break;
			
			case 'S'	:
			case 's'	:
				uart_tx(COM3, "Move backward\n");
				move_backward(speed);
				break;
			
			case 'E' :
			case 'e'	:
				uart_tx(COM3, "Turn right 90 degree\n");
				turn_90degree('R', speed-25);
				break;
			
			case 'D'	:
			case 'd'	:
				uart_tx(COM3, "Turn Right\n");
				turn_atplace('R', speed-25);
				break;
			
			case '1' :
				uart_tx(COM3, "Speed 1 : 45/20\n");
				speed = 45;
				break;
			
			case '2' :
				uart_tx(COM3, "Speed 2 : 60/35\n");
				speed = 60;
				break;
			
			case '3' :
				uart_tx(COM3, "Speed 3 : 70/45\n");
				speed = 70;
				break; 
			
			case 'L'	:
			case 'l'	:
				uart_tx(COM3, "Load\n");
				pneumatic_control(PNEUMATIC5, 1);
				break;
				
			case 'K' :
			case 'k' :
				uart_tx(COM3, "Shoot\n");
				pneumatic_control(PNEUMATIC5, 0);
				break;
			
			case 'O'	:
			case 'o'	:
				uart_tx(COM3, "Grab basket");
				pneumatic_control(PNEUMATIC6, (grab == 0 ? 1 : 0));
				grab = (grab == 0 ? 1 : 0);
				break;
			
			case 'I' :
			case 'i' :
				uart_tx(COM3, "Lift basket");
				pneumatic_control(PNEUMATIC3, (lift == 0 ? 1 : 0));
				lift = (lift == 0 ? 1 : 0);
				break;
			
			case 'M' :
			case 'm' : 
				uart_tx(COM3, "Dance");
				move_backward(60);
				_delay_ms(1000);
				move_forward(60);
				_delay_ms(3000);
				turn_180degree('R', 30);
				_delay_ms(1000);
				move_backward(60);
				_delay_ms(1000);
				move_forward(60);
				_delay_ms(3000);
				turn_180degree('R', 30);
				break;
			
			default :
				motor_stop();
				break;
		}
	}
}
	
int main() {
	/** INITIALIZATION */
	uart_init(COM3, 115200);
	uart_interrupt_init(COM3, &manual_mode);
	tft_init(2, BLACK, WHITE);
	led_init();
	button_init();
	ultrasonic_init();
	IR_init();
	gpio_init();
	motor_init(71, 100, 0);// Value for shooter robot 
	pneumatic_init();
	ticks_init();
	
	//linear_ccd_init();
	//servo_init(143, 10000, 750); //Value for smartcar
	//motor_init(71, 180, 50); //Value for smartcar
	//adc_init();
	
	int manual_stop = 0;
	int b1 = 0;
	int b2 = 0;
	int b3 = 0;
	int start_flag = 0;
	//int stop = 0;
	//int end = 0;
	
	while (1) {	
		/** Shooter robot  testing*/
		 
		if (t == 1) {
			if (get_real_ticks() - time_stop >= 300 && manual_move == 1) {
				motor_stop();
				manual_move = 0;
			}
		}
		else if (t == 0) {
			if (get_real_ticks() >= 1000) {
				if (read_button(BUTTON1) == 0 && b1 == 0) { //CHANGE STATE
					state += (state < 7 ? 1 : -6);
					b1 = 1;
				}
				else if (read_button(BUTTON1) == 1 && b1 == 1) { b1 = 0; }
				
				if (read_button(BUTTON2) == 0 && b2 == 0) { //CHANGE SPEED
					auto_speed += (auto_speed < 90 ? 10 : -20);
					b2 = 1;
				}
				else if (read_button(BUTTON2) == 1 && b2 == 1) { b2 = 0; }
				
				if (read_button(BUTTON3) == 0 && b3 == 0) { //START/STOP
					start_flag += (start_flag == 0 ? 1 : -1);
					b3 = 1;
					flag_time = get_real_ticks();
				}
				else if (read_button(BUTTON3) == 1 && b3 == 1) { b3 = 0; }
			}
			
			if (get_real_ticks()%100 == 0) {
				distance = ultrasonic_emit();
				ir_left = read_IR(0);
				ir_right = read_IR(1);
								
				tft_clear();
				tft_prints(0, 0, "Ultrasonic : %.2f", distance);
				tft_prints(0, 10, "IR Left : %d", ir_left);
				tft_prints(0, 20, "IR Right : %d", ir_right);
				tft_prints(0, 30, "State : %d", state);
				tft_prints(0, 40, "Speed : %d", auto_speed);
				tft_prints(0, 50, "Flag : %d", start_flag);
				if (start_flag == 1) { state_move(); }
			}
		}
	}
}

	/**	LINEAR CCD */
	//ideone.com/CZsOXN		
	/*
		if (get_real_ticks() % 10 == 0) { //Every 10 miliseconds
			if (end == 0) {
				for (int red = 0; red < 128; red++) {
					tft_put_pixel(red, 40, RED);
				}
			
				for (int n = 0; n < 128; n++) {
					tft_put_pixel(n, 160-dest_array[n]+11, BLACK);
				}
				
				linear_ccd_read();
				
				for (int n = 0; n < 128; n++) {
					tft_put_pixel(n, 160-dest_array[n]+11, WHITE);
				}
			
				int right_counter = 0;
				int left_counter =0;
				
				for (int index = 0; index < 48; index++) {
					if (160-dest_array[index+80]+11 >= 40) {
						tft_put_pixel(index+80, 60, GREEN);
						right_counter++;
					}
					else {
						tft_put_pixel(index+80, 60, BLACK);
					}
				
					if (160-dest_array[index]+11 >= 40) {
						tft_put_pixel(index, 60, BLUE);
						left_counter++;
					}
					else {
						tft_put_pixel(index, 60, BLACK);
					}
				}
		
				if (right_counter-left_counter >= 10) {
					servo_control(SERVO1, 750 + (right_counter - left_counter)*2.1);
					motor_control(MOTOR1, 0, 40);
				}
				else if (left_counter - right_counter >= 10) {
					servo_control(SERVO1, 750 + (right_counter - left_counter)*2.1);
					motor_control(MOTOR1, 0, 40);
				}
				else {
					servo_control(SERVO1, 750);
					motor_control(MOTOR1, 0, 45); 
				}
			}
		}
	}
}
			/*int end_counter = 0;
			for (int index = 0; index <= 127; index++) {
				if (160-dest_array[index]+11 >= 40) {
					end_counter++;
				}
			}
			if (end_counter >= 100) {
				end=1;
				/*int end_ticks = get_real_ticks();
				while(get_real_ticks() - end_ticks <= 100	) {
					motor_control(MOTOR1, 1, 10);
					
				}
				motor_control(MOTOR1, 1, 0);
			}
		}*/
