#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "Game.h"

#define MAX_MENSAJES 30
#define CLIENT_PORT 50000
#define BUFFER_SIZE 2000


using namespace std;
std::mutex mu;
bool turno = false;
std::vector<Carta>baraja;
Game game;

int numeroCarta1;
int numeroCarta2;
Palos pCarta1;
Palos pCarta2;
std::string sCarta1;
std::string sCarta2;
std::string tablero[5];

//Escucha por un socket determinado y escribe el mensaje llegado.
//socket: Es el socket al cual debemos escuchar.
//recived: La longitud de los datos que recibiremos.
//aMensajes: Es donde contendra todos los mensajes del chat.
void RecivedFunction(sf::TcpSocket* socket,size_t* recived, vector<string>* aMensajes, sf::RenderWindow* window)
{
	while (window->isOpen())
	{
		int code;
		string text;
		sf::Packet pck;
		sf::TcpSocket::Status statusSocket;
		statusSocket=socket->receive(pck);
		if (statusSocket == sf::TcpSocket::Status::Done)
		{
			pck >> code;
			if (code == Code::MENSAJE)
			{
				pck >> text;
				mu.lock();
				aMensajes->push_back(text);
				if (aMensajes->size() > 9)
				{
					aMensajes->erase(aMensajes->begin(), aMensajes->begin() + 1);
				}
				mu.unlock();
			}
			else if (code == Code::SUBIR)
			{
				pck >> game.apuestaActual>>game.efectivoMesa;
			}
			else if (code == Code::IGUALAR)
			{
				pck >> game.efectivoMesa;
			}
			else if (code == Code::COINS)
			{
				pck >> game.listPlayers[0].efectivo;
			}
			else if (code == Code::TURNO)
			{
				turno = true;
			}
			else if (code == Code::CARTAS)
			{
				int num, palo;
				pck >> num >> palo;

				std::string s;
				//Diamantes
				if (palo == 0)
				{
					if (num == 1)
					{
						s = "./img/A_D.png";
					}
					else if (num == 11)
					{
						s = "./img/J_D.png";
					}
					else if (num == 12)
					{
						s = "./img/Q_D.png";
					}
					else if (num == 13)
					{
						s = "./img/K_D.png";
					}
					else
					{
						s = "./img/" + std::to_string(num) + "_D.png";
					}
				}
				//TREBOLS
				if (palo == 1)
				{
					if (num == 1)
					{
						s = "./img/A_T.png";
					}
					else if (num == 11)
					{
						s = "./img/J_T.png";
					}
					else if (num == 12)
					{
						s = "./img/Q_T.png";
					}
					else if (num == 13)
					{
						s = "./img/K_T.png";
					}
					else
					{
						s = "./img/" + std::to_string(num) + "_T.png";
					}
				}
				//PICAS
				if (palo == 2)
				{
					if (num == 1)
					{
						s = "./img/A_A.png";
					}
					else if (num == 11)
					{
						s = "./img/J_A.png";
					}
					else if (num == 12)
					{
						s = "./img/Q_A.png";
					}
					else if (num == 13)
					{
						s = "./img/K_A.png";
					}
					else
					{
						s = "./img/" + std::to_string(num) + "_A.png";
					}
				}
				//CORAZONES
				if (palo == 3)
				{
					if (num == 1)
					{
						s = "./img/A_H.png";
					}
					else if (num == 11)
					{
						s = "./img/J_H.png";
					}
					else if (num == 12)
					{
						s = "./img/Q_H.png";
					}
					else if (num == 13)
					{
						s = "./img/K_H.png";
					}
					else
					{
						s = "./img/" + std::to_string(num) + "_H.png";
					}
				}
				sCarta1 = s;

				pck >> num >> palo;
				//Diamantes
				if (palo == 0)
				{
					if (num == 1)
					{
						s = "./img/A_D.png";
					}
					else if (num == 11)
					{
						s = "./img/J_D.png";
					}
					else if (num == 12)
					{
						s = "./img/Q_D.png";
					}
					else if (num == 13)
					{
						s = "./img/K_D.png";
					}
					else
					{
						s = "./img/" + std::to_string(num) + "_D.png";
					}
				}
				//TREBOLS
				if (palo == 1)
				{
					if (num == 1)
					{
						s = "./img/A_T.png";
					}
					else if (num == 11)
					{
						s = "./img/J_T.png";
					}
					else if (num == 12)
					{
						s = "./img/Q_T.png";
					}
					else if (num == 13)
					{
						s = "./img/K_T.png";
					}
					else
					{
						s = "./img/" + std::to_string(num) + "_T.png";
					}
				}
				//PICAS
				if (palo == 2)
				{
					if (num == 1)
					{
						s = "./img/A_A.png";
					}
					else if (num == 11)
					{
						s = "./img/J_A.png";
					}
					else if (num == 12)
					{
						s = "./img/Q_A.png";
					}
					else if (num == 13)
					{
						s = "./img/K_A.png";
					}
					else
					{
						s = "./img/" + std::to_string(num) + "_A.png";
					}
				}
				//CORAZONES
				if (palo == 3)
				{
					if (num == 1)
					{
						s = "./img/A_H.png";
					}
					else if (num == 11)
					{
						s = "./img/J_H.png";
					}
					else if (num == 12)
					{
						s = "./img/Q_H.png";
					}
					else if (num == 13)
					{
						s = "./img/K_H.png";
					}
					else
					{
						s = "./img/" + std::to_string(num) + "_H.png";
					}
				}
				sCarta2 = s;

			}
			else if (code == Code::TABLERO)
			{
				int size;
				int num, palo;
				std::string s;
				pck >> size;
				for (int i = 0; i < size; i++)
				{
					s = "";
					pck >> num >> palo;
					//Diamantes
					if (palo == 0)
					{
						if (num == 1)
						{
							s = "./img/A_D.png";
						}
						else if (num == 11)
						{
							s = "./img/J_D.png";
						}
						else if (num == 12)
						{
							s = "./img/Q_D.png";
						}
						else if (num == 13)
						{
							s = "./img/K_D.png";
						}
						else
						{
							s = "./img/" + std::to_string(num) + "_D.png";
						}
					}
					//TREBOLS
					else if (palo == 1)
					{
						if (num == 1)
						{
							s = "./img/A_T.png";
						}
						else if (num == 11)
						{
							s = "./img/J_T.png";
						}
						else if (num == 12)
						{
							s = "./img/Q_T.png";
						}
						else if (num == 13)
						{
							s = "./img/K_T.png";
						}
						else
						{
							s = "./img/" + std::to_string(num) + "_T.png";
						}
					}
					//PICAS
					else if (palo == 2)
					{
						if (num == 1)
						{
							s = "./img/A_A.png";
						}
						else if (num == 11)
						{
							s = "./img/J_A.png";
						}
						else if (num == 12)
						{
							s = "./img/Q_A.png";
						}
						else if (num == 13)
						{
							s = "./img/K_A.png";
						}
						else
						{
							s = "./img/" + std::to_string(num) + "_A.png";
						}
					}
					//CORAZONES
					else if (palo == 3)
					{
						if (num == 1)
						{
							s = "./img/A_H.png";
						}
						else if (num == 11)
						{
							s = "./img/J_H.png";
						}
						else if (num == 12)
						{
							s = "./img/Q_H.png";
						}
						else if (num == 13)
						{
							s = "./img/K_H.png";
						}
						else
						{
							s = "./img/" + std::to_string(num) + "_H.png";
						}
					}
					else
					{
						s = "./img/Reverso.png";
					}
					tablero[i] = s;
				}
			}
			else if (code == Code::NEW_GAME)
			{
				sCarta1 = "./img/Reverso.png";
				sCarta2 = "./img/Reverso.png";
				for (int i = 0; i < 5; i++)
				{
					tablero[i]= "./img/Reverso.png";
				}
				game.apuestaActual = 10;
				game.efectivoMesa = 0;
			}
		}
	}
	
}

