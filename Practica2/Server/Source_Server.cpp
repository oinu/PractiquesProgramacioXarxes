
#include <SFML\Network.hpp>
#include <iostream>

struct Players
{
	sf::IpAddress ip;
	unsigned short port;
	std::string name;
	int x;
	int y;
};

int main()
{
	sf::UdpSocket sock;
	sf::Socket::Status status = sock.bind(50000);
	Players players[1];
	if (status != sf::Socket::Done)
	{
		std::cout << "Error al vincular\n";
		system("pause");
		exit(0);
	}
	sf::Clock clock;
	do
	{
		if (clock.getElapsedTime().asMilliseconds() > 200)
		{
			for (int i = 0; i < 1; i++)
			{
				sf::Packet pck;
				status = sock.receive(pck, players[i].ip, players[i].port);
				if (status == sf::Socket::Done)
				{
					//std::cout << players[i].ip << "		"<<players[i].port<< std::endl;
					int id, pos;
					pck >> id >> pos;
					if (pos == -1)
					{
						break;
					}

					//std::cout << "Se intenta la pos " << pos << std::endl;

					//Movimiento en X
					if (id == 0 && pos >= 100 && pos + 30 <= 700)
					{
						players[i].x = pos;
						std::cout << "Se confirma la x pos " << players[i].x << std::endl;
						sf::Packet pckSend;
						pckSend << 0 << pos;
						sock.send(pck, players[i].ip, players[i].port);

					}

					//Movimiento en Y
					else if (id == 1 && pos >= 200 && pos + 30 <= 500)
					{
						players[i].y = pos;
						std::cout << "Se confirma la y pos " << players[i].y << std::endl;
						sf::Packet pckSend;
						pckSend << 1 << players[i].y;
						sock.send(pck, players[i].ip, players[i].port);

					}

					//Si la posicion es invalida
					else
					{
						sf::Packet pckSend;
						pckSend << -1 << -1;
						sock.send(pckSend, players[i].ip, players[i].port);
					}
				}
			}
			clock.restart();
		}
	} while (true);
	sock.unbind();
	return 0;
}