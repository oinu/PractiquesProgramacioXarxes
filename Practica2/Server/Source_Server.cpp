
#include <SFML\Network.hpp>
#include <iostream>
#include <time.h>

struct Player
{
	sf::IpAddress ip;
	unsigned short port;
	std::string name;
	int x;
	int y;
};

struct Game
{
	std::string pregunta;
	std::string a;
	std::string b;
	std::string c;
	std::string d;
};

enum Code
{
	XMOVE, YMOVE, HELLO, WELLCOME, XPLAYER1, YPLAYER1, XPLAYER2, YPLAYER2, XPLAYER3, YPLAYER3, XPLAYER4, YPLAYER4, ASK, AOPTION, BOPTION, COPTION, DOPTION
};

int main()
{
	sf::UdpSocket sock;
	sf::Socket::Status status = sock.bind(50000);
	std::vector<Player> players;
	Player p;
	int index=-1;
	int preguntaActual=0;
	//float currentTime;
	bool newGame = true;
	if (status != sf::Socket::Done)
	{
		std::cout << "Error al vincular\n";
		system("pause");
		exit(0);
	}

	Game g[5];

	g[0].pregunta = "";
	g[0].a = "";
	g[0].b = "";
	g[0].c = "";
	g[0].d = "";

	g[1].pregunta = "";
	g[1].a = "";
	g[1].b = "";
	g[1].c = "";
	g[1].d = "";

	g[2].pregunta = "";
	g[2].a = "";
	g[2].b = "";
	g[2].c = "";
	g[2].d = "";

	g[3].pregunta = "";
	g[3].a = "";
	g[3].b = "";
	g[3].c = "";
	g[3].d = "";

	g[4].pregunta = "";
	g[4].a = "";
	g[4].b = "";
	g[4].c = "";
	g[4].d = "";

	sf::Clock clock;
	do
	{
		if (clock.getElapsedTime().asMilliseconds() > 200)
		{
			sf::Packet pck;
			if (players.size() == 4 && newGame)
			{
				pck << ASK << g[preguntaActual].pregunta;
				for (int i = 0; i < players.size(); i++)
				{
					sock.send(pck, players[i].ip, players[i].port);
				}
				pck.clear();

				pck << AOPTION << g[preguntaActual].a;
				for (int i = 0; i < players.size(); i++)
				{
					sock.send(pck, players[i].ip, players[i].port);
				}
				pck.clear();

				pck << BOPTION << g[preguntaActual].b;
				for (int i = 0; i < players.size(); i++)
				{
					sock.send(pck, players[i].ip, players[i].port);
				}
				pck.clear();

				pck << COPTION << g[preguntaActual].c;
				for (int i = 0; i < players.size(); i++)
				{
					sock.send(pck, players[i].ip, players[i].port);
				}
				pck.clear();

				pck << DOPTION << g[preguntaActual].d;
				for (int i = 0; i < players.size(); i++)
				{
					sock.send(pck, players[i].ip, players[i].port);
				}
				pck.clear();

				newGame = false;
			}

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
							//std::cout << "Se confirma la x pos " << players[index].x << players[index].name << std::endl;
							sf::Packet pckSend;
							pckSend << XMOVE << pos;
							sock.send(pckSend, players[index].ip, players[index].port);

							//Miramos todos players 
							for (int i = 0; i < players.size(); i++)
							{
								//Evitamos mandar al mismo jugador
								if (index != i)
								{
									//Si el jugado es....
									switch (index)
									{
									case 0:
										pckSend.clear();
										pckSend << XPLAYER1 << pos;
										sock.send(pckSend,players[i].ip,players[i].port);
										break;
									case 1:
										pckSend.clear();
										pckSend << XPLAYER2 << pos;
										sock.send(pckSend, players[i].ip, players[i].port);
										break;
									case 2:
										pckSend.clear();
										pckSend << XPLAYER3 << pos;
										sock.send(pckSend, players[i].ip, players[i].port);
										break;
									case 3:
										pckSend.clear();
										pckSend << XPLAYER4 << pos;
										sock.send(pckSend, players[i].ip, players[i].port);
										break;
									default:
										break;
									}
								}
							}
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
							sf::Packet pckSend;
							pckSend << YMOVE << players[index].y;
							sock.send(pckSend, players[index].ip, players[index].port);

							//Miramos todos players 
							for (int i = 0; i < players.size(); i++)
							{
								//Evitamos mandar al mismo jugador
								if (index != i)
								{
									//Si el jugado es....
									switch (index)
									{
									case 0:
										pckSend.clear();
										pckSend << YPLAYER1 << pos;
										sock.send(pckSend, players[i].ip, players[i].port);
										break;
									case 1:
										pckSend.clear();
										pckSend << YPLAYER2 << pos;
										sock.send(pckSend, players[i].ip, players[i].port);
										break;
									case 2:
										pckSend.clear();
										pckSend << YPLAYER3 << pos;
										sock.send(pckSend, players[i].ip, players[i].port);
										break;
									case 3:
										pckSend.clear();
										pckSend << YPLAYER4 << pos;
										sock.send(pckSend, players[i].ip, players[i].port);
										break;
									default:
										break;
									}
								}
							}
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
						if (index == -1 && players.size()<4)
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