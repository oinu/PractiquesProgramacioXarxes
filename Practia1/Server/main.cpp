#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "../Client/Game.h"

#define MAX_MENSAJES 30
#define SERVER_PORT 50000
#define BUFFER_SIZE 2000


using namespace std;
Game game;

void DisconnectUser(vector<sf::TcpSocket*>& list, sf::SocketSelector *ss, int i)
{
	//Informamos que un usuario se ha desconectado

	string text = game.listPlayers[i].name;
	text += " is disconnected";
	sf::Packet send;
	send << Code::MENSAJE << text;
	for (auto s : list)
	{
		s->send(send);
	}

	//Lo eliminamos del Socket Selector
	ss->remove(*list[i]);

	//Desconectamos el socket del cliente
	list[i]->disconnect();

	delete(list[i]);

	//Eliminamos el socket de la lista
	list.erase(list.begin() + i, list.begin() + i + 1);
}
void DarCartasJugadores(vector<sf::TcpSocket*>& list)
{
	sf::Packet send;
	for (int i = 0; i < list.size(); i++)
	{
		game.listPlayers[i].carta1 = game.DarCarta();
		game.listPlayers[i].carta2 = game.DarCarta();

		send.clear();
		send << CARTAS << game.listPlayers[i].carta1.numero << (int)game.listPlayers[i].carta1.palo;
		send << game.listPlayers[i].carta2.numero << (int)game.listPlayers[i].carta2.palo;
		list[i]->send(send);
	}
}
void PonerCartasTablero(vector<sf::TcpSocket*>& list)
{
	for (int i = 0; i < 5; i++)
	{
		game.cartasTablero[i] = game.DarCarta();
	}
	sf::Packet pck;
	pck << TABLERO << 3;
	for (int i = 0; i < 3; i++)
	{
		pck << game.cartasTablero[i].numero;
		pck << (int)game.cartasTablero[i].palo;
	}
	for (auto p : list)
	{
		p->send(pck);
	}
	
}

bool Pareja(Player p)
{
	if (p.carta1.numero == p.carta1.numero)
		return true;
	else
	{
		for (int i = 0; i < 5; i++)
		{
			if (p.carta1.numero == game.cartasTablero[i].numero)return true;
			else if(p.carta2.numero == game.cartasTablero[i].numero)return true;
		}
		return false;
	}
}
bool DoblePareja(Player p)
{
	bool primera=false, segunda=false;
	for (int i = 0; i < 5; i++)
	{
		if (p.carta1.numero == game.cartasTablero[i].numero)primera= true;
		else if (p.carta2.numero == game.cartasTablero[i].numero)segunda= true;
	}
	return primera && segunda;
}
bool CartaAlta(Player p)
{
	bool primera,segunda;
	for (int i = 0; i < 4; i++)
	{
		primera = (p.carta1.numero > game.listPlayers[i].carta1.numero || p.carta1.numero > game.listPlayers[i].carta2.numero);
		segunda = (p.carta2.numero > game.listPlayers[i].carta1.numero || p.carta2.numero > game.listPlayers[i].carta2.numero);
		if (!primera && !segunda) return false;
	}
	return true;
}
bool Trio(Player p)
{
	if (p.carta1.numero == p.carta1.numero)
	{
		for (int i = 0; i < 5; i++)
		{
			if (p.carta1.numero == game.cartasTablero[i].numero)return true;
		}
		return false;
	}
	else
	{
		bool primera, segunda;
		for (int i = 0; i < 5; i++)
		{
			if (p.carta1.numero == game.cartasTablero[i].numero)
			{
				for (int j = i+1; j < 5; j++)
				{
					if (p.carta1.numero == game.cartasTablero[j].numero) return true;
				}
			}
			else if (p.carta2.numero == game.cartasTablero[i].numero) 
			{
				for (int j = i + 1; j < 5; j++)
				{
					if (p.carta2.numero == game.cartasTablero[j].numero) return true;
				}
			}
		}
		return false;
	}
}
bool Full(Player p)
{
	if (p.carta1.numero == p.carta2.numero)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = i + 1; j < 4; j++)
			{
				if (game.cartasTablero[i].numero == game.cartasTablero[i].numero && game.cartasTablero[i].numero == game.cartasTablero[j + 1].numero)
				{
					return true;
				}
			}
		}

		return false;
	}
	else
	{
		return Pareja(p) && Trio(p);
	}
}
bool Pocker(Player p)
{
	if (p.carta1.numero == p.carta2.numero)
	{
		bool primera = false;
		bool segunda = false;
		for (int i = 0; i < 5; i++)
		{
			if (primera)
			{
				segunda = p.carta1.numero == game.cartasTablero[i].numero;
			}
			else primera= p.carta1.numero == game.cartasTablero[i].numero;
		}

		return primera && segunda;
	}
	else return false;
}

