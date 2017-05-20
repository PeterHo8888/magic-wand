#include <windows.h>
#include <stdio.h>
#include <dbt.h>
#include <winuser.h>

typedef enum {false, true} bool;
extern char* deblank(char* input);
void move_mouse(int x, int y, POINT point);
void process(char* buf);
void serial();
int update_ports();
void listener();

extern bool disc;
extern int ports[33];
extern bool start;

extern HANDLE hComm; 