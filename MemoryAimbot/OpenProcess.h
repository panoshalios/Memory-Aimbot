#pragma once
#include <Windows.h>
#include <iostream>

#define GAMENAME "Counter-Strike Source"

HANDLE hProc;

void OpenProc(HWND hw_Game)
{
	DWORD procID;
	GetWindowThreadProcessId(hw_Game, &procID);
	hProc = OpenProcess(PROCESS_ALL_ACCESS, false, procID);
	if (!hProc)
	{
		std::cout << "Game found but failed to open. Try running this programm as administrator ;)" << std::endl;
		system("pause");
	}
	else
	{
		std::cout << "Game Found!!!" << std::endl;
	}
}

void SearchForGame()
{
	HWND gameHWND = NULL;
	std::cout << "Searching for game!!!" << std::endl;
	while (!gameHWND)
	{
		gameHWND = FindWindow(NULL, GAMENAME);
		Sleep(2000);
	}
	OpenProc(gameHWND);
}
