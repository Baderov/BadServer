#include "pch.h"
#include "NetworkManager.h"

const int MAX_NUM_OF_HEARTBEAT_CHECKS = 5;

unsigned int NetworkManager::botID = 0;

NetworkManager::NetworkManager()
{
	setGameVersion(L"0.0.2");
	bindPort();
}

void NetworkManager::bindPort()
{
	const auto RetryCount = 3;
	//const sf::IpAddress ServerIp = "26.54.85.12"; // global IP.
	const sf::IpAddress ServerIp = sf::IpAddress::getLocalAddress(); // local IP.
	const unsigned short ServerPort = 2000;

	for (int connectionTries = 1; sock.bind(ServerPort, ServerIp) != sf::Socket::Done; ++connectionTries)
	{
		if (connectionTries >= RetryCount)
		{
			std::cerr << "Unable to bind on port " << ServerPort << "\n";
			break;
		}
	}
	std::cout << "Server is up!" << "\n";

	sock.setBlocking(false);
}

void NetworkManager::addBots(unsigned int numOfBots)
{
	std::lock_guard<std::mutex> lock(clients_mtx);
	sf::Packet packet;
	std::wstring prefix = L"connected";
	bool isBot = true;
	size_t numOfConnectedClients = clientsVec.size();
	for (unsigned int i = 0; i < numOfBots; ++i)
	{
		std::wstring botNick = L"Bot" + std::to_wstring(botID);
		sf::IpAddress botIP = std::to_string(botID);
		unsigned short botPort = botID;
		botID++;
		clientsVec.emplace_back(std::make_unique<Client>(isBot, botNick, botIP, botPort));

		packet.clear();
		packet << prefix << clientsVec.back()->getNickname() << clientsVec.back()->getPos().x << clientsVec.back()->getPos().y <<
			clientsVec.back()->getHP() << clientsVec.back()->getIsBot() << clientsVec.back()->getNumOfKills() << clientsVec.back()->getNumOfDeaths() << numOfConnectedClients;

		sendPacketToAllClients(packet);
	}
}

bool NetworkManager::kickBot(std::wstring& botNick)
{
	std::lock_guard<std::mutex> lock(clients_mtx);
	for (size_t i = 0; i < clientsVec.size(); ++i)
	{
		if (clientsVec[i]->getNickname() != botNick || !clientsVec[i]->getIsBot()) { continue; }

		clientsVec.erase(std::remove(clientsVec.begin(), clientsVec.end(), clientsVec[i]), clientsVec.end());

		sf::Packet packet;
		std::wstring prefix = L"disconnected";

		packet.clear();
		packet << prefix << botNick;

		sendPacketToAllClients(packet);

		return true;
	}
	return false;
}

void NetworkManager::kickAllBots()
{
	std::lock_guard<std::mutex> lock(clients_mtx);

	clientsVec.erase(remove_if(clientsVec.begin(), clientsVec.end(), [&](std::unique_ptr<Client>& client)
		{
			if (client->getIsBot())
			{
				std::wstring botNick = client->getNickname();
				sf::Packet packet;
				std::wstring prefix = L"disconnected";

				packet.clear();
				packet << prefix << botNick;

				sendPacketToAllClients(packet);

				return this;
			}
		}), clientsVec.end());
}



bool NetworkManager::addClient(std::wstring& clientNick, sf::IpAddress& ipAddress, unsigned short& port)
{
	std::lock_guard<std::mutex> lock(clients_mtx);
	for (size_t i = 0; i < clientsVec.size(); ++i)
	{
		if (clientsVec[i]->getNickname() == clientNick) { return false; }
	}

	bool isBot = false;

	clientsVec.emplace_back(std::make_unique<Client>(isBot, clientNick, ipAddress, port));

	return true;
}

bool NetworkManager::kickClient(std::wstring& clientNick)
{
	std::lock_guard<std::mutex> lock(clients_mtx);
	for (size_t i = 0; i < clientsVec.size(); ++i)
	{
		if (clientsVec[i]->getNickname() != clientNick || clientsVec[i]->getIsBot()) { continue; }

		clientsVec.erase(std::remove(clientsVec.begin(), clientsVec.end(), clientsVec[i]), clientsVec.end());

		sf::Packet packet;
		std::wstring prefix = L"disconnected";

		packet.clear();
		packet << prefix << clientNick;

		sendPacketToAllClients(packet);

		return true;
	}
	return false;
}

void NetworkManager::sendPacketToAllClients(sf::Packet& packet)
{
	for (size_t i = 0; i < clientsVec.size(); ++i)
	{
		sockSend(packet, clientsVec[i]->getIpAddress(), clientsVec[i]->getPort());
	}
}

void NetworkManager::pingClients()
{
	while (true)
	{
		if (!clientsVec.empty())
		{
			sf::Packet packet;
			std::wstring prefix = L"ping";
			std::wstring disconnectedClient = L"";

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (!clientsVec[i]->getIsBot())
				{
					packet.clear();
					packet << prefix << clientsVec[i]->getNickname();
					clientsVec[i]->setNumOfHeartbeatChecks(clientsVec[i]->getNumOfHeartbeatChecks() + 1);
					sendPacketToAllClients(packet);
				}
			}
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNumOfHeartbeatChecks() >= MAX_NUM_OF_HEARTBEAT_CHECKS && !clientsVec[i]->getIsBot())
				{
					disconnectedClient = clientsVec[i]->getNickname();
					clientsVec.erase(std::remove(clientsVec.begin(), clientsVec.end(), clientsVec[i]), clientsVec.end());

					prefix = L"disconnected";

					packet.clear();
					packet << prefix << disconnectedClient;

					sendPacketToAllClients(packet);

					break;
				}
			}
		}
		sf::sleep(sf::milliseconds(100));
	}
}

void NetworkManager::sockSend(sf::Packet& packet, sf::IpAddress& remoteAddress, unsigned short& remotePort)
{
	std::lock_guard<std::mutex> lock(mtx);
	this->sock.send(packet, remoteAddress, remotePort);
}

void NetworkManager::sockSend(sf::Packet& packet, sf::IpAddress&& remoteAddress, unsigned short&& remotePort)
{
	std::lock_guard<std::mutex> lock(mtx);
	this->sock.send(packet, remoteAddress, remotePort);
}

bool NetworkManager::sockNotRecv(sf::Packet& packet, sf::IpAddress& remoteAddress, unsigned short& remotePort)
{
	std::lock_guard<std::mutex> lock(mtx);
	if (this->sock.receive(packet, remoteAddress, remotePort) != sf::Socket::Done) { return true; }
	return false;
}

std::wstring NetworkManager::getGameVersion()
{
	std::wstring gameVersion = this->gameVersion;
	return gameVersion;
}

void NetworkManager::setGameVersion(std::wstring gameVersion)
{
	this->gameVersion = std::move(gameVersion);
}