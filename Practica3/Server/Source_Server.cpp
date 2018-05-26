
#include <SFML\Network.hpp>
#include <iostream>
#include <thread>
#include "BBDD.h"

#define IP_PORT "192.168.1.136:3306"
#define USER "root"
#define PWD "linux123"
#define DATABASE "game"

struct Player
{
	sf::IpAddress ip;
	unsigned short port;
	std::string name;
	int x;
	int y;
	int points;
};

struct Game
{
	std::string pregunta;
	std::string a;
	std::string b;
	std::string c;
	std::string d;
	int respuesta;
};

enum Code
{
	XMOVE, YMOVE, HELLO, WELLCOME, XPLAYER1, YPLAYER1, XPLAYER2, YPLAYER2, XPLAYER3, YPLAYER3, XPLAYER4, YPLAYER4,
	ASK, PLAYERSNAME,STARTTIME,ENDTIME, ENDGAME, ERROR_LOGIN
};

sf::UdpSocket sock;
sf::Socket::Status status = sock.bind(50000);
std::vector<Player> players;
Player p;
Game g[5];
sf::Packet pck;
int index = -1;
int preguntaActual = 0;
bool state = true;
int currentTime =-1;



void ActualizarMarcadores()
{
	//Mirar las posiciones de los jugadores para determinar que opcion eligio
	for (int i = 0; i < players.size(); i++)
	{
		int x = players[i].x + 15;
		int y = players[i].y + 15;
		

		//Opcion A
		if (x >= 0 && x <= 400 && y >= 200 && y <= 350 && g[preguntaActual].respuesta == 1)
		{
			players[i].points += 10;
		}

		//Opcion B
		else if (x >= 400 && x <= 800 && y >= 200 && y <= 350 && g[preguntaActual].respuesta == 2)
		{
			players[i].points += 10;
		}

		//Opcion C
		if (x >= 0 && x <= 400 && y >= 350 && y <= 500 && g[preguntaActual].respuesta == 3)
		{
			players[i].points += 10;
		}

		//Opcion D
		else if (x >= 400 && x <= 800 && y >= 350 && y <= 500 && g[preguntaActual].respuesta == 4)
		{
			players[i].points += 10;
		}
	}

	//Pasamos las puntuaciones
	pck.clear();
	pck << ENDTIME << players[0].points << players[1].points << players[2].points << players[3].points;
	for (int i = 0; i < players.size(); i++)
	{
		sock.send(pck, players[i].ip, players[i].port);
	}
	pck.clear();

	//Cambia de pregunta, actualmente hay 5
	if (preguntaActual<4)preguntaActual++;
}

void CreacionPreguntas()
{
	g[0].pregunta = "Que es un bracamarte ?";
	g[0].a = "A: Un tipo de espada";
	g[0].b = "B: Una danza del Europa Este";
	g[0].c = "C: Un instrumento africano";
	g[0].d = "D: Una mascara japonesa";
	g[0].respuesta = 1;

	g[1].pregunta = "Artista cofundador del cubismo ?";
	g[1].a = "A : Leonardo Da Vinci";
	g[1].b = "B : Francis Ford Coppola";
	g[1].c = "C : Pablo Picasso";
	g[1].d = "D : Cube McCubinson";
	g[1].respuesta = 3;

	g[2].pregunta = "Que año acabo la Guerra Fria ?";
	g[2].a = "A : 1993";
	g[2].b = "B : 1940";
	g[2].c = "C : Aun sigue";
	g[2].d = "D :1989";
	g[2].respuesta = 4;

	g[3].pregunta = "Cual fue el primer libro impreso ?";
	g[3].a = "A : un libro de recetas";
	g[3].b = "B : una biblia cristiana";
	g[3].c = "C : un registro de propiedades";
	g[3].d = "D : un comic";
	g[3].respuesta = 2;

	g[4].pregunta = "Cuando hablamos de un dingo, nos referimos a...";
	g[4].a = "A : Un plato italiano";
	g[4].b = "B : Un animal australiano";
	g[4].c = "C : Un insulto mexicano";
	g[4].d = "D : Una especie de araña alemana";
	g[4].respuesta = 2;
}

