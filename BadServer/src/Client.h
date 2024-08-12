#ifndef CLIENT_H
#define CLIENT_H

#include "Console.h"

class Client
{
private:
	sf::IpAddress ipAddress;
	unsigned short port;
	sf::Vector2f pos;
	std::wstring nickname;
	int numOfHeartbeatChecks;
	bool isGhost;
	bool isBot;

public:
	Client(bool& isBot, std::wstring& nickame, sf::IpAddress& ipAddress, unsigned short& port);

	sf::IpAddress getIpAddress();
	unsigned short getPort();
	sf::Vector2f getPos();
	std::wstring getNickname();
	int getNumOfHeartbeatChecks();
	bool getIsGhost();
	bool getIsBot();

	void moveClient(sf::Vector2f stepPos);
	void setNumOfHeartbeatChecks(int numOfHeartbeatChecks);
	void setIsGhost(bool isGhost);
};

inline std::mutex clients_mtx;
inline std::vector<std::unique_ptr<Client>> clientsVec;

#endif
