
#include <SFML\Network.hpp>
#include <iostream>

struct Player
{
	sf::IpAddress ip;
	unsigned short port;
	std::string name;
	int x;
	int y;
};
enum Code
{
	XMOVE,YMOVE,HELLO,WELLCOME
};

int main()
{
	sf::UdpSocket sock;
	sf::Socket::Status status = sock.bind(50000);
	std::vector<Player> players;
	Player p;
	int index=-1;
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
			sf::Packet pck;
			status = sock.receive(pck, p.ip, p.port);
				index = -1;
				//Buscamos que si ya es conocido
				for (int i = 0; i < players.size(); i++)
				{
					if (players[i].ip == p.ip && players[i].port == p.port)
						index = i;
				}

				if (status == sf::Socket::Done)
				{
					int id, pos;
					pck >> id;
					Player player;

					switch (id)
					{

					case Code::XMOVE:
						//Movimiento en X
						pck >> pos;
						if (pos >= 0 && pos + 30 <= 800)
						{
							players[index].x = pos;
							std::cout << "Se confirma la x pos " << players[index].x << players[index].name << std::endl;
							sf::Packet pckSend;
							pckSend << XMOVE << pos;
							sock.send(pck, players[index].ip, players[index].port);
						}

						//Si la posicion es invalida
						else
						{
							sf::Packet pckSend;
							pckSend << -1;
							sock.send(pckSend, players[index].ip, players[index].port);
						}
						break;

					case Code::YMOVE:
						//Movimiento en Y
						pck >> pos;
						if (pos >= 200 && pos + 30 <= 500)
						{
							players[index].y = pos;
							std::cout << "Se confirma la y pos " << players[index].y <<players[index].name<< std::endl;
							sf::Packet pckSend;
							pckSend << YMOVE << players[index].y;
							sock.send(pck, players[index].ip, players[index].port);
						}

						//Si la posicion es invalida
						else
						{
							sf::Packet pckSend;
							pckSend << -1;
							sock.send(pckSend, players[index].ip, players[index].port);
						}
						break;
					case Code::HELLO:
						if (index == -1)
						{
							player.ip = p.ip;
							player.port = p.port;
							pck >> player.name;
							players.push_back(player);
							sf::Packet sendPck;
							int size= players.size();
							sendPck << Code::WELLCOME << size;
							sock.send(sendPck, player.ip, player.port);
						}
						break;

					default:
						break;
					}
				}
			clock.restart();
		}
	} while (true);
	sock.unbind();
	return 0;
}