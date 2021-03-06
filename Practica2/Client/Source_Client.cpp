#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

#define IP_SERVER "localhost"
#define PORT_SERVER 50000

struct Player
{
	std::string name;
	int x, previusX,futurX=0,nextX=0;
	int y, previusY,futurY=0,nextY=0;
	int size;
	int points;
	sf::RectangleShape rect,rectPoints;
	bool localPlayer;
};

enum Code
{
	HELLO, WELLCOME, ASK, PLAYERSNAME, STARTTIME, ENDTIME, ENDGAME, AFK,
	DISCONNECTED, ACOMOVE, ACK_ACOMOVE, PLAYER1, PLAYER2, PLAYER3, PLAYER4
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

int currentTime;
sf::Clock timer,helloTimer,hello,movimientoTimer;
bool endGame = false, resposta = false;


std::vector<int>acumulacionHello;

void SendHello()
{
	sf::Packet send;
	send << HELLO<<(int)acumulacionHello.size();
	int r = rand() % 100 + 1;
	if (r > 20)	//20% de perder el paquete
	{
		sock.send(send, IP_SERVER, PORT_SERVER);
	}
	else
	{
		std::cout << "LOST PACKET" << std::endl;
	}
	acumulacionHello.push_back(acumulacionHello.size());
}

void DibujaSFML()
{
	srand(time(NULL));
	sf::Packet pck;
	std::cout << "Username:	";
	std::string name;
	std::cin >> name;
	pck << Code::HELLO << name;
	sock.send(pck, IP_SERVER, PORT_SERVER);
	bool respostaServidor = false;
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

				localPlayer->localPlayer = true;
			}
		}
	}

	sf::RenderWindow window(sf::VideoMode(800, 600), "Party - Quiz Game");
	sf::Font font;
	if (!font.loadFromFile("Roboto-Bold.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	std::vector<int>acumulacionX,acumulacionY;
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
					localPlayer->x -= speed;
					acumulacionX.push_back(-speed);

				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					localPlayer->x+=speed;
					acumulacionX.push_back(speed);
				}
				else if (event.key.code == sf::Keyboard::Up)
				{
					localPlayer->y -= speed;
					acumulacionY.push_back(-speed);;
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					localPlayer->y += speed;
					acumulacionY.push_back(speed);
				}
				break;

			default:
				break;

			}
		}
		//Si estamos en partida
		if (!endGame)
		{
			if (!p1.localPlayer)
			{
				if (p1.futurX != p1.nextX)
				{
					p1.x = p1.nextX;
					if (p1.futurX - p1.nextX > 0)
					{
						p1.nextX += speed;
					}
					else
						p1.nextX -= speed;
				}
				if (p1.futurY != p1.nextY)
				{
					p1.y = p1.nextY;
					if (p1.futurY - p1.nextY > 0)
					{
						p1.nextY += speed;
					}
					else
						p1.nextY -= speed;
				}
			}
			if (!p2.localPlayer)
			{
				if (p2.futurX != p2.nextX)
				{
					p2.x = p2.nextX;
					if (p2.futurX - p2.nextX > 0)
					{
						p2.nextX += speed;
					}
					else
						p2.nextX -= speed;
				}
				if (p2.futurY != p2.nextY)
				{
					p2.y = p2.nextY;
					if (p2.futurY - p2.nextY > 0)
					{
						p2.nextY += speed;
					}
					else
						p2.nextY -= speed;
				}
			}
			if (!p3.localPlayer)
			{
				if (p3.futurX != p3.nextX)
				{
					p3.x = p3.nextX;
					if (p3.futurX - p3.nextX > 0)
					{
						p3.nextX += speed;
					}
					else
						p3.nextX -= speed;
				}
				if (p3.futurY != p3.nextY)
				{
					p3.y = p3.nextY;
					if (p3.futurY - p3.nextY > 0)
					{
						p3.nextY += speed;
					}
					else
						p3.nextY -= speed;
				}
			}
			if (!p4.localPlayer)
			{
				if (p4.futurX != p4.nextX)
				{
					p4.x = p4.nextX;
					if (p4.futurX - p4.nextX > 0)
					{
						p4.nextX += speed;
					}
					else
						p4.nextX -= speed;
				}
				if (p4.futurY != p4.nextY)
				{
					p4.y = p4.nextY;
					if (p4.futurY - p4.nextY > 0)
					{
						p4.nextY += speed;
					}
					else
						p4.nextY -= speed;
				}
			}
			//Mandar Acumulacion de movimentos
			if (acumulacionX.size() + acumulacionY.size() >= 5)
			{
				sf::Packet pckLeft;
				int x = 0, y = 0;
				for (int a : acumulacionX)
				{
					x += a;
				}

				for (int a : acumulacionY)
				{
					y += a;
				}
				pckLeft << ACOMOVE << x << y;
				sock.send(pckLeft, IP_SERVER, PORT_SERVER);
				acumulacionX.clear();
				acumulacionY.clear();
				movimientoTimer.restart();
			}

			//Actualizar tiempo
			if (timer.getElapsedTime().asSeconds() >= 1)
			{
				currentTime--;
				timer.restart();
			}

			//Mandar hello y sus acumulaciones
			if (helloTimer.getElapsedTime().asMilliseconds() >= 2500)
			{
				sf::Packet send;
				//Afegim un hello nou
				acumulacionHello.push_back(acumulacionHello.size());

				//Indiquem la mida
				send << HELLO << (int)acumulacionHello.size();

				//Enviem tots els hellos no rebuts
				for (int i = 0; i < acumulacionHello.size(); i++)
				{
					send << acumulacionHello[i];
				}

				int r = rand() % 100 + 1;
				if (r > 20)	//20% de perder el paquete
				{
					sock.send(send, IP_SERVER, PORT_SERVER);
				}
				else
				{
					std::cout << "LOST PACKET" << std::endl;
				}
				helloTimer.restart();
			}
		}

		sf::Socket::Status status = sock.receive(pck, ipRem, portRem);
		if (status == sf::Socket::Done)
		{
			int id,pos,aux;
			std::vector<int>hellosDelete;
			pck >> id;
			switch (id)
			{
			case -1:
				localPlayer->x = localPlayer->previusX;
				localPlayer->y = localPlayer->previusY;
				break;
			case ASK:
				pck >> pregunta>>a>>b>>c>>d;
				break;
			case PLAYERSNAME:
				pck >> p1.name >> p2.name >> p3.name >> p4.name;
				break;
			case STARTTIME:
				pck >> currentTime;
				break;
			case ENDTIME:
				pck >> p1.points>>p2.points>>p3.points>>p4.points;
				break;
			case ENDGAME:
				endGame = true;
				pck >> pregunta;
				a = "";
				b = a;
				c = a;
				d = a;
				break;

			case AFK:
				pck >> pos;
				for (int i = 0; i < pos; i++)
				{
					pck >> id;
					switch (id)
					{
					case 0:
						p1.name = "-------";
						p1.points = 0;
						break;
					case 1:
						p2.name = "-------";
						p2.points = 0;
						break;
					case 2:
						p3.name = "-------";
						p3.points = 0;
						break;
					case 3:
						p4.name = "-------";
						p4.points = 0;
						break;
					default:
						break;
					}
				}
				break;

			case WELLCOME:
				//Obtengo el id del paquete
				pck >> pos;
				hellosDelete.clear();
				id = -1;
				//Busco el indice con este paquete
				for (int j = 0; j < pos; j++)
				{
					pck >> id;
					aux = -1;
					for (int i = 0; i <acumulacionHello.size(); i++)
					{
						if (acumulacionHello[i] == id)aux=i;
					}
					if(aux!=-1)acumulacionHello.erase(acumulacionHello.begin() + aux);
				}
				break;
			case ACK_ACOMOVE:
				pck>>localPlayer->previusX;
				pck>>localPlayer->previusY;

			case PLAYER1:
				pck >> p1.futurX;
				pck >> p1.futurY;
				if (p1.futurX - p1.nextX > 0)
				{
					p1.nextX = p1.x+ speed;
				}
				else
					p1.nextX = p1.x - speed;

				if (p1.futurY - p1.nextY > 0)
				{
					p1.nextY = p1.y + speed;
				}
				else
					p1.nextY = p1.y - speed;
				break;
			case PLAYER2:
				pck >> p2.futurX;
				pck >> p2.futurY;
				if (p2.futurX - p2.nextX > 0)
				{
					p2.nextX = p2.x + speed;
				}
				else
					p2.nextX = p2.x - speed;

				if (p2.futurY - p2.nextY > 0)
				{
					p2.nextY = p2.y + speed;
				}
				else
					p2.nextY = p2.y - speed;
				break;
			case PLAYER3:
				pck >> p3.futurX;
				pck >> p3.futurY;
				if (p3.futurX - p3.nextX > 0)
				{
					p3.nextX = p3.x + speed;
				}
				else
					p3.nextX = p3.x - speed;

				if (p3.futurY - p3.nextY > 0)
				{
					p3.nextY = p3.y + speed;
				}
				else
					p3.nextY = p3.y - speed;
				break;
			case PLAYER4:
				pck >> p4.futurX;
				pck >> p4.futurY;
				if (p4.futurX - p4.nextX > 0)
				{
					p4.nextX = p4.x + speed;
				}
				else
					p4.nextX = p4.x - speed;

				if (p4.futurY - p4.nextY > 0)
				{
					p4.nextY = p4.y + speed;
				}
				else
					p4.nextY = p4.y - speed;
				break;
			case DISCONNECTED:
				window.close();
				break;
			default:
				break;

			}
		}

		window.clear();
		//Mientras esta en partida
		if (!endGame)
		{
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

			t.setString("Time: " + std::to_string(currentTime));
			t.setPosition(350, 150);
			window.draw(t);

			//PLAYER 1
			p1.rect.setPosition(sf::Vector2f(p1.x, p1.y));
			window.draw(p1.rect);
			window.draw(p1.rectPoints);

			t.setString(p1.name + ":	" + std::to_string(p1.points));
			t.setFillColor(sf::Color(0, 0, 0));
			t.setPosition(20, 540);
			window.draw(t);


			//PLAYER 2
			p2.rect.setPosition(sf::Vector2f(p2.x, p2.y));
			window.draw(p2.rect);
			window.draw(p2.rectPoints);

			t.setFillColor(sf::Color(255, 255, 255));
			t.setString(p2.name + ":	" + std::to_string(p2.points));
			t.setPosition(220, 540);
			window.draw(t);

			//PLAYER 3
			p3.rect.setPosition(sf::Vector2f(p3.x, p3.y));
			window.draw(p3.rect),
				window.draw(p3.rectPoints);

			t.setString(p3.name + ":	" + std::to_string(p3.points));
			t.setPosition(420, 540);
			window.draw(t);

			//PLAYER 4
			p4.rect.setPosition(sf::Vector2f(p4.x, p4.y));
			window.draw(p4.rect);
			window.draw(p4.rectPoints);

			t.setFillColor(sf::Color(0, 0, 0));
			t.setString(p4.name + ":	" + std::to_string(p4.points));
			t.setPosition(620, 540);
			window.draw(t);
		}

		//Cuando acaba
		else
		{
			sf::Text t(pregunta, font, 48);
			t.setFillColor(sf::Color(255, 255, 255));
			t.setStyle(sf::Text::Bold);
			t.setPosition(120, 230);
			window.draw(t);
		}

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

void CloseGame()
{
	sf::Packet pck;
	pck << DISCONNECTED;
	sock.send(pck, IP_SERVER, PORT_SERVER);
}

int main()
{
	StartPlayers();
	StartScene();
	sock.setBlocking(false);
	DibujaSFML();
	CloseGame();
	return 0;
}