#include "mouse.h"

bool disc;
void serial()
{
	srand(time(NULL));

	
	
	int x = 0;
	int y = 0;
	int i = 0;                          // Handle to port
	//char  ComPortName[] = "\\\\.\\COM31";  // Name of port (set in Windows)
	int  status;						   // Status of operations (generic)
	DWORD dwEventMask;                     // Event mask to trigger
	char  SerialBuffer[31];                // Buffer Containing Rxed Data
	DWORD NoBytesRead;                     // Bytes read by ReadFile()

	printf("\n\n +==========================================+");
	printf("\n |    Magic Wand Windows Server Thing :D    |");
	printf("\n +==========================================+\n");


	while(1){
		
		while (disc){
			Sleep(10);
		}
		printf("disc = false\n");
		printf("\n    Opening port...");


		//params
		DCB dcbSerialParams = { 0 };                         // Initializing DCB structure
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

		status = GetCommState(hComm, &dcbSerialParams);      //retreives  the current settings

		if (status == 0){
			printf("\n    Error: GetCommState(), hComm = %d", hComm);
			exit(-1);
		}

		//serial settings
		dcbSerialParams.BaudRate = CBR_115200;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;

		status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

		if (status == 0)
		{
			printf("\n    Error: Setting Device Control Block");
			exit(-2);
		}
		else //If Successfull display the contents of the DCB Structure
		{
			printf("\n\n    Device Control Block:\n");
			printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
			printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
			printf("\n       StopBits = %d", dcbSerialParams.StopBits);
			printf("\n       Parity   = %d", dcbSerialParams.Parity);
		}

		//timeouts
		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout         = 1000;
		timeouts.ReadTotalTimeoutConstant    = 1000;
		timeouts.ReadTotalTimeoutMultiplier  = 1000;
		timeouts.WriteTotalTimeoutConstant   = 1000;
		timeouts.WriteTotalTimeoutMultiplier = 1000;
		
		if (SetCommTimeouts(hComm, &timeouts) == 0){
			printf("\n\n    Error: Setting timeouts");
			exit(-3);
		}
		else{
			printf("\n\n    Setting Serial Port Timeouts Successful");
		}
		
		//recv mask
		status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception
		
		if (status == 0){
			printf("\n\n    Error: Setting CommMask");
			exit(-5);
		}
		else{
			printf("\n\n    Setting CommMask successfull");
		}
		
		
		//waitcom event	
		printf("\n\n    Waiting for data...\n");

		status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received
		
		//wait for char
		if (status == 0)
		{
			printf("\n    Error: Setting WaitCommEvent()");
			exit(-6);
		}
		else //If  WaitCommEvent()==True Read the RXed data using ReadFile();
		{
			while (!disc) {
				do
				{
					status = ReadFile(hComm, &SerialBuffer, sizeof(SerialBuffer) - 1, &NoBytesRead, NULL);
					char* buf = deblank(SerialBuffer);
					//printf("Received: %s\n", buf);
					process(buf);
					
					free(buf);
				} while (NoBytesRead > 0);
			}//for(;;)
			printf("disc = true\n");
		}	
		//printf("\n\nPress any key to continue.");
		CloseHandle(hComm);//Closing the Serial Port
		//return;	
		//_getch();
	}
}//main()
