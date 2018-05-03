#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

#define IP_SERVER "localhost"
#define PORT_SERVER 50000

struct Player
{
	std::string name;
	int x, previusX;
	int y, previusY;
	int size;
	int points;
	sf::RectangleShape rect,rectPoints;
};
enum Code
{
	XMOVE, YMOVE, HELLO, WELLCOME, XPLAYER1, YPLAYER1, XPLAYER2, YPLAYER2, XPLAYER3, YPLAYER3, XPLAYER4, YPLAYER4, ASK, AOPTION, BOPTION, COPTION, DOPTION
};


Player p1,p2,p3,p4;
Player *localPlayer;
int speed = 5;

sf::RectangleShape aOption,bOption, cOption, dOption;
sf::Color aColor(100, 100, 100, 255);
sf::Color bColor(150, 150, 150, 255);

std::string pregunta,a,b,c,d;

sf::UdpSocket sock;
sf::IpAddress ipRem;
unsigned short portRem;

void DibujaSFML()
{

	sf::RenderWindow window(sf::VideoMode(800, 600), "Party - Quiz Game");
	sf::Font font;
	if (!font.loadFromFile("Roboto-Bold.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	sf::Packet pck;
	std::cout << "Username:	";
	std::string name;
	std::cin >> name;
	pck << Code::HELLO << name;
	sock.send(pck, IP_SERVER, PORT_SERVER);
	bool respostaServidor=false;
	while (!respostaServidor)
	{
		sf::Socket::Status status = sock.receive(pck, ipRem, portRem);
		if (status == sf::Socket::Done)
		{
			int id, num;
			pck >> id >> num;
			if (id == WELLCOME)
			{
				switch (num)
				{
				case 1:
					localPlayer = &p1;
					respostaServidor = true;
					localPlayer->name = name;
					break;
				case 2:
					localPlayer = &p2;
					respostaServidor = true;
					localPlayer->name = name;
					break;
				case 3:
					localPlayer = &p3;
					respostaServidor = true;
					localPlayer->name = name;
					break;
				case 4:
					localPlayer = &p4;
					respostaServidor = true;
					localPlayer->name = name;
					break;
				default:
					break;
				}
			}
		}
	}

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
					localPlayer->x -= speed;
					pckLeft <<0<< localPlayer->x;
					sock.send(pckLeft, IP_SERVER, PORT_SERVER);

				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					sf::Packet pckRight;
					localPlayer->x+=speed;
					pckRight << 0 << localPlayer->x;
					sock.send(pckRight, IP_SERVER, PORT_SERVER);
				}
				else if (event.key.code == sf::Keyboard::Up)
				{
					sf::Packet pckUp;
					localPlayer->y -= speed;
					pckUp << 1 << localPlayer->y;
					sock.send(pckUp, IP_SERVER, PORT_SERVER);
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					sf::Packet pckDown;
					localPlayer->y += speed;
					pckDown << 1 << localPlayer->y;
					sock.send(pckDown, IP_SERVER, PORT_SERVER);
				}
				break;

			default:
				break;

			}
		}
		
		sf::Socket::Status status = sock.receive(pck, ipRem, portRem);
		if (status == sf::Socket::Done)
		{
			int id,pos;
			pck >> id;
			switch (id)
			{
			case -1:
				localPlayer->x = localPlayer->previusX;
				localPlayer->y = localPlayer->previusY;
				break;
			case XMOVE:
				pck >> pos;
				localPlayer->previusX = pos;
				break;
			case YMOVE:
				pck >> pos;
				localPlayer->previusY = pos;
				break;
			case XPLAYER1:
				pck >> pos;
				std::cout << "xPlayer1" << std::endl;
				p1.x = pos;
				break;
			case YPLAYER1:
				pck >> pos;
				std::cout << "yPlayer1" << std::endl;
				p1.y = pos;
				break;
			case XPLAYER2:
				pck >> pos;
				std::cout << "xPlayer2" << std::endl;
				p2.x = pos;
				break;
			case YPLAYER2:
				pck >> pos;
				std::cout << "yPlayer2" << std::endl;
				p2.y = pos;
				break;
			case XPLAYER3:
				pck >> pos;
				std::cout << "xPlayer3" << std::endl;
				p3.x = pos;
				break;
			case YPLAYER3:
				pck >> pos;
				std::cout << "yPlayer3" << std::endl;
				p3.y = pos;
				break;
			case XPLAYER4:
				pck >> pos;
				std::cout << "xPlayer4" << std::endl;
				p4.x = pos;
				break;
			case YPLAYER4:
				pck >> pos;
				std::cout << "yPlayer4" << std::endl;
				p4.y = pos;
				break;
			case ASK:
				pck >> pregunta;
				break;
			case AOPTION:
				pck >> a;
				break;
			case BOPTION:
				pck >> b;
				break;
			case COPTION:
				pck >> c;
				break;
			case DOPTION:
				pck >> d;
				break;
			default:
				break;
			}
		}

		window.clear();
		//DRAW THE OPTIONS AND QUESTION
		sf::Text t(a, font, 24);
		t.setFillColor(sf::Color(255, 255, 0));
		t.setStyle(sf::Text::Bold);
		window.draw(aOption);
		t.setPosition(20, 230);
		window.draw(t);

		window.draw(bOption);
		t.setString(b);
		t.setPosition(420, 230);
		window.draw(t);

		window.draw(cOption);
		t.setString(c);
		t.setPosition(20, 380);
		window.draw(t);

		window.draw(dOption);
		t.setString(d);
		t.setPosition(420, 380);
		window.draw(t);

		t.setString(pregunta);
		t.setFillColor(sf::Color(255, 255, 255));
		t.setPosition(20, 50);
		window.draw(t);

		t.setString("Time: "+std::to_string(0));
		t.setPosition(350, 150);
		window.draw(t);

		//PLAYER 1
		p1.rect.setPosition(sf::Vector2f(p1.x, p1.y));
		window.draw(p1.rect);
		window.draw(p1.rectPoints);

		t.setString(std::to_string(p1.points));
		t.setFillColor(sf::Color(0, 0, 0));
		t.setPosition(150, 540);
		window.draw(t);


		//PLAYER 2
		p2.rect.setPosition(sf::Vector2f(p2.x, p2.y));
		window.draw(p2.rect);
		window.draw(p2.rectPoints);

		t.setFillColor(sf::Color(255, 255, 255));
		t.setString(std::to_string(p2.points));
		t.setPosition(350, 540);
		window.draw(t);

		//PLAYER 3
		p3.rect.setPosition(sf::Vector2f(p3.x, p3.y));
		window.draw(p3.rect),
		window.draw(p3.rectPoints);

		t.setString(std::to_string(p3.points));
		t.setPosition(550, 540);
		window.draw(t);

		//PLAYER 4
		p4.rect.setPosition(sf::Vector2f(p4.x, p4.y));
		window.draw(p4.rect);
		window.draw(p4.rectPoints);

		t.setFillColor(sf::Color(0, 0, 0));
		t.setString(std::to_string(p4.points));
		t.setPosition(750, 540);
		window.draw(t);

		window.display();
	}

}

