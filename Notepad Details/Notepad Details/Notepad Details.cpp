#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include "proc.h"

//Tracking Values
int numbers = 0, alphabat = 0, lines = 1, spacials = 0,space = 0;

bool check(HANDLE hProcess, uintptr_t firstchar)
{
	uintptr_t buff = 0;
	ReadProcessMemory(hProcess, (BYTE*)firstchar, &buff, sizeof(buff), 0);
	if (buff == 0)
	{
		ReadProcessMemory(hProcess, (BYTE*)firstchar+2, &buff, sizeof(buff), 0);
		if (buff == 0)
		{
			return false;
		}
	}
	return true;
}

void adder(int num)
{
	if (isalpha(num))
	{
		alphabat++;
	}
	else if (isdigit(num))
	{
		numbers++;
	}
	else if (num == 10 || num == 13)
	{
		lines++;
	}
	else if (num == (int)' ')
	{
		space++;
	}
	else
	{
		spacials++;
	}
}
void printmenu()
{
	std::cout << "\n\nAlphabats : " << alphabat <<
		"\nNumbers : " << numbers <<
		"\nLines : " << (lines/2) + 1 <<
		"\nSpacials : " << spacials<<
		"\nSpaces : "<<space;
}
int main()
{
	HANDLE hProcess = 0;
	uintptr_t moduleBase = 0, objectPtr = 0, firstchar = 0;

	//Get ProcId of the target process
	DWORD procId = GetProcId(L"notepad.exe");

	if (procId)
	{
		//Get Handle to Process
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

		//Getmodulebaseaddress
		moduleBase = GetModuleBaseAddress(procId, L"notepad.exe");

		//Resolve address
		objectPtr = moduleBase + 0x0002C470;

	}
	else
	{
		std::cout << "Process not found, press enter to exit\n";
		getchar();
		return 0;
	}

	DWORD dwExit = 0;
	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
	{
		//Resolve base address of the pointer chain
		firstchar = FindDMAAddy(hProcess, objectPtr, { 0x0,0x0 });
		uintptr_t buff = firstchar;
		for (int i = 0; check(hProcess,firstchar); i++)
		{
			ReadProcessMemory(hProcess, (BYTE*)firstchar, &buff, sizeof(buff), 0);
			firstchar = firstchar + 2;
			std::cout << (char)buff;
			adder((int)(char)buff);
		}
		printmenu();
		system("cls");
		numbers = 0, alphabat = 0, lines = 1, spacials = 0,space = 0;
		//Use incert Key to stop the software
		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			return 0;
		}
	}

	std::cout << "Process not found, press enter to exit\n";
	getchar();
	return 0;
}