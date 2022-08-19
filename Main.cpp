#ifndef UNICODE
#define UNICODE
#endif
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

const wchar_t* USAGE = L"\n\n--- USAGE ---\n\n\
\
.exe <PATH> <START DELAY>\n\n\
\
<PATH>\n\
Path to folder containing files you want to encrypt and archive.\n\n\
\
<START DELAY>\n\
Time before automatic GUI interfacing starts (in seconds).\n\
DEFAULT: 10\n";

const wchar_t* INVALID_ARGS_ERROR = L"\nERROR: The given arguments are invalid.\n";
const wchar_t* ARG2_NOT_NUM_ERROR = L"\nERROR: Second argument is not number.\n";
const wchar_t* DIR_NOT_FOUND_ERROR = L"\nERROR: Could not find directory on given path.\n";
const wchar_t* FILE_NOT_DIR_ERROR = L"\nERROR: The file on the given path is not a directory.\n";
const wchar_t* GARBRO_NOT_FOUND_ERROR = L"\nERROR: GARbro not found.\n";
const wchar_t* FAILED_TO_RUN_GARBRO_ERROR = L"\nERROR: Failed to run GARbro.\n";
const wchar_t* FAILED_TO_FIND_GARBRO_MAIN_WINDOW_ERROR = L"\nERROR: Failed to find GARbro main window.\n";
const wchar_t* FAILED_TO_FIND_CREATE_ARCHIVE_WINDOW = L"\nERROR: Failed to find \"Create Archive\" window.\n";
const wchar_t* FAILED_TO_CREATE_ARCHIVE = L"ERROR: Failed to create archive.";
const wchar_t* SUCCESS = L"\nSuccess! The .xp3 archive has been created!\n\
It is located in the \"OUTPUT\" folder.\n";

const wchar_t* GARBRO_PATH = L"GARbro\\GARbro.GUI.exe";
const wchar_t* OUTPUT_PATH = L"OUTPUT";
const char* ARCHIVE_NAME = "patch.xp3";

HWND hGARmain;	// Handle to main GARbro window.
HWND hGARcreate;	// Handle to "Create Archive" GARbro window.
INPUT input;	// Structure containing information related to user input to simulate.
int waitTime = 10000;	// Default time to wait before starting auto GUI-ing (in ms).

void PrintToConsole(const wchar_t* text) {	// Prints wchar_t array to console.
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), text, wcslen(text), NULL, NULL);
}

bool CheckDependencies() {	// Checks if program has dependencies.
	DWORD fileAttrib = GetFileAttributesW(GARBRO_PATH);

	if (fileAttrib == INVALID_FILE_ATTRIBUTES || fileAttrib == FILE_ATTRIBUTE_DIRECTORY) {	// Checks if GARbro exe exists on hard coded path.
		PrintToConsole(GARBRO_NOT_FOUND_ERROR);
		return false;
	}

	fileAttrib = GetFileAttributesW(OUTPUT_PATH);
	if (fileAttrib == INVALID_FILE_ATTRIBUTES || fileAttrib != FILE_ATTRIBUTE_DIRECTORY) {
		CreateDirectoryW(OUTPUT_PATH, NULL);
	}

	return true;
}

int CalculateAbsoluteX(int x)
{
	return (x * 65536) / GetSystemMetrics(0);	// GetSystemMetrics(0) returns X of current screen res.
}

int CalculateAbsoluteY(int y)
{
	return (y * 65536) / GetSystemMetrics(1);	// GetSystemMetrics(1) returns Y of current screen res.
}

void MoveMouse(int x, int y) {
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.dx = CalculateAbsoluteX(x);
	input.mi.dy = CalculateAbsoluteY(y);
	input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

	SendInput(1, &input, sizeof(INPUT));
}

void ClickMouse() {
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &input, sizeof(INPUT));

	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &input, sizeof(INPUT));
}

void MultiClick(int count) {
	for (int i = 0; i < count; i++) {
		ClickMouse();
	}
}

void SendKey(WORD key) {
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key;
	input.ki.wScan = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
	input.ki.dwFlags = 0;
	SendInput(1, &input, sizeof(INPUT));

	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}

void SendKeyCombo(WORD key1, WORD key2) {
	input.type = INPUT_KEYBOARD;
	input.ki.dwFlags = 0;	// --- Push down keys ---

	input.ki.wVk = key1;
	input.ki.wScan = MapVirtualKeyA(key1, MAPVK_VK_TO_VSC);
	SendInput(1, &input, sizeof(INPUT));

	input.ki.wVk = key2;
	input.ki.wScan = MapVirtualKeyA(key2, MAPVK_VK_TO_VSC);
	SendInput(1, &input, sizeof(INPUT));

	input.ki.dwFlags = KEYEVENTF_KEYUP;	// --- Release up keys ---

	input.ki.wVk = key1;
	input.ki.wScan = MapVirtualKeyA(key1, MAPVK_VK_TO_VSC);
	SendInput(1, &input, sizeof(INPUT));

	input.ki.wVk = key2;
	input.ki.wScan = MapVirtualKeyA(key2, MAPVK_VK_TO_VSC);
	SendInput(1, &input, sizeof(INPUT));
}

void SendKeysString(const char* text) {
	for (int i = 0; i < strlen(text); i++) {
		if (text[i] == ':') {	// If character is virtual key which is for both ":" and ";", pick ":".
			SendKeyCombo(VK_SHIFT, VK_OEM_1);
		}
		else {
			SendKey(VkKeyScanA(text[i]));
		}
	}
}

