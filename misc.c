#include "mouse.h"

int sumx;
int lastx_error;

int sumy;
int lasty_error;

float kp = 0.05;
float ks2 = 2.5;
float ki = 0.01;
float kd = 0.02;
float kdamp = 1./3;
int st = 750;

HANDLE hComm;
int ports[33] = {0};

void remove_device(){
	disc = true;
	CloseHandle(hComm);
	printf("\nDevice removed.\n");
	update_ports();
}

void add_device(){

	if(disc){
		int i = update_ports();
		printf("\ni is %d\n", i);
		char str[20];
		char tmp[4];
		strcpy(str,"\\\\.\\COM");
		itoa(i, tmp, 10);
		strcat(str,tmp);
		if(i != -1 && i != -2){
			hComm = CreateFile( str,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
			if(hComm == INVALID_HANDLE_VALUE){
				printf("\nError creating handle on %s.\n", str);
				return;
			}
			else{
				printf("\nHandle created successfully on port %s, hComm = %d.\n", str, hComm);
				disc = false;
				return;
			}
		}
	}
}

int update_ports(){
	HANDLE bComm;

	char str[20];
	char tmp[4];
	int i=0;
	int j=0;
	for(i=0; i<33; ++i){
		strcpy(str,"\\\\.\\COM");
		itoa(i, tmp, 10);
		strcat(str,tmp);
		bComm = CreateFile( str, // Name of the Port to be Opened
		GENERIC_READ | GENERIC_WRITE, // Read/Write Access
		0,                            // No Sharing, ports cant be shared
		NULL,                         // No Security
		OPEN_EXISTING,                // Open existing port only
		0,                            // Non Overlapped I/O
		NULL);                        // Null for Comm Devices
		if(bComm == INVALID_HANDLE_VALUE){
			ports[i] = 0;
		}
		else{
			printf("\nyay on %s\n", str);
			int k = -1;
			if(ports[i]==0 && !start){
				k = i;
				printf("\nReturning a value of %d for k\n", k);
				ports[i] = 1;
				CloseHandle(bComm);
				return k;
			}
			ports[i] = 1;
			CloseHandle(bComm);
		}
		//printf("%s: %d\n", str, ports[i]);
	}
	CloseHandle(hComm);
	return -1;
	
	//CloseHandle(hComm);
	//return -2;
}

/*
bool prev=true;
void listener(){
	while(1){
		if(disc==true && prev != disc){
			printf("\nHANDLE CLOSED\n");
			CloseHandle(hComm);
		}
		Sleep(10);
		prev = disc;
	}
}
*/





char* deblank(char* input)                                                  /* deblank accepts a char[] argument and returns a char[] */
{
	char *output=input;
	int i, j;
	for (i = 0, j = 0; i<strlen(input); i++,j++)                        /* Evaluate each character in the input */
	{
		if (input[i]!=' ')                                                  /* If the character is not a space */
		output[j]=input[i];                                             /* Copy that character to the output char[] */
		else
		j--;                                                            /* If it is a space then do not increment the output index (j), the next non-space will be entered at the current index */
	}
	return output;                                                          /* Return output char[]. Should have no spaces*/
}

void move_mouse(int x, int y, POINT point){
	
	int delx = x - point.x;
	int dely = y - point.y;
	float k = 0.05;
	int xc = (int)( (delx/st)*(delx/st)*ks2 + kp * delx + ki * sumx + kd * (delx - lastx_error));
	int yc = (int)((dely/st)*(dely/st)*ks2 + kp * dely + ki * sumy + kd * (dely - lasty_error));
	
	sumx = sumx * kdamp + delx;
	sumy = sumy * kdamp + dely;
	lastx_error = delx;
	lasty_error = dely;
	
	SetCursorPos(point.x + xc, point.y + yc);
}


