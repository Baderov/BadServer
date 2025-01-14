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
			std::wstring clientGameVersion = L"";

			if (!(packet >> clientNick && packet >> clientGameVersion)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			if (clientGameVersion != nm->getGameVersion())
			{
				prefix = L"game_version";
				packet.clear();
				packet << prefix;
				nm->sockSend(packet, connection.ipAddress, connection.port);
				continue;
			}

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

			size_t numOfConnectedClients = clientsVec.size();

			if (!(packet >> clientNick)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				packet.clear();

				packet << prefix << clientsVec[i]->getNickname() << clientsVec[i]->getPos().x << clientsVec[i]->getPos().y << clientsVec[i]->getHP() <<
					clientsVec[i]->getIsBot() << clientsVec[i]->getNumOfKills() << clientsVec[i]->getNumOfDeaths() << numOfConnectedClients;

				nm->sockSend(packet, connection.ipAddress, connection.port);
			}

			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() == clientNick) { continue; }

				packet.clear();

				packet << prefix << clientNick << clientsVec.back()->getPos().x << clientsVec.back()->getPos().y << clientsVec.back()->getHP() <<
					clientsVec.back()->getIsBot() << clientsVec.back()->getNumOfKills() << clientsVec.back()->getNumOfDeaths() << numOfConnectedClients;

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

				clientsVec[i]->setHP(100);
				clientsVec[i]->setNumOfDeaths(clientsVec[i]->getNumOfDeaths() + 1);
				clientsVec[i]->setPos(clientStartPos);

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
			sf::Vector2f clientNewPos(0.f, 0.f);

			if (!(packet >> clientNick && packet >> clientStepPos.x && packet >> clientStepPos.y)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() != clientNick) { continue; }

				clientNewPos = clientsVec[i]->getPos() + clientStepPos;

				clientsVec[i]->setPos(clientNewPos);

				break;
			}

			packet.clear();
			packet << prefix << clientNick << clientNewPos.x << clientNewPos.y;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"shoot")
		{
			std::wstring bulletCreatorNick = L"";
			sf::Vector2f bulletAimPos(0.f, 0.f);
			sf::Vector2f bulletPos(0.f, 0.f);
			sf::Vector2f currentVelocity(0.f, 0.f);

			if (!(packet >> bulletCreatorNick && packet >> bulletAimPos.x && packet >> bulletAimPos.y && packet >> bulletPos.x
				&& packet >> bulletPos.y && packet >> currentVelocity.x && packet >> currentVelocity.y)) {
				std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue;
			}

			std::lock_guard<std::mutex> lock(clients_mtx);
			packet.clear();
			packet << prefix << bulletCreatorNick << bulletAimPos.x << bulletAimPos.y << bulletPos.x << bulletPos.y << currentVelocity.x << currentVelocity.y;
			nm->sendPacketToAllClients(packet);
		}

		else if (prefix == L"hit")
		{
			std::wstring shooterClientNick = L"";
			std::wstring woundedClientNick = L"";
			int woundedClientHP = 0;
			bool woundedClientDead = false;

			if (!(packet >> shooterClientNick && packet >> woundedClientNick)) { std::wcout << L"prefix_" << prefix << "_error!" << std::endl; continue; }

			std::lock_guard<std::mutex> lock(clients_mtx);
			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() != woundedClientNick) { continue; }

				clientsVec[i]->setHP(clientsVec[i]->getHP() - 10);
				if (clientsVec[i]->getHP() <= 0) { woundedClientDead = true; }

				woundedClientHP = clientsVec[i]->getHP();

				break;
			}

			for (size_t i = 0; i < clientsVec.size(); ++i)
			{
				if (clientsVec[i]->getNickname() != shooterClientNick) { continue; }

				if (woundedClientHP <= 0)
				{
					clientsVec[i]->setNumOfKills(clientsVec[i]->getNumOfKills() + 1);
				}

				break;
			}


			packet.clear();
			packet << prefix << shooterClientNick << woundedClientNick << woundedClientHP << woundedClientDead;
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

		else { std::wcout << L"Readidng error! prefix: " << prefix << "\n"; }
	}
}

bool is_number(const std::wstring& s)
{
	return !s.empty() && (s.find_first_not_of(L"0123456789") == s.npos);
}

void consoleEventHandler(std::unique_ptr<Console>& console, std::unique_ptr<NetworkManager>& nm)
{
	unsigned int consoleActionNum = 0;
	unsigned int numOfBots = 0;
	std::wstring answer;

	while (true)
	{
		SetConsoleTextAttribute(console->getHandle(), 11);
		std::cout << "\nSelect an action:\n1. Add bots.\n2. Kick bot.\n3. Kick all bots.\n4. Kick client.\n5. Print online clients.\n";

		SetConsoleTextAttribute(console->getHandle(), 15);
		std::cout << "Action: ";
		std::getline(std::wcin, answer);
		std::cout << std::endl;

		if (answer == L"1")
		{
			std::cout << "Write the number of bots: ";
			std::getline(std::wcin, answer);
			if (is_number(answer) && std::stoi(answer) <= 100)
			{
				nm->addBots(std::stoi(answer));
				SetConsoleTextAttribute(console->getHandle(), 14);
				std::cout << "Bots added!" << std::endl;
			}

		}

		else if (answer == L"2")
		{
			std::cout << "Write the bot's nickname: ";
			std::getline(std::wcin, answer);
			if (nm->kickBot(answer))
			{
				SetConsoleTextAttribute(console->getHandle(), 14);
				std::cout << "Bot was kicked out!" << std::endl;
			}
			else
			{
				SetConsoleTextAttribute(console->getHandle(), 12);
				std::cout << "Wrong nickname!" << std::endl;
			}
		}

		else if (answer == L"3")
		{
			nm->kickAllBots();
			SetConsoleTextAttribute(console->getHandle(), 14);
			std::cout << "Bots was kicked out!" << std::endl;
		}

		else if (answer == L"4")
		{
			std::cout << "Write the client's nickname: ";
			std::getline(std::wcin, answer);
			if (nm->kickClient(answer))
			{
				SetConsoleTextAttribute(console->getHandle(), 14);
				std::cout << "Client was kicked out!" << std::endl;
			}
			else
			{
				SetConsoleTextAttribute(console->getHandle(), 12);
				std::cout << "Wrong nickname!" << std::endl;
			}
		}

		else if (answer == L"5")
		{
			console->printOnlineClients();
		}
		else
		{
			SetConsoleTextAttribute(console->getHandle(), 12);
			std::cout << "Wrong action!" << std::endl;
		}
	}
}

int main()
{
	auto console = std::make_unique<Console>();
	auto nm = std::make_unique<NetworkManager>();

	std::thread pingThread([&]() { nm->pingClients(); });
	pingThread.detach();

	std::thread consoleThread([&]() { consoleEventHandler(console, nm); });
	consoleThread.detach();

	handleEvents(nm);

	return 0;
}