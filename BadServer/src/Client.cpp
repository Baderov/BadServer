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

	if (this->nickname == L"Bot0" || this->nickname == L"Baderov") { pos = sf::Vector2f(2500.f, 2500.f); }
	else if (this->nickname == L"Bot1" || this->nickname == L"Baderovd") { pos = sf::Vector2f(2800.f, 2800.f); }
	else { pos = sf::Vector2f(static_cast<float>(500 + rand() % 4000), static_cast<float>(500 + rand() % 4000)); }
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
void Client::moveClient(sf::Vector2f stepPos)
{
	this->pos += std::move(stepPos);
}

void Client::setNumOfHeartbeatChecks(int numOfHeartbeatChecks)
{
	this->numOfHeartbeatChecks = std::move(numOfHeartbeatChecks);
}

void Client::setIsGhost(bool isGhost)
{
	this->isGhost = std::move(isGhost);
}