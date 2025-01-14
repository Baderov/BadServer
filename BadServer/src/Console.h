#ifndef CONSOLE_H
#define CONSOLE_H

#include "Client.h"

class Console
{
private:
	HANDLE handle;
	bool showOnlineClients;
	std::mutex mtx;
public:
	Console();

	HANDLE& getHandle();
	void setConsoleSettings();
	void printOnlineClients();
};

#endif
