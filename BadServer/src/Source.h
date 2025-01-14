#ifndef SOURCE_H
#define SOURCE_H

#include "Client.h"
#include "NetworkManager.h"

void handleEvents(std::unique_ptr<NetworkManager>& nm);

void consoleEventHandler(std::unique_ptr<Console>& console, std::unique_ptr<NetworkManager>& nm);

int main();

#endif
