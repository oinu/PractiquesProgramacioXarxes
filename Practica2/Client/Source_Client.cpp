#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

#define IP_SERVER "localhost"
#define PORT_SERVER 50000

int x,previusX;
int y, previusY;
int speed = 5;
sf::UdpSocket sock;

void DibujaSFML()
{

	sf::RenderWindow window(sf::VideoMode(800, 600), "Party - Quiz Game");

	while (window.isOpen())
	{
		sf::Event event;

		//Este primer WHILE es para controlar los eventos del mouse
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Left)
				{
					sf::Packet pckLeft;
					x = x - speed;
					pckLeft <<0<< x;
					sock.send(pckLeft, IP_SERVER, PORT_SERVER);

				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					sf::Packet pckRight;
					x = x + speed;
					pckRight << 0 << x;
					sock.send(pckRight, IP_SERVER, PORT_SERVER);
				}
				else if (event.key.code == sf::Keyboard::Up)
				{
					sf::Packet pckUp;
					y = y - speed;
					pckUp << 1 << y;
					sock.send(pckUp, IP_SERVER, PORT_SERVER);
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					sf::Packet pckDown;
					y = y + speed;
					pckDown << 1 << y;
					sock.send(pckDown, IP_SERVER, PORT_SERVER);
				}
				break;

			default:
				break;

			}
		}
		
		sf::Packet pck;
		sf::IpAddress ipRem;
		unsigned short portRem;
		sf::Socket::Status status = sock.receive(pck, ipRem, portRem);
		if (status == sf::Socket::Done)
		{
			int id,pos;
			pck >>id >> pos;
			if (id == -1)
			{
				x = previusX;
				y = previusY;
				std::cout << "No valid position" << std::endl;
			}
			else if(id==0)
			{
				previusX = pos;
			}
			else if (id == 1)
			{
				previusY = pos;
			}
		}

		window.clear();

		sf::RectangleShape rectBlanco(sf::Vector2f(1, 600));
		rectBlanco.setFillColor(sf::Color::White);
		rectBlanco.setPosition(sf::Vector2f(100, 0));
		window.draw(rectBlanco);
		rectBlanco.setPosition(sf::Vector2f(700, 0));
		window.draw(rectBlanco);

		sf::RectangleShape rectBlanco2(sf::Vector2f(800, 1));
		rectBlanco2.setFillColor(sf::Color::White);
		rectBlanco2.setPosition(sf::Vector2f(0, 200));
		window.draw(rectBlanco2);
		rectBlanco2.setPosition(sf::Vector2f(0, 500));
		window.draw(rectBlanco2);

		sf::RectangleShape rectAvatar(sf::Vector2f(30, 30));
		rectAvatar.setFillColor(sf::Color::Green);
		rectAvatar.setPosition(sf::Vector2f(x, y));
		window.draw(rectAvatar);

		


		

		window.display();
	}

}

int main()
{
	x = 200;
	previusX = x;
	y = 300;
	previusY = y;
	sock.setBlocking(false);
	DibujaSFML();
	return 0;
}