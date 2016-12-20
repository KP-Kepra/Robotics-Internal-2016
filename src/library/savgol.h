#include <stdio.h>
#include <math.h>
#include "stm32f10x.h"
#include "lcd_blue.h"

void savgol_filter(u32 buffer[], int framelen);

extern u32 dest_array[128];