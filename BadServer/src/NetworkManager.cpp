#include "pch.h"
#include "NetworkManager.h"

const int MAX_NUM_OF_HEARTBEAT_CHECKS = 5;

NetworkManager::NetworkManager() { consoleSettings(); bindPort(); addBots(); }

void NetworkManager::bindPort()
{
	const auto RetryCount = 3;
	const sf::IpAddress ServerIp = sf::IpAddress::getLocalAddress();
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

void NetworkManager::addBots()
{
	bool isBot = true;

	std::lock_guard<std::mutex> lock(clients_mtx);
	for (int i = 0; i < 15; ++i)
	{
		std::wstring botNick = L"Bot" + std::to_wstring(i);
		sf::IpAddress botIP = "192.168.1.2" + std::to_string(i);
		unsigned short botPort = 2000 + i;
		clientsVec.emplace_back(std::make_unique<Client>(isBot, botNick, botIP, botPort));
	}
}

bool NetworkManager::addClient(std::wstring& clientNick, sf::IpAddress& ipAddress, unsigned short& port)
{
	for (size_t i = 0; i < clientsVec.size(); ++i)
	{
		if (clientsVec[i]->getNickname() == clientNick) { return false; }
	}

	bool isBot = false;

	clientsVec.emplace_back(std::make_unique<Client>(isBot, clientNick, ipAddress, port));

	printOnlineClients();

	return true;
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
			std::wstring leftTheServerNick = L"";

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
					leftTheServerNick = clientsVec[i]->getNickname();
					clientsVec.erase(std::remove(clientsVec.begin(), clientsVec.end(), clientsVec[i]), clientsVec.end());

					printOnlineClients();

					prefix = L"disconnected";

					packet.clear();
					packet << prefix << leftTheServerNick;

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