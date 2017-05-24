#include "mouse.h"


#define APPWM_ICONNOTIFY (WM_APP + 1)


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

NOTIFYICONDATA nid = {};

bool start;
bool disc;

GUID WceusbshGUID = { 0x4d36e978, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
LPSTR lpCmdLine, int NCmdShow){

	DWORD dwWidth = GetSystemMetrics(SM_CXSCREEN);
	DWORD dwHeight = GetSystemMetrics(SM_CYSCREEN);
	
	MSG  msg;
	WNDCLASSW wc = {0};

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = L"MouseM";
	wc.hInstance     = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc   = WndProc;
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	char szTitle[]	 = "MouseM";

	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"MouseM",
	WS_OVERLAPPEDWINDOW | WS_VISIBLE,
	0, 0, dwWidth, dwHeight, NULL, NULL, hInstance, NULL);
	
	
	HWND stealth;
	AllocConsole();
	stealth=FindWindowA("ConsoleWindowClass",NULL);
	ShowWindow(stealth,0);
	
	
	
	
	HWND hwnd;
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, szTitle);
	if(!hMutex)
	hMutex = CreateMutex(0, 0, szTitle);
	else
	{		
		printf("Another instance is running.");		
		return 0; // Exit Second instance
	}
	
	
	
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

	
	
	hwnd = FindWindow(NULL, szTitle);
	ShowWindow(hwnd, SW_HIDE);
	
	static HDEVNOTIFY hDeviceNotify;
	bool DoRegisterDeviceInterfaceToHwnd( 
	IN GUID InterfaceClassGuid, 
	IN HWND hWnd,
	OUT HDEVNOTIFY *hDeviceNotify 
	){
		ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
		NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		NotificationFilter.dbcc_classguid = InterfaceClassGuid;
		
		*hDeviceNotify = RegisterDeviceNotification(hwnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
		
		if(*hDeviceNotify == NULL){
			printf("Uhhhh...");
			return false;
		}
		return true;
	}
	
	DoRegisterDeviceInterfaceToHwnd(
                        WceusbshGUID, 
                        hwnd,
                        &hDeviceNotify);
	
	
	HICON hIcon = (HICON)(LoadImage(NULL,
    MAKEINTRESOURCE(105),
    IMAGE_ICON,
    GetSystemMetrics(SM_CXSMICON),
            GetSystemMetrics(SM_CYSMICON),
    LR_DEFAULTCOLOR));

	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	//Notification
	nid.hIcon = (HICON)LoadImage(hInstance,MAKEINTRESOURCE(105),
		IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uCallbackMessage = APPWM_ICONNOTIFY;
	strcpy(nid.szTip, "Magic Wand Receiver is Active");
	Shell_NotifyIcon(NIM_ADD, &nid);
	
	
	
	disc = true;
	start = true;
	//update_ports();
	add_device();
	start = false;
	_beginthread(serial, 0, NULL);
	
	printf("apple");
	
	while (GetMessage(&msg, NULL, 0, 0)) {
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	

	return (int) msg.wParam;
}


void ShowContextMenu(HWND hWnd)
{
	POINT curPoint ;
    GetCursorPos( &curPoint ) ;
	
    HMENU hPopupMenu = CreatePopupMenu();
	AppendMenu(hPopupMenu, MF_STRING, 1, "About");
	AppendMenu(hPopupMenu, MF_STRING, 2, "Help");
	AppendMenu(hPopupMenu, MF_STRING, 3, "Exit");
	SetForegroundWindow(hWnd);
	TrackPopupMenu(hPopupMenu, TPM_LEFTBUTTON | TPM_RIGHTALIGN, curPoint.x, curPoint.y, 0, hWnd, NULL);
	
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, 
WPARAM wParam, LPARAM lParam) {
	
	HDC hdc;
	PAINTSTRUCT ps;
	
	DWORD wmId;
	DWORD wmEvent;
	
	LRESULT lRet = 1;
	static HDEVNOTIFY hDeviceNotify;
	static ULONGLONG msgCount = 0;

	switch(msg) {
		
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		
		
		switch(wmId)
		{
			case 1:
				system("start \"\" https://fuhrms.github.io/magicwand/MathBehindTheMagic.pdf");
				break;
			case 2:
				system("start \"\" https://fuhrms.github.io/magicwand");
				break;
			case 3:
				Shell_NotifyIcon(NIM_DELETE, &nid);
				PostQuitMessage(0);
				break;
			default:
				break;
		}
		break;
		
		
		
	case APPWM_ICONNOTIFY:
		switch(lParam)
		{
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_CONTEXTMENU:
			ShowContextMenu(hwnd);
			break;
		}
	break;
		
		
		
	case WM_DEVICECHANGE:
		{

			// Output some messages to the window.
			switch (wParam)
			{
			case DBT_DEVICEARRIVAL:
				msgCount++;
				printf("Message %d: DBT_DEVICEARRIVAL\n", msgCount);
				add_device();
				break;
			case DBT_DEVICEREMOVECOMPLETE:
				msgCount++;
				printf("Message %d: DBT_DEVICEREMOVECOMPLETE\n", msgCount);
				remove_device();
				break;
			default:
				msgCount++;
				break;
			}
		}
		break;

	case WM_PAINT:

		hdc = BeginPaint(hwnd, &ps);
		
		int dwWidth = GetSystemMetrics(SM_CXSCREEN);
		int dwHeight = GetSystemMetrics(SM_CYSCREEN);

		EndPaint(hwnd, &ps);
		break;

	case WM_CREATE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}