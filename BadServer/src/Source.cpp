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
			std::wstring clientNick = L"";

			if (!(packet >> clientNick)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			if (nm->addClient(clientNick, connection.ipAddress, connection.port))
			{
				packet.clear();
				packet << prefix << clientNick;
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
			std::wstring clientNick = L"";

			if (!(packet >> clientNick)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				packet.clear();
				packet << prefix << clientsVec[i]->getNickname() << clientsVec[i]->getPos().x << clientsVec[i]->getPos().y;
				nm->sockSend(packet, connection.ipAddress, connection.port);
			}

			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() == clientNick) { continue; }

				packet.clear();
				packet << prefix << clientNick << clientsVec.back()->getPos().x << clientsVec.back()->getPos().y;
				nm->sockSend(packet, clientsVec[i]->getIpAddress(), clientsVec[i]->getPort());
			}
		}

		else if (prefix == L"respawn")
		{
			std::wstring clientNick = L"";
			sf::Vector2f clientStartPos(0.f, 0.f);

			if (!(packet >> clientNick && packet >> clientStartPos.x && packet >> clientStartPos.y)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() != clientNick) { continue; }

				clientsVec[i]->setClientPos(clientStartPos);

				break;
			}

			packet.clear();
			packet << prefix << clientNick << clientStartPos.x << clientStartPos.y;
			nm->sendPacketToAllClients(packet);

		}

		else if (prefix == L"mousePos")
		{
			sf::Vector2f clientMousePos(0.f, 0.f);
			std::wstring clientNick = L"";

			if (!(packet >> clientNick && packet >> clientMousePos.x && packet >> clientMousePos.y)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			packet.clear();
			packet << prefix << clientNick << clientMousePos.x << clientMousePos.y;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"move")
		{
			std::wstring clientNick = L"";
			sf::Vector2f clientStepPos(0.f, 0.f);

			if (!(packet >> clientNick && packet >> clientStepPos.x && packet >> clientStepPos.y)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() != clientNick) { continue; }

				clientsVec[i]->moveClient(clientStepPos);

				break;
			}

			packet.clear();
			packet << prefix << clientNick << clientStepPos.x << clientStepPos.y;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"shoot")
		{
			std::wstring bulletCreatorNick = L"";
			sf::Vector2f bulletAimPos(0.f, 0.f);
			sf::Vector2f bulletPos(0.f, 0.f);

			if (!(packet >> bulletCreatorNick && packet >> bulletAimPos.x && packet >> bulletAimPos.y && packet >> bulletPos.x && packet >> bulletPos.y)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			packet.clear();
			packet << prefix << bulletCreatorNick << bulletAimPos.x << bulletAimPos.y << bulletPos.x << bulletPos.y;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"msg")
		{
			std::wstring clientNick = L"";
			std::wstring clientMsg = L"";

			if (!(packet >> clientNick && packet >> clientMsg)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			packet.clear();
			packet << prefix << clientNick << clientMsg;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"ghost")
		{
			std::wstring clientNick = L"";
			bool isGhost = false;

			if (!(packet >> clientNick && packet >> isGhost)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() != clientNick) { continue; }

				clientsVec[i]->setIsGhost(isGhost);

				break;
			}

			packet.clear();
			packet << prefix << clientNick << isGhost;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"ping")
		{
			std::wstring clientNick = L"";

			if (!(packet >> clientNick)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() != clientNick) { continue; }

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