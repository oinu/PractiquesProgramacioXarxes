#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>
#include <vector>

enum Palos{ DIAMANTE, TREBOL, PICA, CORAZON, OCULTO };

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
	Carta carta1;
	Carta carta2;
	int efectivo;
	int cantidadApostada;

	Player::Player()
	{
		//Comprovar que se llaman al constructor de carta
		efectivo = 1000;
		cantidadApostada = 0;
	}

	Player::Player(Carta c1, Carta c2)
	{
		carta1 = c1;
		carta2 = c2;
		efectivo = 1000;
		cantidadApostada = 0;
	}
};
class Game
{
public:
	std::vector<Player*> players;
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
	void DrawScene(sf::RenderWindow* window);
	void InitClient(sf::Font aFont);
};

