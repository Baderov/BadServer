#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "Client.h"

class NetworkManager
{
private:
	std::mutex mtx;
	sf::UdpSocket sock;
	std::wstring gameVersion;
	bool showOnlineClients;
	static unsigned int botID;
public:
	NetworkManager();

	void bindPort();

	void addBots(unsigned int numOfBots);
	bool kickBot(std::wstring& botNick);
	void kickAllBots();
	bool addClient(std::wstring& clientNick, sf::IpAddress& ipAddress, unsigned short& port);
	bool kickClient(std::wstring& clientNick);

	void sendPacketToAllClients(sf::Packet& packet);
	void pingClients();
	void sockSend(sf::Packet& packet, sf::IpAddress& serverIP, unsigned short& serverPort);
	void sockSend(sf::Packet& packet, sf::IpAddress&& serverIP, unsigned short&& serverPort);
	bool sockNotRecv(sf::Packet& packet, sf::IpAddress& remoteAddress, unsigned short& remotePort);

	std::wstring getGameVersion();
	void setGameVersion(std::wstring gameVersion);
};

#endif