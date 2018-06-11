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
bool turno = true;
Game game;

//Escucha por un socket determinado y escribe el mensaje llegado.
//socket: Es el socket al cual debemos escuchar.
//recived: La longitud de los datos que recibiremos.
//aMensajes: Es donde contendra todos los mensajes del chat.
void RecivedFunction(sf::TcpSocket* socket,size_t* recived, vector<string>* aMensajes, sf::RenderWindow* window)
{
	while (window->isOpen())
	{
		/*char buffer[BUFFER_SIZE];
		int codigo;
		int cantidad;
		string s;
		sf::Socket::Status status = socket->receive(buffer, sizeof(buffer), *recived);
		
		if (status == sf::Socket::Status::Done)
		{
			s = buffer;
			codigo = atoi(&s[0]);
			s.erase(s.begin(), s.begin() + 1);
			if(codigo==6)cout << "Total en mesa: " + to_string(game.efectivoMesa) << endl;
			switch (codigo)
			{
			case 0:
				mu.lock();
				aMensajes->push_back(s);
				if (aMensajes->size() > 9)
				{
					aMensajes->erase(aMensajes->begin(), aMensajes->begin() + 1);
				}
				mu.unlock();
				break;
			case 4:
				//ES EL PRIMERO
				if (s.size() > 0)
				{
					mu.lock();
					aMensajes->push_back(s);
					if (aMensajes->size() > 9)
					{
						aMensajes->erase(aMensajes->begin(), aMensajes->begin() + 1);
					}
					mu.unlock();
				}
				turno = true;
				break;
			case 5:
				turno = true;
				break;
			
			//CONFIRMA SUBE APUESTA
			case 6:
				cantidad = stoi(s);
				//cout << to_string(cantidad )<< endl;
				/*game.listPlayers[0].efectivo -= cantidad;
				game.apuestaActual += 10;
				game.efectivoMesa += cantidad;
				turno = false;
				break;
			//CONFIRMA IGUALA
			case 7:
				cantidad = stoi(s);
				//cout << to_string(cantidad )<< endl;
				/*game.listPlayers[0].efectivo -= cantidad;
				game.efectivoMesa += cantidad;
				turno = false;
				break;
			//CONFIRMA DESCARTE
			case 8:
				turno = false;
				break;
			default:
				break;
			}
		}*/
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

	// Obtenemos nuestra direccion ip, y nos connectamos con el puerto indicado y nuestra ip
	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	socket.connect(ip, CLIENT_PORT);

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
				mousePosition = sf::Mouse::getPosition();
				if (turno)
				{

					//DESCARTAR MANO
					if (mousePosition.x > 760 && mousePosition.x < 880 && mousePosition.y > 680 && mousePosition.y < 715)
					{
						//TO DO

						/*msn = std::to_string(3);
						cout << "DESCARTA" << endl;
						socketStatus = socket.send(msn.c_str(), msn.size() + 1);
						turno = false;
						if (socketStatus == sf::TcpSocket::Status::Disconnected)
						{
							msn = "Server Disconnected";
							aMensajes.push_back(msn);
							if (aMensajes.size() > 9)
							{
								aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
							}
						}*/
					}
					//IGUALAR APUESTA
					else if (mousePosition.x > 760 && mousePosition.x < 880 && mousePosition.y > 640 && mousePosition.y < 675)
					{
						//TO DO

						/*msn = std::to_string(2);
						cout << "IGUALA APUESTA" << endl;
						socketStatus = socket.send(msn.c_str(), msn.size() + 1);
						
						//cambio en local
						game.listPlayers[0].efectivo -= game.apuestaActual;
						game.efectivoMesa += game.apuestaActual;
						turno = false;

						if (socketStatus == sf::TcpSocket::Status::Disconnected)
						{
							msn = "Server Disconnected";
							aMensajes.push_back(msn);
							if (aMensajes.size() > 9)
							{
								aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
							}
						}*/
					}
					//SUBIR APUESTA
					else if (mousePosition.x > 760 && mousePosition.x < 880 && mousePosition.y > 590 && mousePosition.y < 635)
					{
						//TO DO

						/*msn = std::to_string(1);
						cout << "SUBE APUESTA" << endl;
						socketStatus = socket.send(msn.c_str(), msn.size() + 1);

						//Cambiar copia en local
						game.apuestaActual += 10;
						game.listPlayers[0].efectivo -= game.apuestaActual;
						game.efectivoMesa += game.apuestaActual;
						turno = false;

						if (socketStatus == sf::TcpSocket::Status::Disconnected)
						{
							msn = "Server Disconnected";
							aMensajes.push_back(msn);
							if (aMensajes.size() > 9)
							{
								aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
							}
						}*/
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
		game.DrawScene(&window);
		

		window.display();
		window.clear();
	}
	

	socket.disconnect();
	t.join();

}