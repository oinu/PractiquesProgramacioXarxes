#include "Game.h"



Game::Game()
{
	//JUGADORES Y SUS POSICIONES EN EL TABLERO
	listPlayers = new Player[4];
	listPlayers[0].carta1.img.setPosition(sf::Vector2f(310, 420));
	listPlayers[0].carta2.img.setPosition(sf::Vector2f(390, 420));

	listPlayers[1].carta1.img.rotate(90);
	listPlayers[1].carta1.img.setPosition(130, 200);
	listPlayers[1].carta2.img.rotate(90);
	listPlayers[1].carta2.img.setPosition(130, 280);

	listPlayers[2].carta1.img.setPosition(sf::Vector2f(310, 0));
	listPlayers[2].carta2.img.setPosition(sf::Vector2f(390, 0));

	listPlayers[3].carta1.img.rotate(-90);
	listPlayers[3].carta1.img.setPosition(670, 300);
	listPlayers[3].carta2.img.rotate(-90);
	listPlayers[3].carta2.img.setPosition(670, 380);

	//CARTAS EN EL TABLERO
	cartasTablero = new Carta[5];
	for (int i = 0; i < 5; i++)
	{
		cartasTablero[i].img.setPosition(sf::Vector2f(200+(80*i), 220));
	}
}


Game::~Game()
{
}

void Game::DrawScene(sf::RenderWindow* window)
{
	//PINTAR BACKGROUND
	window->draw(tablero);

	//PINTAR JUGADORES
	for (int i = 0; i < 4; i++)
	{
		window->draw(listPlayers[i].carta1.img);
		window->draw(listPlayers[i].carta2.img);
	}

	//PINTAR TABLERO
	for (int i = 0; i < 5; i++)
	{
		window->draw(cartasTablero[i].img);
	}

	//PINTAR TEXTO TOTAL MESA
	total = "TOTAL EN MESA: "+ std::to_string(efectivoMesa)+"	APUESTA ACTUAL: " + std::to_string(apuestaActual);
	txt->setString(total);
	txt->setPosition(140, 150);
	window->draw(*txt);

	//PINTAR TEXTO EFECTIVO JUGADOR
	total = "EFECTIVO: " + std::to_string(listPlayers[0].efectivo);
	txt->setString(total);
	txt->setPosition(480, 500);
	window->draw(*txt);
	
}

void Game::InitClient(sf::Font aFont)
{
	font = aFont;
	txt = new sf::Text(total, font, 24);
	txt->setFillColor(sf::Color(255, 255, 255));
	txt->setStyle(sf::Text::Bold);
	txt->setPosition(300, 150);

	if (!texture.loadFromFile("./img/Tauler.png"))
	{
		std::cout << "Error cargar imagen" << std::endl;
	}
	tablero = (sf::Sprite)texture;
	tablero.scale(sf::Vector2f(1, 0.915));
}
