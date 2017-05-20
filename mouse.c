#include "mouse.h"

bool a_set = false;
bool b_set = false;
bool last_left = false;
bool last_right = false;
bool x_offset = false;
int x;
int y;
int width;
int height;
int ax;
int bx;
int ay;
int by;
int baseline_x = 50;
int baseline_y = 50;
int screen_width;
int screen_height;
int sx;
int sy;

int apply_x_offset(int n) {
	return (n+180)%360;
}

void reset_calibration() {
	if (abs(ax-bx) > 180 && a_set && b_set) {
		x_offset = true;
		ax = apply_x_offset(ax);
		bx = apply_x_offset(bx);
		x = apply_x_offset(x);
	} else {
		x_offset = false;
	}
	width = bx-ax;
	height = by-ay;	
}

void move_cursor() {
	if (a_set && b_set) {
		sx = screen_width * (x - ax) / width + baseline_x;
		sy = screen_height * (y - ay) / height + baseline_y;
		
		POINT point;
		GetCursorPos(&point);
		move_mouse(sx, sy, point);
	}
}

void send_event(int event) {
	INPUT Input={0};
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = event;
	SendInput(1,&Input,sizeof(INPUT));
}

void process(char* buf){
	
	screen_width = GetSystemMetrics(SM_CXSCREEN) - 2*baseline_x;
	screen_height = GetSystemMetrics(SM_CYSCREEN) - 2*baseline_y;
	
	char* temp = (char*)malloc(sizeof(char)*strlen(buf)+1);
	int csv[5];
	
	temp = strtok(buf, ",");
	if(temp == NULL){return;}
	csv[0] = atoi(temp);
	
	temp = strtok(NULL, ",");
	if(temp == NULL){return;}
	csv[1] = x_offset ? apply_x_offset(atoi(temp)) : atoi(temp);
	
	temp = strtok(NULL, ",");
	if(temp == NULL){return;}
	csv[2] = atoi(temp);
	
	if(csv[0]==3){
		temp = strtok(NULL, ",");
		if(temp==NULL){return;}
		csv[3] = temp[0]=='t';
		if(temp==NULL){return;}
		temp = strtok(NULL, ",");
		csv[4] = temp[0]=='t';
	}
	
	free(temp);

	switch (csv[0]) {
		case 0: //start calibration
			a_set = b_set = x_offset = false;
			//api stuff draw small circles at baseline values given above
			break;

		case 1: //set top-left
			ax = x = csv[1];
			ay = y = csv[2];
			a_set = true;
			reset_calibration();
			move_cursor();
			break;

		case 2: //set bottom-right
			bx = x = csv[1];
			by = y = csv[2];
			b_set = true;
			reset_calibration();
			move_cursor();
			break;

		case 3: //move cursor
			x = csv[1];
			y = csv[2];
			
			//printf("(x, y): (%d, %d)\n", x, y);
			move_cursor();
			if (csv[3] > last_left) {
				send_event(MOUSEEVENTF_LEFTDOWN);
			} else if (csv[3] < last_left) {
				send_event(MOUSEEVENTF_LEFTUP);
			}
			
			if (csv[4] > last_right) {
				send_event(MOUSEEVENTF_RIGHTDOWN);
			} else if (csv[4] < last_right) {
				send_event(MOUSEEVENTF_RIGHTUP);
			}
			
			last_left = csv[3];
			last_right = csv[4];
			
			break;

		case 4: //end caibration
			//blah blah blah api stuff
			break;
	}
}










