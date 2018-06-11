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

void DisconnectUser(vector<sf::TcpSocket*>& list, sf::SocketSelector *ss, int i)
{

	//Lo eliminamos del Socket Selector
	ss->remove(*list[i]);

	//Desconectamos el socket del cliente
	list[i]->disconnect();

	delete(list[i]);

	//Eliminamos el socket de la lista
	list.erase(list.begin() + i, list.begin() + i + 1);

	
	//Informamos que un usuario se ha desconectado
	//TO DO


	/*
	for (sf::TcpSocket* socket : list)
	{
		string outMsn = std::to_string(0)+"A User Disconected";
		socket->send(outMsn.c_str(), outMsn.size() + 1);
	}*/
}

int main()
{
	//VARIABLES DE JUEGO
	int jugadorActual=-1;
	bool start = false;

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

	Game game;

	while (listenerStatus != sf::TcpListener::Status::Disconnected)
	{
		//Mientras haya elementos en el Socket Selector, esperara a que algun socket le envie algo.
		while (ss.wait())
		{
			//Comprovamos si es un Listener
			if (ss.isReady(listener))
			{

				//Se añade el socket al Socket Selector
				sf::TcpSocket* socket = new sf::TcpSocket();
				//Esperamos peticion de un cliente
				sf::TcpListener::Status st = listener.accept(*socket);
				if (st == sf::TcpListener::Status::Done)
				{
					socketList.push_back(socket);
				}
				ss.add(*socket);
				
				//Les indicamos que se ha connectado un nuevo usuario
				//TO DO

				/*msn = std::to_string(0)+"New User Connected!";
				for (sf::TcpSocket* s : socketList)
				{
					if (s == socketList[0] && socketList.size()== 4)
					{
						msn = std::to_string(4) + "New User Connected!";
					}

					if(s!=socket)s->send(msn.c_str(), msn.size() + 1);
				}
				cout << "New User Connecte" << endl;
				*/
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
							//TO DO

							{
								//msn = buffer;
								//int codigo = atoi(&buffer[0]);
								//sf::TcpSocket::Status st;
								//switch (codigo)
								//{
								//case 0:
								//	//Enviamos el mensajes
								//	/*for (int i = 0; i < socketList.size(); i++)
								//	{
								//		st = socketList[i]->send(msn.c_str(), msn.size() + 1);

								//		if (st == sf::TcpSocket::Status::Error)
								//		{
								//			cout << "Error al enviar" << endl;
								//		}

								//	}*/
								//	break;
								//case 1:
								//	msn.clear();
								//	//Subimos la cantidad para apostar
								//	game.apuestaActual += 10;

								//	//Le restamos dicha cantidad al jugador
								//	game.listPlayers[jugadorActual].efectivo -= game.apuestaActual;

								//	//Sumamos la cantidad a la mesa
								//	game.efectivoMesa += game.apuestaActual;
								//	
								//	//Le indicamos que la tranasacion es correcta
								//	msn = to_string(6)+to_string(game.apuestaActual);


								//	/*st = socketList[j]->send(msn.c_str(), msn.size() + 1);
								//	if (st == sf::TcpSocket::Status::Done)
								//	{
								//		cout << "ENVIADO" << endl;
								//	}*/

								//	//Pasamos de turno
								//	/*jugadorActual++;
								//	if (jugadorActual == 3)jugadorActual = 0;
								//	msn = to_string(4);
								//	socketList[jugadorActual]->send(msn.c_str(), msn.size() + 1);*/

								//break;
								//case 2:
								//	msn.clear();
								//	//Le restamos dicha cantidad al jugador
								//	game.listPlayers[jugadorActual].efectivo -= game.apuestaActual;

								//	//Sumamos la cantidad a la mesa
								//	game.efectivoMesa += game.apuestaActual;

								//	//Le indicamos que la tranasacion es correcta
								//	msn = to_string(7) + to_string(game.apuestaActual);


								//	/*st = socketList[j]->send(msn.c_str(), msn.size() + 1);
								//	if (st == sf::TcpSocket::Status::Done)
								//	{
								//		cout << "ENVIADO" << endl;
								//	}*/

								//	//Pasamos de turno
								//	/*jugadorActual++;
								//	if (jugadorActual == 3)jugadorActual = 0;
								//	msn = to_string(4);
								//	socketList[jugadorActual]->send(msn.c_str(), msn.size() + 1);*/

								//	break;
								//case 3:
								//	msn.clear();
								//	//Le indicamos que la tranasacion es correcta
								//	/*msn = to_string(8) + to_string(game.apuestaActual);
								//	st = socketList[j]->send(msn.c_str(), msn.size() + 1);*/

								//	//Pasamos de turno
								//	/*jugadorActual++;
								//	if (jugadorActual == 3)jugadorActual = 0;
								//	msn = to_string(4);
								//	st=socketList[jugadorActual]->send(msn.c_str(), msn.size() + 1);
								//	if (st == sf::TcpSocket::Status::Done)
								//	{
								//		cout << "ENVIADO" << endl;
								//	}*/
								//	break;
								//default:
								//	break;
								//}
								//for (int i = 0; i < socketList.size(); i++)
								//{
								//	cout << "Mensaje: " + msn << endl;
								//	st = socketList[i]->send(msn.c_str(), msn.size() + 1);

								//	if (st == sf::TcpSocket::Status::Error)
								//	{
								//		cout << "Error al enviar" << endl;
								//	}

								//}
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