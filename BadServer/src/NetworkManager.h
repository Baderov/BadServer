#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "Client.h"

class NetworkManager
{
private:
	std::mutex mtx;
	sf::UdpSocket sock;
public:
	NetworkManager();

	void bindPort();
	void addBots();
	bool addClient(std::wstring& clientNick, sf::IpAddress& ipAddress, unsigned short& port);
	void sendPacketToAllClients(sf::Packet& packet);
	void pingClients();
	void sockSend(sf::Packet& packet, sf::IpAddress& serverIP, unsigned short& serverPort);
	void sockSend(sf::Packet& packet, sf::IpAddress&& serverIP, unsigned short&& serverPort);
	bool sockNotRecv(sf::Packet& packet, sf::IpAddress& remoteAddress, unsigned short& remotePort);;
};

#endif