void SendKeyMultiple(WORD key, int count) {
	for (int i = 0; i < count; i++) {
		SendKey(key);
	}
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[]) {	// ---------- MAIN PROGRAM ----------
	if (argc != 2 && argc != 3) {	// Checks if user did not give one and two arguments.
		if (argc == 1) {	// If he gave zero, print usage.
			PrintToConsole(USAGE);
			return 0;
		}

		else {	// Else, show invalid arg error message.
			PrintToConsole(INVALID_ARGS_ERROR);
			return 0;
		}
	}

	if (argc == 3) {	// If user gave two args (path + start delay):
		waitTime = _wtoi(argv[2]) * 1000;
		if (waitTime == 0) {
			PrintToConsole(ARG2_NOT_NUM_ERROR);
			return 0;
		}
	}

	DWORD fileAttrib = GetFileAttributes(argv[1]);

	if (fileAttrib == INVALID_FILE_ATTRIBUTES) {
		PrintToConsole(DIR_NOT_FOUND_ERROR);
		return 0;
	}

	if (fileAttrib != FILE_ATTRIBUTE_DIRECTORY) {
		PrintToConsole(FILE_NOT_DIR_ERROR);
		return 0;
	}

	if (!CheckDependencies()) {
		return 0;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	wchar_t garbroProcPath[2048];
	GetCurrentDirectoryW(2024, garbroProcPath);
	wcscat(garbroProcPath, L"\\");
	wcscat(garbroProcPath, GARBRO_PATH);

	if (!CreateProcessW(garbroProcPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {	// Starts GARbro.
		PrintToConsole(FAILED_TO_RUN_GARBRO_ERROR);
		return 0;
	}

	Sleep(waitTime);

	hGARmain = FindWindowW(NULL, L"GARbro");
	if (hGARmain == NULL) {
		PrintToConsole(FAILED_TO_FIND_GARBRO_MAIN_WINDOW_ERROR);
		return 0;
	}

	// --- START OF THE AUTO GUI-ING ---

	SetWindowPos(hGARmain, NULL, 0, 0, 640, 480, NULL);
	ShowWindow(hGARmain, SW_SHOW);

	MoveMouse(200, 65);
	ClickMouse();
	SendKeyCombo(VK_CONTROL, VkKeyScanA('a'));
	SendKey(VK_BACK);

	char currentPathA[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, currentPathA);

	SendKeysString(currentPathA);

	SendKey(VK_RETURN);

	ClickMouse();
	SendKeyCombo(VK_CONTROL, VkKeyScanA('a'));
	SendKey(VK_BACK);

	char argPathCharsA[MAX_PATH];
	wcstombs(argPathCharsA, argv[1], wcslen(argv[1]));
	argPathCharsA[wcslen(argv[1])] = NULL;	// Removes the "\0" character from the end. Breaks the GARbro path thing. Don't know why.

	SendKeysString(argPathCharsA);

	SendKey(VK_RETURN);

	SendKeyCombo(VK_CONTROL, VkKeyScanA('a'));
	SendKey(VK_F3);

	Sleep(waitTime);

	hGARcreate = FindWindowW(NULL, L"Create Archive");
	if (hGARcreate == NULL) {
		PrintToConsole(FAILED_TO_FIND_CREATE_ARCHIVE_WINDOW);
		return 0;
	}

	SetWindowPos(hGARcreate, NULL, 0, 0, 640, 480, SWP_NOSIZE);
	ShowWindow(hGARcreate, SW_SHOW);

	MoveMouse(200, 75);
	ClickMouse();
	SendKeyCombo(VK_CONTROL, VkKeyScanA('a'));
	SendKey(VK_BACK);

	char outputPathA[MAX_PATH];
	wcstombs(outputPathA, OUTPUT_PATH, wcslen(OUTPUT_PATH));
	outputPathA[wcslen(OUTPUT_PATH)] = NULL;	// Removing "\0" character from end. Again. Cause breaks. Again.

	char archivePath[MAX_PATH];
	strcpy(archivePath, currentPathA);
	strcat(archivePath, "\\");
	strcat(archivePath, outputPathA);
	strcat(archivePath, "\\");
	strcat(archivePath, ARCHIVE_NAME);

	SendKeysString(archivePath);

	MoveMouse(100, 130);
	ClickMouse();
	Sleep(waitTime / 5);
	SendKeyMultiple(VK_DOWN, 19);
	Sleep(waitTime / 10);
	SendKeyMultiple(VK_UP, 3);
	Sleep(waitTime / 10);
	SendKey(VK_RETURN);
	Sleep(waitTime / 10);
	MoveMouse(250, 150);
	ClickMouse();
	Sleep(waitTime / 5);
	SendKeyMultiple(VkKeyScanA('y'), 4);
	Sleep(waitTime / 10);
	SendKey(VK_RETURN);
	Sleep(waitTime / 5);
	MoveMouse(135, 220);
	ClickMouse();
	MoveMouse(135, 240);
	ClickMouse();
	MoveMouse(200, 75);
	ClickMouse();
	Sleep(waitTime / 10);

	SendKeyMultiple(VK_TAB, 8);
	Sleep(waitTime / 10);
	SendKey(VK_RETURN);
	Sleep(waitTime / 5);
	SendKey(VK_RETURN);

	Sleep(waitTime / 2);
	TerminateProcess(pi.hProcess, 0);
	

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);


	DWORD outputAttrib = GetFileAttributesA(archivePath);
	if (outputAttrib == INVALID_FILE_ATTRIBUTES || outputAttrib == FILE_ATTRIBUTE_DIRECTORY) {
		PrintToConsole(FAILED_TO_CREATE_ARCHIVE);
	}
	else {
		PrintToConsole(SUCCESS);
	}

	return 0;
}