int main()
{
	string playerName;
	cout << "INTRODUCE NOMBRE USUARIO: ";
	cin >> playerName;
	//ESTABLECER CONNECION
	sf::TcpSocket socket;
	sf::TcpSocket::Status socketStatus;
	size_t recived;
	sCarta2 = "./img/Reverso.png";
	sCarta1 = "./img/Reverso.png";
	for (int i =0 ;i<5;i++)
	{
		tablero[i]= "./img/Reverso.png";
	}

	// Obtenemos nuestra direccion ip, y nos connectamos con el puerto indicado y nuestra ip
	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	socket.connect(ip, CLIENT_PORT);
	sf::Packet p;
	p << HELLO << playerName;
	socketStatus=socket.send(p);
	if (socketStatus == sf::TcpSocket::Status::Disconnected)
	{
		cout << "SERVIDOR DESCONNECTADO";
	}

	std::vector<std::string> aMensajes;

	sf::Vector2i screenDimensions(800, 800);

	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Gold'en Texas Poker");

	sf::Font font;
	if (!font.loadFromFile("Roboto-Bold.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	sf::String mensaje = " >";

	sf::Text chattingText(mensaje, font, 24);
	chattingText.setFillColor(sf::Color(0, 160, 0));
	chattingText.setStyle(sf::Text::Bold);


	sf::Text text(mensaje, font, 24);
	text.setFillColor(sf::Color(0, 160, 0));
	text.setStyle(sf::Text::Bold);
	text.setPosition(0, 760);

	sf::RectangleShape separator(sf::Vector2f(800, 5));
	separator.setFillColor(sf::Color(200, 200, 200, 255));
	separator.setPosition(0, 550);

	sf::RectangleShape separator2(sf::Vector2f(800, 5));
	separator2.setFillColor(sf::Color(200, 200, 200, 255));
	separator2.setPosition(0, 750);

	string msn;

	//EL JUEGO
	game.InitClient(font);

	//RECIVE
	//Se genera un thread (hilo), que escucha si llegan mensajes o no.
	thread t(RecivedFunction, &socket, &recived, &aMensajes, &window);

	while (window.isOpen())
	{
		sf::Event evento;
		sf::Vector2i mousePosition;
		while (window.pollEvent(evento))
		{
			switch (evento.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (evento.key.code == sf::Keyboard::Escape)
					window.close();
				else if (evento.key.code == sf::Keyboard::Return)
				{
					// SEND
					// Pasamos el mensaje a std::string para hacerlo mas facil en el momento de enviarlo.
					sf::Packet send;
					string s = mensaje;
					s.erase(s.begin()+1);
					s = playerName + ": " + s;
					send << Code::MENSAJE << s;
					socketStatus = socket.send(send);

					if (socketStatus == sf::TcpSocket::Status::Disconnected)
					{
						msn = "Server Disconnected";
						aMensajes.push_back(msn);
						if (aMensajes.size() > 9)
						{
							aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
						}
					}
					mensaje = ">";
				}
				break;
			case sf::Event::MouseButtonPressed:
				mousePosition = sf::Mouse::getPosition(window);
				if(turno)
				{

					//DESCARTAR MANO
					if (mousePosition.x > 195 && mousePosition.x < 305 && mousePosition.y > 505 && mousePosition.y < 535)
					{
						sf::Packet send;
						send << Code::DESCARTAR;
						socketStatus =socket.send(send);
						if (socketStatus == sf::TcpSocket::Status::Disconnected)
						{
							msn = "Server Disconnected";
							aMensajes.push_back(msn);
							if (aMensajes.size() > 9)
							{
								aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
							}
						}
						turno = false;
					}
					//IGUALAR APUESTA
					else if (mousePosition.x > 195 && mousePosition.x < 305 && mousePosition.y > 465 && mousePosition.y < 495)
					{
						//TO DO
						sf::Packet send;
						send << Code::IGUALAR;
						socketStatus = socket.send(send);
						if (socketStatus == sf::TcpSocket::Status::Disconnected)
						{
							msn = "Server Disconnected";
							aMensajes.push_back(msn);
							if (aMensajes.size() > 9)
							{
								aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
							}
						}

						turno = false;
					}
					//SUBIR APUESTA
					else if (mousePosition.x > 195 && mousePosition.x < 305 && mousePosition.y > 425 && mousePosition.y < 455)
					{
						//TO DO

						sf::Packet send;
						send << Code::SUBIR;
						socketStatus = socket.send(send);
						if (socketStatus == sf::TcpSocket::Status::Disconnected)
						{
							msn = "Server Disconnected";
							aMensajes.push_back(msn);
							if (aMensajes.size() > 9)
							{
								aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
							}
						}
						turno = false;
					}
				}
				break;
			case sf::Event::TextEntered:
				if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
					mensaje += (char)evento.text.unicode;
				else if (evento.text.unicode == 8 && mensaje.getSize() > 0)
					mensaje.erase(mensaje.getSize() - 1, mensaje.getSize());
				break;
			}
		}
		

		window.draw(separator);
		for (size_t i = 0; i < aMensajes.size(); i++)
		{
			std::string chatting = aMensajes[i];
			chattingText.setPosition(sf::Vector2f(0, (20 * i)+550));
			chattingText.setString(chatting);
			window.draw(chattingText);
		}
		std::string mensaje_ = mensaje + "_";
		text.setString(mensaje_);
		window.draw(text);

		//PINTAMOS EL JUEGO
		Carta c(numeroCarta1, pCarta1, sCarta1);
		Carta c2(numeroCarta2, pCarta2, sCarta2);
		game.listPlayers[0].carta1 = c;
		game.listPlayers[0].carta2 = c2;

		Carta ct(numeroCarta1, pCarta1, tablero[0]);
		game.cartasTablero[0] = ct;

		Carta ct2(numeroCarta1, pCarta1, tablero[1]);
		game.cartasTablero[1] = ct2;

		Carta ct3(numeroCarta1, pCarta1, tablero[2]);
		game.cartasTablero[2] = ct3;

		Carta ct4(numeroCarta1, pCarta1, tablero[3]);
		game.cartasTablero[3] = ct4;

		Carta ct5(numeroCarta1, pCarta1, tablero[4]);
		game.cartasTablero[4] = ct5;

		for (int i = 0; i<5; i++)
		{
			game.cartasTablero[i].img.setPosition(sf::Vector2f(200 + (80 * i), 220));
		}
		game.DrawScene(&window,turno);
		

		window.display();
		window.clear();
	}
	

	socket.disconnect();
	t.join();

}