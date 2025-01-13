#include "pch.h"
#include "Client.h"

Client::Client(bool& isBot, std::wstring& nickname, sf::IpAddress& ipAddress, unsigned short& port)
{
	numOfHeartbeatChecks = 0;
	this->isBot = isBot;
	this->nickname = nickname;
	this->ipAddress = ipAddress;
	this->port = port;
	isGhost = false;
	HP = 100;
	numOfKills = 0;
	numOfDeaths = 0;

	pos = sf::Vector2f(static_cast<float>(500 + rand() % 4000), static_cast<float>(500 + rand() % 4000));
}



// GETTERS.
sf::IpAddress Client::getIpAddress()
{
	sf::IpAddress ipAddress = this->ipAddress;
	return ipAddress;
}

unsigned short Client::getPort()
{
	unsigned short port = this->port;
	return port;
}

sf::Vector2f Client::getPos()
{
	sf::Vector2f pos = this->pos;
	return pos;
}

std::wstring Client::getNickname()
{
	std::wstring nickname = this->nickname;
	return nickname;
}

int Client::getNumOfHeartbeatChecks()
{
	int numOfHeartbeatChecks = this->numOfHeartbeatChecks;
	return numOfHeartbeatChecks;
}

int Client::getHP()
{
	int HP = this->HP;
	return HP;
}

int Client::getNumOfKills()
{
	int numOfKills = this->numOfKills;
	return numOfKills;
}

int Client::getNumOfDeaths()
{
	int numOfDeaths = this->numOfDeaths;
	return numOfDeaths;
}

bool Client::getIsGhost()
{
	bool isGhost = this->isGhost;
	return isGhost;
}

bool Client::getIsBot()
{
	bool isBot = this->isBot;
	return isBot;
}



// SETTERS.
void Client::setPos(sf::Vector2f pos)
{
	this->pos = std::move(pos);
}

void Client::setNumOfHeartbeatChecks(int numOfHeartbeatChecks)
{
	this->numOfHeartbeatChecks = std::move(numOfHeartbeatChecks);
}

void Client::setHP(int HP)
{
	this->HP = std::move(HP);
}

void Client::setNumOfKills(int numOfKills)
{
	this->numOfKills = std::move(numOfKills);
}

void Client::setNumOfDeaths(int numOfDeaths)
{
	this->numOfDeaths = std::move(numOfDeaths);
}

void Client::setIsGhost(bool isGhost)
{
	this->isGhost = std::move(isGhost);
}