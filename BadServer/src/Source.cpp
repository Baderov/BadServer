#include "pch.h"
#include "Source.h"

struct Connection
{
	sf::IpAddress ipAddress = "";
	unsigned short port = 0;
};

void handleEvents(std::unique_ptr<NetworkManager>& nm)
{
	sf::Packet packet;
	std::wstring prefix = L"";

	while (true)
	{
		Connection connection;
		packet.clear();
		prefix = L"";

		if (nm->sockNotRecv(packet, connection.ipAddress, connection.port)) { continue; };
		if (!(packet >> prefix)) { std::cout << "prefix error!" << "\n"; continue; }

		if (prefix == L"regNick")
		{
			std::wstring tempNick = L"";

			if (!(packet >> tempNick)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			if (nm->addClient(tempNick, connection.ipAddress, connection.port))
			{
				packet.clear();
				packet << prefix << tempNick;
				nm->sockSend(packet, connection.ipAddress, connection.port);
			}
			else
			{
				prefix = L"conError_nickExists";
				packet.clear();
				packet << prefix;
				nm->sockSend(packet, connection.ipAddress, connection.port);
			}
		}

		else if (prefix == L"connected")
		{
			std::wstring tempNick = L"";

			if (!(packet >> tempNick)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				packet.clear();
				packet << prefix << clientsVec[i]->getNickname() << clientsVec[i]->getPos().x << clientsVec[i]->getPos().y;
				nm->sockSend(packet, connection.ipAddress, connection.port);
			}

			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() == clientsVec.back()->getNickname()) { continue; }

				packet.clear();
				packet << prefix << clientsVec.back()->getNickname() << clientsVec.back()->getPos().x << clientsVec.back()->getPos().y;
				nm->sockSend(packet, clientsVec[i]->getIpAddress(), clientsVec[i]->getPort());
			}
		}

		else if (prefix == L"respawn")
		{
			std::wstring respawnedNick = L"";
			sf::Vector2f startPos(0.f, 0.f);

			if (!(packet >> respawnedNick && packet >> startPos.x && packet >> startPos.y)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			packet.clear();
			packet << prefix << respawnedNick << startPos.x << startPos.y;
			nm->sendPacketToAllClients(packet);

		}

		else if (prefix == L"mousePos")
		{
			sf::Vector2f tempMousePos(0.f, 0.f);
			std::wstring tempNick = L"";

			if (!(packet >> tempNick && packet >> tempMousePos.x && packet >> tempMousePos.y)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			packet.clear();
			packet << prefix << tempNick << tempMousePos.x << tempMousePos.y;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"move")
		{
			std::wstring tempNick = L"";
			sf::Vector2f tempStepPos(0.f, 0.f);

			if (!(packet >> tempNick && packet >> tempStepPos.x && packet >> tempStepPos.y)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() != tempNick) { continue; }

				clientsVec[i]->moveClient(tempStepPos);

				break;
			}

			packet.clear();
			packet << prefix << tempNick << tempStepPos.x << tempStepPos.y;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"shoot")
		{
			std::wstring tempCreatorNick = L"";
			sf::Vector2f tempAimPos(0.f, 0.f);
			sf::Vector2f tempBulletPos(0.f, 0.f);

			if (!(packet >> tempCreatorNick && packet >> tempAimPos.x && packet >> tempAimPos.y && packet >> tempBulletPos.x && packet >> tempBulletPos.y)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			packet.clear();
			packet << prefix << tempCreatorNick << tempAimPos.x << tempAimPos.y << tempBulletPos.x << tempBulletPos.y;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"msg")
		{
			std::wstring tempNick = L"";
			std::wstring msg = L"";

			if (!(packet >> tempNick && packet >> msg)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			packet.clear();
			packet << prefix << tempNick << msg;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"ghost")
		{
			std::wstring ghostNick = L"";
			bool isGhost = false;

			if (!(packet >> ghostNick && packet >> isGhost)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() != ghostNick) { continue; }

				clientsVec[i]->setIsGhost(isGhost);

				break;
			}

			packet.clear();
			packet << prefix << ghostNick << isGhost;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"ping")
		{
			std::wstring tempNick = L"";

			if (!(packet >> tempNick)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() != tempNick) { continue; }

				clientsVec[i]->setNumOfHeartbeatChecks(0);

				break;
			}
		}

		else { std::wcout << L"Reading error! prefix: " << prefix << "\n"; }
	}
}

int main()
{
	auto nm = std::make_unique<NetworkManager>();

	std::thread pingThread([&]() { nm->pingClients(); });
	pingThread.detach();

	handleEvents(nm);

	return 0;
}