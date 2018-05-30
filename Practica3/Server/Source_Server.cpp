
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

struct Question
{
	std::string pregunta;
	std::string a;
	std::string b;
	std::string c;
	std::string d;
	int respuesta;
};

struct Game
{
	std::vector<Player> players;
	Question g[5];
	int preguntaActual = 0;
	int currentTime = -1;
};

enum Code
{
	XMOVE, YMOVE, HELLO, WELLCOME, XPLAYER1, YPLAYER1, XPLAYER2, YPLAYER2, XPLAYER3, YPLAYER3, XPLAYER4, YPLAYER4,
	ASK, PLAYERSNAME, STARTTIME, ENDTIME, ENDGAME, ERROR_LOGIN, CHANGE_PASSWORD, ERROR_CHANGE, NEW_USER, ERROR_NEW_USER,
	NEW_MATCH, NUMBER_GAMES,JOIN,MATCHES
};
sf::UdpSocket sock;
sf::Socket::Status status = sock.bind(50000);

Player p;
bool state = true;
sf::Packet pck;
int playerIndex = -1;
int menuPlayerIndex = -1;
int gameIndex = 0;
BBDD bd(IP_PORT, USER, PWD);
std::vector<Game> games;
std::vector<Player>menuPlayers;



void ActualizarMarcadores()
{
	//Mirar las posiciones de los jugadores para determinar que opcion eligio
	for (int i = 0; i < games[gameIndex].players.size(); i++)
	{
		int x = games[gameIndex].players[i].x + 15;
		int y = games[gameIndex].players[i].y + 15;
		

		//Opcion A
		if (x >= 0 && x <= 400 && y >= 200 && y <= 350 && games[gameIndex].g[games[gameIndex].preguntaActual].respuesta == 1)
		{
			games[gameIndex].players[i].points += 10;
		}

		//Opcion B
		else if (x >= 400 && x <= 800 && y >= 200 && y <= 350 && games[gameIndex].g[games[gameIndex].preguntaActual].respuesta == 2)
		{
			games[gameIndex].players[i].points += 10;
		}

		//Opcion C
		if (x >= 0 && x <= 400 && y >= 350 && y <= 500 && games[gameIndex].g[games[gameIndex].preguntaActual].respuesta == 3)
		{
			games[gameIndex].players[i].points += 10;
		}

		//Opcion D
		else if (x >= 400 && x <= 800 && y >= 350 && y <= 500 && games[gameIndex].g[games[gameIndex].preguntaActual].respuesta == 4)
		{
			games[gameIndex].players[i].points += 10;
		}
	}

	//Pasamos las puntuaciones
	pck.clear();
	pck << ENDTIME << games[gameIndex].players[0].points << games[gameIndex].players[1].points << games[gameIndex].players[2].points << games[gameIndex].players[3].points;
	for (int i = 0; i < games[gameIndex].players.size(); i++)
	{
		sock.send(pck, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
	}
	pck.clear();

	//Cambia de pregunta, actualmente hay 5
	if (games[gameIndex].preguntaActual<4)games[gameIndex].preguntaActual++;
}

void CreacionPreguntas()
{
	std::vector<int>idQuestions;
	for (int i = 0; i < 5; i++)
	{
		bd.ReturnQuestion(games[gameIndex].g[i].pregunta, games[gameIndex].g[i].a, games[gameIndex].g[i].b, games[gameIndex].g[i].c, games[gameIndex].g[i].d, games[gameIndex].g[i].respuesta,idQuestions);
	}
}

void NewGame()
{
	//Pasar pregunta y repuestas
	pck.clear();
	pck << ASK << games[gameIndex].g[games[gameIndex].preguntaActual].pregunta << games[gameIndex].g[games[gameIndex].preguntaActual].a << games[gameIndex].g[games[gameIndex].preguntaActual].b << games[gameIndex].g[games[gameIndex].preguntaActual].c << games[gameIndex].g[games[gameIndex].preguntaActual].d;
	for (int i = 0; i < games[gameIndex].players.size(); i++)
	{
		sock.send(pck, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
	}
	pck.clear();

	//Informar que empieza la partida y el tiempo que tienen.
	games[gameIndex].currentTime = 30;
	pck << STARTTIME << 30;
	for (int i = 0; i < games[gameIndex].players.size(); i++)
	{
		sock.send(pck, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
	}
	pck.clear();
}

void EndGame()
{
	std::vector<int>ganadores;
	int points=-1;
	//Guardamos el indice del jugador ganador
	for (int i = 0; i < games[gameIndex].players.size(); i++)
	{
		if (games[gameIndex].players[i].points > points)
		{
			ganadores.clear();
			ganadores.push_back(i);
			points = games[gameIndex].players[i].points;
		}
		else if (games[gameIndex].players[i].points == points)
		{
			ganadores.push_back(i);
		}
	}

	std::string nombresGanadores;
	for (int i = 0; i < ganadores.size(); i++)
	{
		nombresGanadores += games[gameIndex].players[i].name+ "	";
	}
	//Indicamos el jugador ganador
	pck.clear();
	pck << ENDGAME << nombresGanadores + "	WIN!!!";
	for (int i = 0; i < games[gameIndex].players.size(); i++)
	{
		sock.send(pck, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
	}
	pck.clear();
}

void ActualizarTime()
{
	sf::Clock timer;
	while (state)
	{
		if (timer.getElapsedTime().asSeconds() >= 1 && games.size()>0)
		{
			//Se resta de los segundos restantes
			games[gameIndex].currentTime--;
			//Si llega a cero, empieza una nueva
			if (games[gameIndex].currentTime == 0)
			{
				ActualizarMarcadores();
				if(games[gameIndex].preguntaActual<4)NewGame();
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

	bd.SelectDataBase(DATABASE);

	sf::Clock clock;

	//Mira si ha pasado un segundo
	std::thread t(ActualizarTime);

	do
	{
		pck.clear();
		if (clock.getElapsedTime().asMilliseconds() > 200)
		{
				status = sock.receive(pck, p.ip, p.port);
				playerIndex = -1;
				menuPlayerIndex = -1;
				
				//Si es usuario del menu
				for (int i = 0; i < menuPlayers.size(); i++)
				{
					if (menuPlayers[i].ip == p.ip && menuPlayers[i].port == p.port)
						menuPlayerIndex = i;
				}

				//Si es usuario de partida
				if (menuPlayerIndex == -1 && games.size()>0)
				{
					//Recurremos todas la partidas existentes
					for (int j = 0; j < games.size(); j++)
					{
						//Recurremos todos los usuarios de cada partida
						for (int i = 0; i < games[j].players.size(); i++)
						{
							//Si el ip y puerto coinciden se guardan los dos indices
							if (games[j].players[i].ip == p.ip && games[j].players[i].port == p.port)
							{
								playerIndex = i;
								gameIndex = j;
							}
								
						}
					}
					
				}
				

				if (status == sf::Socket::Done)
				{
					int id, pos;
					std::string name,email, password;
					pck >> id;
					Player player;
					Game g;

					switch (id)
					{

					case Code::XMOVE:
						//Movimiento en X
						pck >> pos;
						if (pos >= 0 && pos + 30 <= 800)
						{
							games[gameIndex].players[playerIndex].x = pos;
							sf::Packet pckSend;
							pckSend << XMOVE << pos;
							sock.send(pckSend, games[gameIndex].players[playerIndex].ip, games[gameIndex].players[playerIndex].port);

							//Miramos todos players 
							for (int i = 0; i < games[gameIndex].players.size(); i++)
							{
								//Evitamos mandar al mismo jugador
								if (playerIndex != i)
								{
									//Si el jugado es....
									switch (playerIndex)
									{
									case 0:
										pckSend.clear();
										pckSend << XPLAYER1 << pos;
										sock.send(pckSend, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
										break;
									case 1:
										pckSend.clear();
										pckSend << XPLAYER2 << pos;
										sock.send(pckSend, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
										break;
									case 2:
										pckSend.clear();
										pckSend << XPLAYER3 << pos;
										sock.send(pckSend, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
										break;
									case 3:
										pckSend.clear();
										pckSend << XPLAYER4 << pos;
										sock.send(pckSend, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
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
							sock.send(pckSend, games[gameIndex].players[playerIndex].ip, games[gameIndex].players[playerIndex].port);
						}
						break;

					case Code::YMOVE:
						//Movimiento en Y
						pck >> pos;
						if (pos >= 200 && pos + 30 <= 500)
						{
							games[gameIndex].players[playerIndex].y = pos;
							sf::Packet pckSend;
							pckSend << YMOVE << games[gameIndex].players[playerIndex].y;
							sock.send(pckSend, games[gameIndex].players[playerIndex].ip, games[gameIndex].players[playerIndex].port);

							//Miramos todos players 
							for (int i = 0; i < games[gameIndex].players.size(); i++)
							{
								//Evitamos mandar al mismo jugador
								if (playerIndex != i)
								{
									//Si el jugado es....
									switch (playerIndex)
									{
									case 0:
										pckSend.clear();
										pckSend << YPLAYER1 << pos;
										sock.send(pckSend, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
										break;
									case 1:
										pckSend.clear();
										pckSend << YPLAYER2 << pos;
										sock.send(pckSend, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
										break;
									case 2:
										pckSend.clear();
										pckSend << YPLAYER3 << pos;
										sock.send(pckSend, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
										break;
									case 3:
										pckSend.clear();
										pckSend << YPLAYER4 << pos;
										sock.send(pckSend, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
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
							sock.send(pckSend, games[gameIndex].players[playerIndex].ip, games[gameIndex].players[playerIndex].port);
						}
						break;

					case Code::HELLO:
						if (playerIndex == -1 && menuPlayerIndex==-1)
						{
							player.ip = p.ip;
							player.port = p.port;

							//Guardamos el nombre recibido
							std::string pwd, name;
							pck >> name >> pwd;

							User u = bd.ReturnUserByName(name);
							sf::Packet sendPck;
							if (u.password == pwd)
							{
								player.name = name;
								menuPlayers.push_back(player);
								sendPck << Code::WELLCOME<<(int)games.size();
								sock.send(sendPck, player.ip, player.port);

								for (int i=0; i<games.size();i++)
								{
									sendPck.clear();
									sendPck << MATCHES << games[i].players[0].name << (int)games[i].players.size();
									sock.send(sendPck, player.ip, player.port);
								}
							}
							else
							{
								sendPck << Code::ERROR_LOGIN;
								sock.send(sendPck, player.ip, player.port);
							}
						}
						/*
						if (playerIndex == -1 && games[gameIndex].players.size()<4)
						{
							//Guardamos la ip, el puerto y le indicamos la puntuacion
							player.ip = p.ip;
							player.port = p.port;
							player.points = 0;
							
							//Situamos los jugadores segun van llegando
							if (games[gameIndex].players.size() == 0)
							{
								player.x = 370;
								player.y = 320;
							}
							else if (games[gameIndex].players.size() == 1)
							{
								player.x = 430;
								player.y = 320;
							}
							else if (games[gameIndex].players.size() == 2)
							{
								player.x = 370;
								player.y = 380;
							}
							else if (games[gameIndex].players.size() == 3)
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
								int size = games[gameIndex].players.size();
								games[gameIndex].players.push_back(player);
								sendPck << Code::WELLCOME << size;
								sock.send(sendPck, player.ip, player.port);

								//Cuando llega el ultimo jugador.
								if (games[gameIndex].players.size() == 4)
								{
									NewGame();
									for (int i = 0; i < games[gameIndex].players.size(); i++)
									{
										sendPck.clear();
										sendPck << PLAYERSNAME << games[gameIndex].players[0].name << games[gameIndex].players[1].name << games[gameIndex].players[2].name << games[gameIndex].players[3].name;
										sock.send(sendPck, games[gameIndex].players[i].ip, games[gameIndex].players[i].port);
									}
								}
							}
							else
							{
								sendPck << Code::ERROR_LOGIN;
								sock.send(sendPck, player.ip, player.port);
							}
							
						}*/
						break;
					case CHANGE_PASSWORD:
						pck >> email >> password;
						
						if (bd.RestorePassword(email, password))
						{
							sf::Packet pckSend;
							pckSend << Code::CHANGE_PASSWORD;
							sock.send(pckSend, p.ip, p.port);
						}
						else
						{
							sf::Packet pckSend;
							pckSend << Code::ERROR_CHANGE;
							sock.send(pckSend, p.ip, p.port);
						}

						break;

					case NEW_USER:
						pck >> name >> password>> email;

						if (bd.InsertUser(name, password, email))
						{
							sf::Packet pckSend;
							pckSend << Code::NEW_USER;
							sock.send(pckSend, p.ip, p.port);
						}
						else
						{
							sf::Packet pckSend;
							pckSend << Code::ERROR_NEW_USER;
							sock.send(pckSend, p.ip, p.port);
						}

						break;

					case NEW_MATCH:
						//Añade el jugador
						g.players.push_back(menuPlayers[menuPlayerIndex]);
						menuPlayers.erase(menuPlayers.begin() + menuPlayerIndex);
						games.push_back(g);

						//Metemos el jugador en el creador de partida
						pck.clear();
						pck << JOIN;
						sock.send(pck, g.players[0].ip, g.players[0].port);

						//Indicamos que se ha creado una partida nueva
						for (auto a : menuPlayers)
						{
							pck.clear();
							pck <<NUMBER_GAMES<<(int) games.size()<<g.players[0].name<< (int)games[games.size()-1].players.size();
							sock.send(pck, a.ip, a.port);
						}
						break;
					case JOIN:
							pck >> pos;
							std::cout << pos;
							games[pos].players.push_back(menuPlayers[menuPlayerIndex]);
							menuPlayers.erase(menuPlayers.begin() + menuPlayerIndex);
							player = games[pos].players[games[pos].players.size() - 1];
							pck.clear();
							pck << JOIN;
							sock.send(pck, player.ip, player.port);
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