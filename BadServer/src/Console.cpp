#include "pch.h"
#include "Console.h"

void consoleSettings()
{
	setlocale(LC_ALL, "RUS"); // for Cyrillic in the console.
	// remove the ability to select and perform various actions in the console.
	handle = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(handle, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	mode &= ~ENABLE_INSERT_MODE;
	mode &= ~ENABLE_MOUSE_INPUT;
	SetConsoleMode(handle, mode);
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, 14);
}

void printOnlineClients()
{
	SetConsoleTextAttribute(handle, 11);
	std::wcout << L"\nOnline clients: \n";
	std::wcout << L"--------------------------------------------------------------------\n";
	if (clientsVec.size() > 0)
	{
		for (size_t i = 0; i < clientsVec.size(); ++i)
		{
			std::wcout << L"Client num: " + std::to_wstring(i) + L". Client name: " + clientsVec[i]->getNickname() + L".\n";
		}
	}

	else
	{
		SetConsoleTextAttribute(handle, 14);
		std::wcout << L"***Server is empty!***\n";
		SetConsoleTextAttribute(handle, 11);
	}
	std::wcout << L"--------------------------------------------------------------------\n\n";
	SetConsoleTextAttribute(handle, 15);
}