int main()
{
	//VARIABLES DE JUEGO
	int jugadorActual=-1;
	bool start = false;
	int cartasMostrar = 3;
	cout << "CREANDO BARAJA ESPERA..." << endl;
	game.InitServer();
	cout << "PUEDE CONNECTARSE!" << endl;

	//ESTABLECER CONNECION
	sf::SocketSelector ss;
	vector<sf::TcpSocket*> socketList;
	sf::TcpSocket::Status socketStatus;

	sf::TcpListener listener;
	sf::TcpListener::Status listenerStatus;

	char buffer[BUFFER_SIZE];
	string msn;
	size_t recived;	

	//Le indicamos a que puerto debe escuchar para el servidor
	listenerStatus =listener.listen(SERVER_PORT);

	//Añadimos el listener al Socket Selector
	ss.add(listener);

	while (listenerStatus != sf::TcpListener::Status::Disconnected)
	{
		//Mientras haya elementos en el Socket Selector, esperara a que algun socket le envie algo.
		while (ss.wait())
		{
			//Comprovamos si es un Listener
			if (ss.isReady(listener) && socketList.size()<4)
			{
				jugadorActual = 0;
				//Se añade el socket al Socket Selector
				sf::TcpSocket* socket = new sf::TcpSocket();
				//Esperamos peticion de un cliente
				sf::TcpListener::Status st = listener.accept(*socket);
				if (st == sf::TcpListener::Status::Done)
				{
					socketList.push_back(socket);
				}
				ss.add(*socket);

				if (socketList.size() == 4 && !start)
				{
					start = true;
					sf::Packet pck;
					pck << TURNO;
					socketList[0]->send(pck);
					DarCartasJugadores(socketList);
					PonerCartasTablero(socketList);
				}
			}

			//Si no es el listener, sera un socket
			else
			{

				//Miramos de que socket recivimos el mensaje
				for (int j = 0; j < socketList.size(); j++)
				{
					//Encontramos el socket
					if (ss.isReady(*socketList[j]))
					{
						//Recivimos el mensaje.
						sf::Packet pck;
						socketStatus = socketList[j]->receive(pck);

						//Pasar el contenido del buffer, al string de mensaje.
						if (socketStatus == sf::TcpSocket::Status::Done)
						{
							int code;
							string text;
							pck >> code;

							if (code == Code::MENSAJE)
							{
								pck >> text;
								sf::Packet send;
								send << Code::MENSAJE << text;
								for (auto s : socketList)
								{
									s->send(send);
								}
							}
							else if (code == Code::HELLO)
							{
								pck >> text;
								game.listPlayers[jugadorActual].name = text;
								text += " is connected";
								sf::Packet sendPck;
								sendPck << MENSAJE << text;
								for (int i = 0; i < socketList.size(); i++)
								{
									if (i != j)socketList[i]->send(sendPck);
								}
							}
							else if (!game.listPlayers[jugadorActual].passa)
							{
								if (code == Code::IGUALAR)
								{
									game.listPlayers[jugadorActual].efectivo -= game.apuestaActual;
									game.efectivoMesa += game.apuestaActual;
									sf::Packet send;
									send << Code::IGUALAR << (int)game.efectivoMesa;
									for (auto s : socketList)
									{
										socketStatus = s->send(send);
										if (socketStatus == sf::TcpSocket::Status::Disconnected)
										{
											DisconnectUser(socketList, &ss, j);
										}
									}
									send.clear();
									send << COINS << game.listPlayers[jugadorActual].efectivo;
									socketStatus = socketList[j]->send(send);

									if (socketStatus == sf::TcpSocket::Status::Disconnected)
									{
										DisconnectUser(socketList, &ss, j);
									}
								}

								else if (code == Code::SUBIR)
								{
									game.apuestaActual += 10;
									game.listPlayers[jugadorActual].efectivo -= game.apuestaActual;
									game.efectivoMesa += game.apuestaActual;

									sf::Packet send;
									send << Code::SUBIR << (int)game.apuestaActual << (int)game.efectivoMesa;
									for (auto s : socketList)
									{
										socketStatus = s->send(send);
										if (socketStatus == sf::TcpSocket::Status::Disconnected)
										{
											DisconnectUser(socketList, &ss, j);
										}
									}
									send.clear();
									send << COINS << game.listPlayers[jugadorActual].efectivo;
									socketStatus = socketList[j]->send(send);

									if (socketStatus == sf::TcpSocket::Status::Disconnected)
									{
										DisconnectUser(socketList, &ss, j);
									}
								}

								else if (code == Code::DESCARTAR)
								{
									game.listPlayers[jugadorActual].passa = true;
								}

								do
								{
									jugadorActual++;
									if (jugadorActual == socketList.size())
									{
										jugadorActual = 0;
										cartasMostrar++;
										if (cartasMostrar < 6)
										{
											sf::Packet pck;
											pck << TABLERO << cartasMostrar;
											for (int i = 0; i < cartasMostrar; i++)
											{
												pck << game.cartasTablero[i].numero;
												pck << (int)game.cartasTablero[i].palo;
											}
											for (auto p : socketList)
											{
												p->send(pck);
											}
										}
										else
										{
											//Comprovamos que manos tienen
											std::vector<int>ganadores;
											for (int i = 0; i < 4; i++)
											{
												if(game.listPlayers[i].passa)ganadores.push_back(-1);
												else
												{
													if (Pocker(game.listPlayers[i]))
													{
														ganadores.push_back(6);
													}
													else if (Full(game.listPlayers[i]))
													{
														ganadores.push_back(5);
													}
													else if (DoblePareja(game.listPlayers[i]))
													{
														ganadores.push_back(4);
													}
													else if (Trio(game.listPlayers[i]))
													{
														ganadores.push_back(3);
													}
													else if (Pareja(game.listPlayers[i]))
													{
														ganadores.push_back(2);
													}
													else
													{
														ganadores.push_back(0);
													}
												}
											}

											//Miramos si alguien gana por algo que no sea carta alta
											int index = -1, puntos = 0;
											for (int i =0; i<ganadores.size(); i++)
											{
												if (ganadores[i] > puntos)
												{
													puntos = ganadores[i];
													index = i;
												}
											}

											//Comprovamos que no sea carta alta
											if (index == -1)
											{
												for (int i = 0; i < 4; i++)
												{
													if (CartaAlta(game.listPlayers[i]) && !game.listPlayers[i].passa)
													{
														index = i;
													}
												}
											}

											for (int i = 0; i < 4; i++)
											{
												game.listPlayers[i].passa = false;
											}

											game.listPlayers[index].efectivo += game.efectivoMesa;
											game.efectivoMesa = 0;
											game.apuestaActual = 10;

											//Actualizamos marcadores
											for (int i = 0; i < socketList.size(); i++)
											{
												sf::Packet packet;
												packet << COINS << game.listPlayers[i].efectivo;
												socketList[i]->send(packet);
											}

											//Nueva partida
											for (int i = 0; i < socketList.size(); i++)
											{
												sf::Packet packet;
												packet << NEW_GAME;
												socketList[i]->send(packet);
											}
											//Otra baraja
											for (int i = 0; i < socketList.size(); i++)
											{
												sf::Packet packet;
												packet << MENSAJE << "GANADOR: "+ game.listPlayers[index].name;
												socketList[i]->send(packet);
											}
											for (int i = 0; i < socketList.size(); i++)
											{
												sf::Packet packet;
												packet << MENSAJE <<"CARGANDO PARTIDA. SERVIDOR OCUPADO";
												socketList[i]->send(packet);
											}
											game.InitServer();
											DarCartasJugadores(socketList);
											PonerCartasTablero(socketList);
											cartasMostrar = 3;


										}
										
									}

									if (!game.listPlayers[jugadorActual].passa)
									{
										pck.clear();
										pck << TURNO;
										socketStatus == socketList[jugadorActual]->send(pck);
										if (socketStatus == sf::TcpSocket::Status::Disconnected)
										{
											DisconnectUser(socketList, &ss, jugadorActual);
										}
									}

								} while ((socketStatus == sf::TcpSocket::Status::Disconnected && socketList.size() > 0) || (game.listPlayers[jugadorActual].passa&& socketList.size() > 0));
							}
						}

						//Si se ha desconnectado
						else if (socketStatus == sf::TcpSocket::Status::Disconnected)
						{
							DisconnectUser(socketList, &ss, j);
						}

						break;
					}
				}
			}
		}
	}


	//Vaciar el Socket Selector
	ss.clear();

	//Paramos de escuchar el puerto, para así dejarlo libre.
	listener.close();

	//Desconnectamos todos los clientes
	for (sf::TcpSocket* &socket : socketList)
	{
		socket->disconnect();
	}

	//Limpiamos el vector
	socketList.clear();

}