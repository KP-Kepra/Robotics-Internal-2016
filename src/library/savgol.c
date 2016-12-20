#include "savgol.h"

u32 dest_array[128];

void savgol_init() {
}

void savgol_filter(u32 buffer[], int framelen) {
	int degree = 3;
	int i = -(framelen-1)/2;	//Starting point for frame length
	int j = (framelen-1)/2;
	int m = degree+1;			//Assign the polynomial order/degree as columns in array
	float sum;

	float At[2][framelen];
	float AtxAeven[2][2];
	float AtxAeven_inv[2][2];

	float coef[framelen];
	
	/* 2xframelen At*/
	for (int a = 0; a <= m-1; a++) {
		if (a % 2 == 0){
			i = -(framelen-1)/2;
			for (int b = 0; b <= framelen-1; b++) {
				At[a/2][b] = pow(i, a);
				i++;
			}
		}
	}

	/* 2x2 AtxA */
	for (int a = 0; a <= 1; a++){
		for (int b = 0; b <= 1; b++){
			i = -(framelen-1)/2;
			sum = 0;
			while (i <= j){
				sum += pow(i, 2*(a+b));
				i++;
			}
			AtxAeven[a][b] = sum;
		}
	}

	/* 2x2 AtxA Inverse */
	AtxAeven_inv[0][0] = (AtxAeven[1][1]);
	AtxAeven_inv[1][1] = (AtxAeven[0][0]);
	AtxAeven_inv[0][1] = (-(AtxAeven[0][1]));
	AtxAeven_inv[1][0] = (-(AtxAeven[1][0]));
	float det = (AtxAeven_inv[0][0]*AtxAeven_inv[1][1]-AtxAeven_inv[0][1]*AtxAeven_inv[1][0]);

	/* Coefficient Array */
	for (int a = 0; a <= (framelen-1); a++) {
		float sum = 0;
		for (int b = 0; b <= 1; b++) {
			sum += (AtxAeven_inv[0][b])*(At[b][a]);
		}
		coef[a] = sum/det;
	}
	
	/* Smoothing */
	for (int a = 0; a < 128; a++) {
		u32 smoothed_y = 0;
		for (int b = -2; b <= 2; b++) {
			if (((a+b) >= 0) && ((a+b) <= 127)) {
				smoothed_y += coef[b+2] * buffer[a+b];
			}
		}
		
		if ((int)(smoothed_y) < 0) {
			dest_array[a] = 0;
		}
		else {
			dest_array[a] = (int)(smoothed_y);
		}
		
		for (int cap = 0; cap <= 1; cap++) {
			dest_array[cap] = dest_array[3-cap];
			dest_array[127-cap] = dest_array[124+cap];
		}
	}
}