void StartPlayers()
{
	//Init Player 1
	p1.size = 30;
	p1.x = 400 - p1.size;
	p1.previusX = p1.x;
	p1.y = 350-p1.size;
	p1.previusY = p1.y;

	p1.rect.setSize(sf::Vector2f(p1.size, p1.size));
	p1.rect.setFillColor(sf::Color::Green);

	p1.rectPoints.setSize(sf::Vector2f(200, 100));
	p1.rectPoints.setFillColor(sf::Color::Green);
	p1.rectPoints.setPosition(sf::Vector2f(0, 500));

	//Init Player 2
	p2.size = 30;
	p2.x = 430 - p2.size;
	p2.previusX = p2.x;
	p2.y = 350 - p2.size;
	p2.previusY = p2.y;

	p2.rect.setSize(sf::Vector2f(p2.size, p2.size));
	p2.rect.setFillColor(sf::Color::Blue);

	p2.rectPoints.setSize(sf::Vector2f(200, 100));
	p2.rectPoints.setFillColor(sf::Color::Blue);
	p2.rectPoints.setPosition(sf::Vector2f(200, 500));

	//Init Player 3
	p3.size = 30;
	p3.x = 400 - p3.size;
	p3.previusX = p3.x;
	p3.y = 380 - p3.size;
	p3.previusY = p3.y;

	p3.rect.setSize(sf::Vector2f(p3.size, p3.size));
	p3.rect.setFillColor(sf::Color::Red);

	p3.rectPoints.setSize(sf::Vector2f(200, 100));
	p3.rectPoints.setFillColor(sf::Color::Red);
	p3.rectPoints.setPosition(sf::Vector2f(400, 500));

	//Init Player 4
	p4.size = 30;
	p4.x = 430 - p4.size;
	p4.previusX = p4.x;
	p4.y = 380 - p4.size;
	p4.previusY = p4.y;

	p4.rect.setSize(sf::Vector2f(p4.size, p4.size));
	p4.rect.setFillColor(sf::Color::Yellow);

	p4.rectPoints.setSize(sf::Vector2f(200, 100));
	p4.rectPoints.setFillColor(sf::Color::Yellow);
	p4.rectPoints.setPosition(sf::Vector2f(600, 500));
}

void StartScene()
{
	aOption.setSize(sf::Vector2f(400, 150));
	aOption.setFillColor(aColor);
	aOption.setPosition(sf::Vector2f(0, 200));

	bOption.setSize(sf::Vector2f(400, 150));
	bOption.setFillColor(bColor);
	bOption.setPosition(sf::Vector2f(400, 200));

	cOption.setSize(sf::Vector2f(400, 150));
	cOption.setFillColor(bColor);
	cOption.setPosition(sf::Vector2f(0, 350));

	dOption.setSize(sf::Vector2f(400, 150));
	dOption.setFillColor(aColor);
	dOption.setPosition(sf::Vector2f(400, 350));
}

int main()
{
	StartPlayers();
	StartScene();
	sock.setBlocking(false);
	DibujaSFML();
	return 0;
}