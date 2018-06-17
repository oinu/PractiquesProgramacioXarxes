#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>
#include <vector>

enum Palos{ DIAMANTE, TREBOL, PICA, CORAZON, OCULTO };
enum Code{MENSAJE,DESCARTAR,IGUALAR,SUBIR,TURNO,COINS,HELLO,CARTAS, TABLERO,NEW_GAME};

struct Carta
{
	int numero;
	Palos palo;
	sf::Sprite img;
	sf::Texture texture;

	Carta::Carta()
	{
		numero = 0;
		palo = Palos::OCULTO;
		if (!texture.loadFromFile("./img/Reverso.png"))
		{
			std::cout << "Error cargar imagen" << std::endl;
		}
		img= (sf::Sprite)texture;
		img.scale(sf::Vector2f(0.16, 0.16));
		
	}
	Carta::Carta(int num, Palos p, std::string s)
	{
		numero = num;
		palo = p;
		if (!texture.loadFromFile(s))
		{
			std::cout << "Error cargar imagen" << std::endl;
		}
		img = (sf::Sprite)texture;
		img.scale(sf::Vector2f(0.16, 0.16));

	}

	Carta::Carta(int aNum, Palos aPalo,sf::Texture aTexture, sf::Sprite aImg)
	{
		numero = aNum;
		palo = aPalo;
		texture = aTexture;
		img = aImg;
	}

};

struct Player
{
	std::string name;
	Carta carta1;
	Carta carta2;
	int efectivo;
	int cantidadApostada;
	bool passa;

	Player::Player()
	{
		//Comprovar que se llaman al constructor de carta
		efectivo = 1000;
		cantidadApostada = 0;
		passa = false;
	}

	Player::Player(Carta c1, Carta c2)
	{
		carta1 = c1;
		carta2 = c2;
		efectivo = 1000;
		cantidadApostada = 0;
		passa = false;
	}
};

class Game
{
public:
	std::vector<Player*> players;
	std::vector<Carta>Mazo;
	Player* listPlayers ;
	Carta* cartasTablero;
	sf::Texture texture;
	sf::Sprite tablero;
	sf::String total;
	sf::Font font;
	sf::Text* txt;

	sf::Texture raiseButtonText;
	sf::Sprite raiseButton;
	sf::Texture callButtonText;
	sf::Sprite callButton;
	sf::Texture foldbuttonText;
	sf::Sprite foldButton;

	int efectivoMesa;
	int apuestaActual;
	Game();
	~Game();
	void InitServer();
	void DrawScene(sf::RenderWindow* window,bool turno);
	void InitClient(sf::Font aFont);
	Carta DarCarta();
};

