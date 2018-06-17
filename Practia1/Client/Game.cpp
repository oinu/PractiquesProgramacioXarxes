#include "Game.h"



Game::Game()
{
	srand(time(NULL));
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

	apuestaActual = 10;
}


Game::~Game()
{
}

void Game::InitServer()
{
	//Creamos el mazo ordenado
	std::vector<Carta>ordenado;
	for (int i = 0; i<4; i++)
	{
		for (int j = 1; j < 14; j++)
		{
			sf::Texture t;
			std::string s;
			sf::Sprite img;
			img.scale(sf::Vector2f(0.16, 0.16));
			//Diamantes
			if (i == 0)
			{
				if (j == 1)
				{
					s = "./img/A_D.png";
				}
				else if (j == 11)
				{
					s = "./img/J_D.png";
				}
				else if (j == 12)
				{
					s = "./img/Q_D.png";
				}
				else if (j == 13)
				{
					s = "./img/K_D.png";
				}
				else
				{
					s = "./img/" + std::to_string(j) + "_D.png";
				}
			}
			//TREBOLS
			if (i == 1)
			{
				if (j == 1)
				{
					s = "./img/A_T.png";
				}
				else if (j == 11)
				{
					s = "./img/J_T.png";
				}
				else if (j == 12)
				{
					s = "./img/Q_T.png";
				}
				else if (j == 13)
				{
					s = "./img/K_T.png";
				}
				else
				{
					s = "./img/" + std::to_string(j) + "_T.png";
				}
			}
			//PICAS
			if (i == 2)
			{
				if (j == 1)
				{
					s = "./img/A_A.png";
				}
				else if (j == 11)
				{
					s = "./img/J_A.png";
				}
				else if (j == 12)
				{
					s = "./img/Q_A.png";
				}
				else if (j == 13)
				{
					s = "./img/K_A.png";
				}
				else
				{
					s = "./img/" + std::to_string(j) + "_A.png";
				}
			}
			//CORAZONES
			if (i == 3)
			{
				if (j == 1)
				{
					s = "./img/A_H.png";
				}
				else if (j == 11)
				{
					s = "./img/J_H.png";
				}
				else if (j == 12)
				{
					s = "./img/Q_H.png";
				}
				else if (j == 13)
				{
					s = "./img/K_H.png";
				}
				else
				{
					s = "./img/" + std::to_string(j) + "_H.png";
				}
			}

			if (!t.loadFromFile(s))
			{
				std::cout << "Error cargar imagen" << std::endl;
			}
			Carta c(j, (Palos)i, t, img);
			ordenado.push_back(c);
		}
	}

	int size = ordenado.size();
	for (int i = 0; i < size; i++)
	{
		int r = rand() % ordenado.size();
		Mazo.push_back(ordenado[r]);
		ordenado.erase(ordenado.begin() + r);
	}
}

void Game::DrawScene(sf::RenderWindow* window,bool turno)
{
	//PINTAR BACKGROUND
	window->draw(tablero);

	//PINTAR JUGADORES
	listPlayers[0].carta1.img.setPosition(sf::Vector2f(310, 420));
	listPlayers[0].carta2.img.setPosition(sf::Vector2f(390, 420));

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

	//PINTAR BOTONES
	if (turno)
	{
		window->draw(raiseButton);
		window->draw(callButton);
		window->draw(foldButton);
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

	if (!raiseButtonText.loadFromFile("./img/Raise.png"))
	{
		std::cout << "Error cargar imagen" << std::endl;
	}
	raiseButton = (sf::Sprite)raiseButtonText;
	raiseButton.setPosition(190, 420);
	raiseButton.scale(sf::Vector2f(0.2, 0.2));

	if (!callButtonText.loadFromFile("./img/Call.png"))
	{
		std::cout << "Error cargar imagen" << std::endl;
	}
	callButton = (sf::Sprite)callButtonText;
	callButton.setPosition(190, 460);
	callButton.scale(sf::Vector2f(0.2, 0.2));

	if (!foldbuttonText.loadFromFile("./img/Fold.png"))
	{
		std::cout << "Error cargar imagen" << std::endl;
	}
	foldButton = (sf::Sprite)foldbuttonText;
	foldButton.setPosition(190, 500);
	foldButton.scale(sf::Vector2f(0.2, 0.2));
}

Carta Game::DarCarta()
{
	Carta c=Mazo[0];
	Mazo.erase(Mazo.begin());
	return c;
}