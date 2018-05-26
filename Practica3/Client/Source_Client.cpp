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
	XMOVE, YMOVE, HELLO, WELLCOME, XPLAYER1, YPLAYER1, XPLAYER2, YPLAYER2, XPLAYER3, YPLAYER3, XPLAYER4, YPLAYER4,
	ASK, PLAYERSNAME, STARTTIME, ENDTIME, ENDGAME, ERROR_LOGIN
};
enum GameState{LOGIN,REGISTER,MENU,MACKING,PLAY,END};


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
sf::Clock timer;
sf::Font font;
GameState state = GameState::LOGIN;
bool clickLogin=false;

bool introUserName, introPassword, introEmail;
std::string tempUserName, tempPwd, tempEmail;

void DrawGame(sf::RenderWindow& window)
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
void DrawMenu(sf::RenderWindow& window)
{
	//Pintamos el fondo
	sf::RectangleShape background;
	background.setPosition(10, 10);
	background.setSize(sf::Vector2f(780,580));
	background.setFillColor(sf::Color(150, 150, 150, 255));	
	window.draw(background);

	//Titulo del juego
	sf::Text t("Party Quiz", font, 48);
	t.setPosition(40, 20);
	t.setFillColor(sf::Color(255, 255, 0));
	t.setStyle(sf::Text::Bold);
	window.draw(t);

	//Recuadro lista de partidas
	sf::RectangleShape listaPartidas;
	listaPartidas.setPosition(40, 100);
	listaPartidas.setSize(sf::Vector2f(600,460));
	listaPartidas.setFillColor(sf::Color(255, 255, 255, 255));
	window.draw(listaPartidas);

	//Partidas dentro la lista
	sf::RectangleShape partida;
	partida.setSize(sf::Vector2f(450, 50));
	partida.setFillColor(sf::Color(150, 150, 150, 255));
	for (int i = 0; i < 6; i++)
	{
		partida.setPosition(60,120+70*i);
		window.draw(partida);
		std::string text = "Game 1		1/4";
		sf::Text textPartida(text, font, 24);
		textPartida.setPosition(partida.getPosition()+sf::Vector2f(10,0));
		textPartida.setFillColor(sf::Color(255, 255, 0));
		window.draw(textPartida);
		text = "Join";
		textPartida.setString(text);
		textPartida.setPosition(partida.getPosition() + sf::Vector2f(300, 0));
		window.draw(textPartida);
		
	}

	//Boton crear partida
	sf::RectangleShape nuevaPartida;
	nuevaPartida.setPosition(670, 250);
	nuevaPartida.setSize(sf::Vector2f(100, 50));
	nuevaPartida.setFillColor(sf::Color(50, 255, 50, 255));
	window.draw(nuevaPartida);

	sf::Text textNew("New", font, 28);
	textNew.setPosition(nuevaPartida.getPosition()+sf::Vector2f(20,5));
	textNew.setFillColor(sf::Color::Black);
	window.draw(textNew);

	//Boton Salir
	sf::RectangleShape salirPartida;
	salirPartida.setPosition(670, 320);
	salirPartida.setSize(sf::Vector2f(100, 50));
	salirPartida.setFillColor(sf::Color(50, 50, 255, 255));
	window.draw(salirPartida);

	sf::Text textExit("Exit", font, 28);
	textExit.setPosition(salirPartida.getPosition() + sf::Vector2f(20, 5));
	textExit.setFillColor(sf::Color::Black);
	window.draw(textExit);

}
void DrawLogin(sf::RenderWindow& window)
{
	//Pintamos el fondo
	sf::RectangleShape background;
	background.setPosition(10, 10);
	background.setSize(sf::Vector2f(780, 580));
	background.setFillColor(sf::Color(150, 150, 150, 255));
	window.draw(background);

	//Login
	sf::Text t("Login", font, 48);
	t.setPosition(350, 20);
	t.setFillColor(sf::Color(255, 255, 0));
	t.setStyle(sf::Text::Bold);
	window.draw(t);

	//Usuario
	t.setString("User:");
	t.setCharacterSize(24);
	t.setPosition(300, 150);
	t.setFillColor(sf::Color(0, 0, 0));
	t.setStyle(sf::Text::Bold);
	window.draw(t);

	sf::RectangleShape usuario;
	usuario.setPosition(t.getPosition()+sf::Vector2f(80,-5));
	usuario.setSize(sf::Vector2f(200, 50));
	usuario.setFillColor(sf::Color(255, 255, 255, 255));
	window.draw(usuario);

	t.setString(tempUserName);
	t.setPosition(usuario.getPosition()+sf::Vector2f(10,0));
	t.setFillColor(sf::Color(0, 0, 0));
	t.setStyle(sf::Text::Bold);
	window.draw(t);

	//Contraseña
	t.setString("Password:");
	t.setCharacterSize(24);
	t.setPosition(250, 250);
	t.setFillColor(sf::Color(0, 0, 0));
	t.setStyle(sf::Text::Bold);
	window.draw(t);

	sf::RectangleShape pwd;
	pwd.setPosition(t.getPosition() + sf::Vector2f(130, -5));
	pwd.setSize(sf::Vector2f(200, 50));
	pwd.setFillColor(sf::Color(255, 255, 255, 255));
	window.draw(pwd);

	t.setString(tempPwd);
	t.setPosition(pwd.getPosition() + sf::Vector2f(10, 0));
	t.setFillColor(sf::Color(0, 0, 0));
	t.setStyle(sf::Text::Bold);
	window.draw(t);

	//NewUser
	sf::RectangleShape login;
	login.setPosition(pwd.getPosition() + sf::Vector2f(-40, 100));
	login.setSize(sf::Vector2f(100, 50));
	login.setFillColor(sf::Color(255, 255, 255, 255));
	window.draw(login);

	t.setString("Login");
	t.setPosition(login.getPosition() + sf::Vector2f(10, 10));
	t.setFillColor(sf::Color(0, 0, 0));
	t.setStyle(sf::Text::Bold);
	window.draw(t);

	//Log In
	sf::RectangleShape newUser;
	newUser.setPosition(login.getPosition() + sf::Vector2f(0, 100));
	newUser.setSize(sf::Vector2f(150, 50));
	newUser.setFillColor(sf::Color(255, 255, 255, 255));
	window.draw(newUser);

	t.setString("New User");
	t.setPosition(newUser.getPosition() + sf::Vector2f(10, 10));
	t.setFillColor(sf::Color(0, 0, 0));
	t.setStyle(sf::Text::Bold);
	window.draw(t);
}
void Login()
{
	sf::Packet pck;
	std::string name;
	
	if (clickLogin)
	{
		pck << Code::HELLO << tempUserName<<tempPwd;
		sock.send(pck, IP_SERVER, PORT_SERVER);
	}
}