void NewGame()
{
	//Pasar pregunta y repuestas
	pck.clear();
	pck << ASK << g[preguntaActual].pregunta << g[preguntaActual].a << g[preguntaActual].b << g[preguntaActual].c << g[preguntaActual].d;
	for (int i = 0; i < players.size(); i++)
	{
		sock.send(pck, players[i].ip, players[i].port);
	}
	pck.clear();

	//Informar que empieza la partida y el tiempo que tienen.
	currentTime = 30;
	pck << STARTTIME << 30;
	for (int i = 0; i < players.size(); i++)
	{
		sock.send(pck, players[i].ip, players[i].port);
	}
	pck.clear();
}

void EndGame()
{
	std::vector<int>ganadores;
	int points=-1;
	//Guardamos el indice del jugador ganador
	for (int i = 0; i < players.size(); i++)
	{
		if (players[i].points > points)
		{
			ganadores.clear();
			ganadores.push_back(i);
			points = players[i].points;
		}
		else if (players[i].points == points)
		{
			ganadores.push_back(i);
		}
	}

	std::string nombresGanadores;
	for (int i = 0; i < ganadores.size(); i++)
	{
		nombresGanadores += players[i].name+ "	";
	}
	//Indicamos el jugador ganador
	pck.clear();
	pck << ENDGAME << nombresGanadores + "	WIN!!!";
	for (int i = 0; i < players.size(); i++)
	{
		sock.send(pck, players[i].ip, players[i].port);
	}
	pck.clear();
}

void ActualizarTime()
{
	sf::Clock timer;
	while (state)
	{
		if (timer.getElapsedTime().asSeconds() >= 1)
		{
			//Se resta de los segundos restantes
			currentTime--;
			//Si llega a cero, empieza una nueva
			if (currentTime == 0)
			{
				ActualizarMarcadores();
				if(preguntaActual<4)NewGame();
				else EndGame();
			}
			timer.restart();
		}
	}
}

int main()
{
	if (status != sf::Socket::Done)
	{
		std::cout << "Error al vincular\n";
		system("pause");
		exit(0);
	}
	BBDD bd(IP_PORT, USER, PWD);
	bd.SelectDataBase(DATABASE);

	//Se introduccen las preguntas y sus respuestas
	CreacionPreguntas();

	sf::Clock clock;

	//Mira si ha pasado un segundo
	std::thread t(ActualizarTime);
	do
	{
		pck.clear();
		if (clock.getElapsedTime().asMilliseconds() > 200)
		{
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
										sock.send(pckSend, players[i].ip, players[i].port);
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
							//Guardamos la ip, el puerto y le indicamos la puntuacion
							player.ip = p.ip;
							player.port = p.port;
							player.points = 0;
							
							//Situamos los jugadores segun van llegando
							if (players.size() == 0)
							{
								player.x = 370;
								player.y = 320;
							}
							else if (players.size() == 1)
							{
								player.x = 430;
								player.y = 320;
							}
							else if (players.size() == 2)
							{
								player.x = 370;
								player.y = 380;
							}
							else if (players.size() == 3)
							{
								player.x = 430;
								player.y = 380;
							}

							//Guardamos el nombre recibido
							std::string pwd,name;
							pck >>name>> pwd;
							
							User u = bd.ReturnUserByName(name);
							sf::Packet sendPck;

							//Usuario y contraseña correctos
							if (u.password == pwd)
							{
								player.name = name;
								int size = players.size();
								players.push_back(player);
								sendPck << Code::WELLCOME << size;
								sock.send(sendPck, player.ip, player.port);

								//Cuando llega el ultimo jugador.
								if (players.size() == 4)
								{
									NewGame();
									for (int i = 0; i < players.size(); i++)
									{
										sendPck.clear();
										sendPck << PLAYERSNAME << players[0].name << players[1].name << players[2].name << players[3].name;
										sock.send(sendPck, players[i].ip, players[i].port);
									}
								}
							}
							else
							{
								sendPck << Code::ERROR_LOGIN;
								sock.send(sendPck, player.ip, player.port);
							}
							
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
	state = false;
	t.join();
	return 0;
}