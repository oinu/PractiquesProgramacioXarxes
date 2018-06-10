
#include <SFML\Network.hpp>
#include <iostream>
#include <thread>
#include "BBDD.h"

#define IP_PORT "192.168.1.143:3306"
#define USER "root"
#define PWD "linux123"
#define DATABASE "game"

struct Player
{
	sf::IpAddress ip;
	unsigned short port;
	bool ready=false;
	std::string name;
	int x;
	int y;
	int points;
	int idPlayer;
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
	std::vector<std::string>text;
	Question g[5];
	int preguntaActual = 0;
	int currentTime = 30;
	bool play = false;
};

enum Code
{
	XMOVE, YMOVE, HELLO, WELLCOME, XPLAYER1, YPLAYER1, XPLAYER2, YPLAYER2, XPLAYER3, YPLAYER3, XPLAYER4, YPLAYER4,
	ASK, PLAYERSNAME, STARTTIME, ENDTIME, ENDGAME, ERROR_LOGIN, CHANGE_PASSWORD, ERROR_CHANGE, NEW_USER, ERROR_NEW_USER,
	NEW_MATCH, NUMBER_GAMES,JOIN,MATCHES, GO, PLAYERREADY,CURRENTPLAYERS,STARTGAME,EXIT,UPDATEMATCHES, MENSAJE
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



void ActualizarMarcadores(int game)
{
	//Mirar las posiciones de los jugadores para determinar que opcion eligio
	for (int i = 0; i < games[game].players.size(); i++)
	{
		int x = games[game].players[i].x + 15;
		int y = games[game].players[i].y + 15;
		

		//Opcion A
		if (x >= 0 && x <= 400 && y >= 200 && y <= 350 && games[game].g[games[game].preguntaActual].respuesta == 1)
		{
			games[game].players[i].points += 10;
		}

		//Opcion B
		else if (x >= 400 && x <= 800 && y >= 200 && y <= 350 && games[game].g[games[game].preguntaActual].respuesta == 2)
		{
			games[game].players[i].points += 10;
		}

		//Opcion C
		if (x >= 0 && x <= 400 && y >= 350 && y <= 500 && games[game].g[games[game].preguntaActual].respuesta == 3)
		{
			games[game].players[i].points += 10;
		}

		//Opcion D
		else if (x >= 400 && x <= 800 && y >= 350 && y <= 500 && games[game].g[games[game].preguntaActual].respuesta == 4)
		{
			games[game].players[i].points += 10;
		}
	}

	//Pasamos las puntuaciones
	pck.clear();
	pck << ENDTIME << games[game].players[0].points << games[game].players[1].points << games[game].players[2].points << games[game].players[3].points;
	for (int i = 0; i < games[game].players.size(); i++)
	{
		sock.send(pck, games[game].players[i].ip, games[game].players[i].port);
	}
	pck.clear();

	//Cambia de pregunta, actualmente hay 5
	if (games[game].preguntaActual<4)games[game].preguntaActual++;
}

void CreacionPreguntas(int game)
{
	std::vector<int>idQuestions;
	for (int i = 0; i < 5; i++)
	{
		bd.ReturnQuestion(games[game].g[i].pregunta, games[game].g[i].a, games[game].g[i].b, games[game].g[i].c, games[game].g[i].d, games[game].g[i].respuesta,idQuestions);
	}
}

void NewGame(int game)
{
	//Pasar pregunta y repuestas
	pck.clear();
	pck << ASK << games[game].g[games[game].preguntaActual].pregunta << games[game].g[games[game].preguntaActual].a << games[game].g[games[game].preguntaActual].b << games[game].g[games[game].preguntaActual].c << games[game].g[games[game].preguntaActual].d;
	for (int i = 0; i < games[game].players.size(); i++)
	{
		sock.send(pck, games[game].players[i].ip, games[game].players[i].port);
	}
	pck.clear();

	//Informar que empieza la partida y el tiempo que tienen.
	games[game].currentTime = 30;
	pck << STARTTIME << 30;
	for (int i = 0; i < games[game].players.size(); i++)
	{
		sock.send(pck, games[game].players[i].ip, games[game].players[i].port);
	}
	pck.clear();
}

void EndGame(int game)
{
	std::vector<int>ganadores;
	int points=-1;
	//Guardamos el indice del jugador ganador
	for (int i = 0; i < games[game].players.size(); i++)
	{
		if (games[game].players[i].points > points)
		{
			ganadores.clear();
			ganadores.push_back(i);
			points = games[game].players[i].points;
		}
		else if (games[game].players[i].points == points)
		{
			ganadores.push_back(i);
		}
	}

	std::string nombresGanadores;
	for (int i = 0; i < ganadores.size(); i++)
	{
		nombresGanadores += games[game].players[i].name+ "	";
	}
	//Indicamos el jugador ganador
	pck.clear();
	pck << ENDGAME << nombresGanadores + "	WIN!!!";
	for (int i = 0; i < games[game].players.size(); i++)
	{
		sock.send(pck, games[game].players[i].ip, games[game].players[i].port);
	}
	pck.clear();

	for (int i = 0; i < 4; i++)
	{
		menuPlayers.push_back(games[game].players[i]);
	}
	games.erase(games.begin() + game);
	
	//Hacer el paquete
	pck.clear();
	int size = games.size();
	if (size > 6)size = 6;
	pck << UPDATEMATCHES << size;
	for (int i=0; i<size; i++)
	{
		pck << games[i].players[0].name << (int)games[i].players.size();
	}

	for (auto a : menuPlayers)
	{
		sock.send(pck, a.ip, a.port);
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
			for (int i=0; i<games.size();i++)
			{
				//Se resta de los segundos restantes
				if (games[i].play)games[i].currentTime--;
				//Si llega a cero, empieza una nueva
				if (games[i].currentTime == 0)
				{
					ActualizarMarcadores(i);
					if (games[i].preguntaActual<4)NewGame(i);
					else EndGame(i);
				}
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
	bd.UpdateSessionEnd(1);

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
					bool ready;
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
							player.points = 0;

							//Guardamos el nombre recibido
							std::string pwd, name;
							pck >> name >> pwd;

							User u = bd.ReturnUserByName(name);
							sf::Packet sendPck;
							if (u.password == pwd)
							{
								bd.InsertSession(u.id);
								player.idPlayer = u.id;
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
						pck << JOIN << 0<< g.players[0].name;
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
							if (games[pos].players.size() < 4)
							{
								games[pos].players.push_back(menuPlayers[menuPlayerIndex]);
								menuPlayers.erase(menuPlayers.begin() + menuPlayerIndex);
								player = games[pos].players[games[pos].players.size() - 1];
								id = games[pos].players.size() - 1;

								pck.clear();
								pck << JOIN << id;
								switch (id)
								{
								case 1:
									pck << games[pos].players[0].name << games[pos].players[1].name;
									break;
								case 2:
									pck << games[pos].players[0].name << games[pos].players[1].name << games[pos].players[2].name;
									break;
								case 3:
									pck << games[pos].players[0].name << games[pos].players[1].name << games[pos].players[2].name << games[pos].players[3].name;
									break;
								default:
									break;
								}
								sock.send(pck, player.ip, player.port);
								if (id < 4)
								{
									pck.clear();
									pck << CURRENTPLAYERS << id;
									pck << player.name;
									for (int i = 0; i < id; i++)
									{
										sock.send(pck, games[pos].players[i].ip, games[pos].players[i].port);
									}
									pck.clear();
								}
							}
							break;
					case GO:
						games[gameIndex].players[playerIndex].ready = true;
						ready = true;
						for (auto a : games[gameIndex].players)
						{
							if (!a.ready) ready = false;
						}
						if (!ready)
						{
							pck.clear();
							pck << Code::PLAYERREADY << (int)playerIndex;
							for (auto a : games[gameIndex].players)
							{
								sock.send(pck, a.ip, a.port);
							}
							pck.clear();
						}
						else
						{
							if (games[gameIndex].players.size() == 4)
							{
								CreacionPreguntas(gameIndex);
								NewGame(gameIndex);
								pck.clear();
								pck << Code::STARTGAME;
								for (auto a : games[gameIndex].players)
								{
									sock.send(pck, a.ip, a.port);
									bd.UpdateSessionNumGames(a.idPlayer);
								}
								pck.clear();
								games[gameIndex].play = true;

							}
						}
						break;

					case EXIT:
						if (menuPlayerIndex == -1)
						{
							bd.UpdateSessionEnd(games[gameIndex].players[playerIndex].idPlayer);
							games[gameIndex].players.erase(games[gameIndex].players.begin() + playerIndex);
						}
						else
						{
							bd.UpdateSessionEnd(menuPlayers[menuPlayerIndex].idPlayer);
							menuPlayers.erase(menuPlayers.begin() + menuPlayerIndex);
						}
						break;

					case MENSAJE:
						pck >> name;
						name = games[gameIndex].players[playerIndex].name + ": " + name;
						if (games[gameIndex].text.size() == 15)
						{
							games[gameIndex].text.erase(games[gameIndex].text.begin());
						}
						games[gameIndex].text.push_back(name);
						name = "";
						for (std::string s : games[gameIndex].text)
						{
							name += s + "\n";
						}

						pck.clear();
						pck<<MENSAJE << name;
						for (Player a : games[gameIndex].players)
						{
							sock.send(pck, a.ip, a.port);
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