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
	int HP;
	int numOfKills;
	int numOfDeaths;
	bool isGhost;
	bool isBot;

public:
	Client(bool& isBot, std::wstring& nickame, sf::IpAddress& ipAddress, unsigned short& port);

	sf::IpAddress getIpAddress();
	unsigned short getPort();
	sf::Vector2f getPos();
	std::wstring getNickname();
	int getNumOfHeartbeatChecks();
	int getHP();
	int getNumOfKills();
	int getNumOfDeaths();
	bool getIsGhost();
	bool getIsBot();

	void setPos(sf::Vector2f pos);
	void setNumOfHeartbeatChecks(int numOfHeartbeatChecks);
	void setHP(int HP);
	void setNumOfKills(int numOfKills);
	void setNumOfDeaths(int numOfDeaths);
	void setIsGhost(bool isGhost);
};

inline std::mutex clients_mtx;
inline std::vector<std::unique_ptr<Client>> clientsVec;

#endif
