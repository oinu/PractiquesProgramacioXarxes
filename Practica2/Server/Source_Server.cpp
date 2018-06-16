
#include <SFML\Network.hpp>
#include <iostream>
#include <thread>

struct Player
{
	sf::IpAddress ip;
	unsigned short port;
	std::string name;
	int x;
	int y;
	int points;
	int timeSendPosition = 0;
	int timeHello = 0;
	std::vector<int>acumulacionHello;
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
	HELLO, WELLCOME, ASK, PLAYERSNAME, STARTTIME, ENDTIME, ENDGAME, AFK,
	DISCONNECTED, ACOMOVE, ACK_ACOMOVE,PLAYER1,PLAYER2,PLAYER3,PLAYER4
};

sf::UdpSocket sock;
sf::Socket::Status status = sock.bind(50000);
std::vector<Player> players;
Player p;
Game g[5];
sf::Packet pck;
int index = -1;
int preguntaActual = 0;
bool state = true, start=false;
int currentTime =-1;



void ActualizarMarcadores()
{
	//Mirar las posiciones de los jugadores para determinar que opcion eligio
	for (int i = 0; i < players.size(); i++)
	{
		int x = players[i].x + 15;
		int y = players[i].y + 15;
		

		if (players[i].points != -1)
		{
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
	}

	//Pasamos las puntuaciones
	pck.clear();
	pck << ENDTIME << players[0].points << players[1].points << players[2].points << players[3].points;
	for (int i = 0; i < players.size(); i++)
	{
		if(players[i].points!=-1)sock.send(pck, players[i].ip, players[i].port);
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
		if (players[i].points != -1) sock.send(pck, players[i].ip, players[i].port);
	}
	pck.clear();

	//Informar que empieza la partida y el tiempo que tienen.
	currentTime = 10;
	pck << STARTTIME << currentTime;
	for (int i = 0; i < players.size(); i++)
	{
		if (players[i].points != -1) sock.send(pck, players[i].ip, players[i].port);
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
		else if (players[i].points == points && players[i].points != -1)
		{
			ganadores.push_back(i);
		}
	}

	std::string nombresGanadores="";
	for (int i = 0; i < ganadores.size(); i++)
	{
		nombresGanadores += players[i].name+ "	";
	}
	//Indicamos el jugador ganador
	pck.clear();
	pck << ENDGAME << nombresGanadores + "	WIN!!!";
	for (int i = 0; i < players.size(); i++)
	{
		if (players[i].points != -1)sock.send(pck, players[i].ip, players[i].port);
	}
	pck.clear();
}

void ActualizarTime()
{
	sf::Clock timer;
	while (state)
	{
		if (timer.getElapsedTime().asSeconds() >= 1 && start)
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
			//Incrementamos el tiempo de no movimiento
			std::vector<int>indexAfkPlayers;
			for (int i = 0; i<players.size(); i++)
			{
				if (players[i].points != -1)
				{
					players[i].timeSendPosition++;
					if (players[i].timeSendPosition >= 20)
					{
						indexAfkPlayers.push_back(i);
					}
				}
			}

			//Los jugadores que estan afk hace 10 segundos
			pck.clear();
			pck << AFK << (int)indexAfkPlayers.size();
			for (int i : indexAfkPlayers)
			{
				pck << i;
				sf::Packet dis;
				dis << DISCONNECTED;
				sock.send(dis, players[i].ip, players[i].port);
				players[i].points = -1;
			}

			//Se informa a los jugadores activos
			for (int i =0; i < players.size();i++)
			{
				if (players[i].points!= - 1) sock.send(pck, players[i].ip, players[i].port);
			}
			pck.clear();
			indexAfkPlayers.clear();

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
				int vacios = 0;
				//Buscamos que si ya es conocido
				for (int i = 0; i < players.size(); i++)
				{
					if (players[i].ip == p.ip && players[i].port == p.port)
						index = i;
					if (players[i].points == -1)vacios++;
				}
				if (vacios == players.size())players.clear();

				if (status == sf::Socket::Done)
				{
					int id, pos, aux;
					pck >> id;
					Player player;
					//std::cout << id << std::endl;
					switch (id)
					{
					case Code::HELLO:
						if (index == -1 && players.size() < 4 && !start)
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
								player.x = 400;
								player.y = 320;
							}
							else if (players.size() == 2)
							{
								player.x = 370;
								player.y = 350;
							}
							else if (players.size() == 3)
							{
								player.x = 400;
								player.y = 350;
							}

							//Guardamos el nombre recibido
							pck >> player.name;
							players.push_back(player);
							sf::Packet sendPck;
							int size = players.size();
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
								start = true;
							}
						}
						else if (players[index].points != -1)
						{
							pck >> pos;
							//comprovamos que conocemos todos los hello pendientes
							for (int i = 0; i < pos; i++)
							{
								pck >> id;
								for (int j = 0; j < players[index].acumulacionHello.size(); j++)
								{
									//Si es un hello perdido
									if (players[index].acumulacionHello[j] == id)aux = j;
								}
								//Lo añadimos
								players[index].acumulacionHello.push_back(aux);
							}
							
							players[index].timeHello = 0;

							//Si se acumulan mas de 10 paquetes se informa al cliente
							if (players[index].acumulacionHello.size() > 10)
							{
								pck.clear();
								pck << WELLCOME << players[index].acumulacionHello.size();
								for (int i : players[index].acumulacionHello)
								{
									pck << i;
								}
								sock.send(pck, players[index].ip, players[index].port);
								pck.clear();
								players[index].acumulacionHello.clear();
							}
						}
						break;

					case Code::DISCONNECTED:
						players[index].points = -1;
						pck.clear();
						pck << AFK << 1 << index;
						for (int i = 0; i < players.size(); i++)
						{
							if (index != i && players[i].points != -1)
							{
								if(players[i].points!=-1)sock.send(pck, players[i].ip, players[i].port);
							}
						}
						break;

					case Code::ACOMOVE:
						if(players[index].points != -1)
						{
							//id es la x y pos es la y
							pck >> id >> pos;
							id += players[index].x;
							pos += players[index].y;
							players[index].timeSendPosition = 0;

							if (id >= 0 && id + 30 <= 800 && pos >= 200 && pos + 30 <= 500)
							{

								players[index].x = id;
								players[index].y = pos;

								pck.clear();
								pck << ACK_ACOMOVE << id << pos;
								sock.send(pck, players[index].ip, players[index].port);
								pck.clear();

								//Miramos todos players 
								for (int i = 0; i < players.size(); i++)
								{
									//Evitamos mandar al mismo jugador
									if (index != i && players[index].points != -1)
									{
										//Si el jugado es....
										switch (index)
										{
										case 0:
											pck.clear();
											pck << PLAYER1 << id << pos;
											sock.send(pck, players[i].ip, players[i].port);
											break;
										case 1:
											pck.clear();
											pck << PLAYER2 << id << pos;
											sock.send(pck, players[i].ip, players[i].port);
											break;
										case 2:
											pck.clear();
											pck << PLAYER3 << id << pos;
											sock.send(pck, players[i].ip, players[i].port);
											break;
										case 3:
											pck.clear();
											pck << PLAYER4 << id << pos;
											sock.send(pck, players[i].ip, players[i].port);
											break;
										default:
											break;
										}
									}
								}
							}
							else
							{
								pck.clear();
								pck << -1;
								sock.send(pck, players[index].ip, players[index].port);
								pck.clear();
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