void DibujaSFML()
{

	sf::RenderWindow window(sf::VideoMode(800, 600), "Party - Quiz Game");
	if (!font.loadFromFile("Roboto-Bold.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	sf::Packet pck;
	
	int mouseX, mouseY;
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
				switch (state)
				{
				case REGISTER:
					break;
				case MENU:
					break;
				case MACKING:
					break;
				case PLAY:
					if (event.key.code == sf::Keyboard::Left)
					{
						sf::Packet pckLeft;
						localPlayer->x -= speed;
						pckLeft << 0 << localPlayer->x;
						sock.send(pckLeft, IP_SERVER, PORT_SERVER);

					}
					else if (event.key.code == sf::Keyboard::Right)
					{
						sf::Packet pckRight;
						localPlayer->x += speed;
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
				case END:
					break;
				default:
					break;
				}
				break;
			case sf::Event::TextEntered:
				if (introUserName)
				{
					if (event.text.unicode >= 32 && event.text.unicode <= 126 && tempUserName.size()<10)
						tempUserName += (char)event.text.unicode;
					else if (event.text.unicode == 8 && tempUserName.size() > 0)
						tempUserName.erase(tempUserName.size() - 1, tempUserName.size());
				}
				else if (introPassword)
				{
					if (event.text.unicode >= 32 && event.text.unicode <= 126 && tempPwd.size()<10)
						tempPwd += (char)event.text.unicode;
					else if (event.text.unicode == 8 && tempPwd.size() > 0)
						tempPwd.erase(tempPwd.size() - 1, tempPwd.size());
				}
				break;
			case sf::Event::MouseButtonPressed:
				mouseX = sf::Mouse::getPosition(window).x;
				mouseY = sf::Mouse::getPosition(window).y;

				if (state == GameState::LOGIN)
				{
					//User Name Text
					if (mouseX > 380 && mouseX < 580 && mouseY>150 && mouseY < 190)
					{
						introUserName = true;
						introPassword = false;
						introEmail = false;
					}

					//Password Text
					else if (mouseX > 380 && mouseX < 580 && mouseY>250 && mouseY < 300)
					{
						introUserName = false;
						introPassword = true;
						introEmail = false;
					}

					//Login Button
					else if (mouseX > 340 && mouseX < 440 && mouseY>350 && mouseY < 390)
					{
						introUserName = false;
						introPassword = false;
						introEmail = false;
						clickLogin = true;
					}
				}
			default:
				break;

			}
		}

		if (timer.getElapsedTime().asSeconds() >= 1)
		{
			currentTime--;
			timer.restart();
		}

		//Mensaje recibido
		sf::Socket::Status status = sock.receive(pck, ipRem, portRem);
		if (status == sf::Socket::Done)
		{
			int id,pos;
			std::string name;

			pck >> id;

			switch (id)
			{
			case WELLCOME:
				pck >> name;
				switch (id)
				{
				case 1:
					localPlayer = &p1;
					localPlayer->name = name;
					break;
				case 2:
					localPlayer = &p2;
					localPlayer->name = name;
					break;
				case 3:
					localPlayer = &p3;
					localPlayer->name = name;
					break;
				case 4:
					localPlayer = &p4;
					localPlayer->name = name;
					break;
				default:
					break;
				}
				clickLogin = false;
				state = GameState::MENU;
				break;
			case ERROR_LOGIN:
				tempUserName = "";
				tempPwd = "";
				clickLogin = false;
				break;
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
				p1.x = pos;
				break;
			case YPLAYER1:
				pck >> pos;
				p1.y = pos;
				break;
			case XPLAYER2:
				pck >> pos;
				p2.x = pos;
				break;
			case YPLAYER2:
				pck >> pos;
				p2.y = pos;
				break;
			case XPLAYER3:
				pck >> pos;
				p3.x = pos;
				break;
			case YPLAYER3:
				pck >> pos;
				p3.y = pos;
				break;
			case XPLAYER4:
				pck >> pos;
				p4.x = pos;
				break;
			case YPLAYER4:
				pck >> pos;
				p4.y = pos;
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
				state = GameState::END;
				pck >> pregunta;
				a = "";
				b = a;
				c = a;
				d = a;
				break;
			default:
				break;
			}
		}

		window.clear();

		//Mientras esta en partida
		sf::Text t(pregunta, font, 48);
		switch (state)
		{
		case LOGIN:
			Login();
			DrawLogin(window);
			break;
		case REGISTER:
			break;
		case MENU:
			DrawMenu(window);
			break;
		case MACKING:
			break;
		case PLAY:
			DrawGame(window);
			break;
		case END:
			t.setFillColor(sf::Color(255, 255, 255));
			t.setStyle(sf::Text::Bold);
			t.setPosition(120, 230);
			window.draw(t);
			break;
		default:
			break;
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

int main()
{
	StartPlayers();
	StartScene();
	sock.setBlocking(false);
	DibujaSFML();
	return 0;
}