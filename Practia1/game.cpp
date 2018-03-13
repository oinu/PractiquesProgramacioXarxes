#include "iostream"

using namespace std;

enum Palos {DIAMANTE,TREBOL,PICA,CORAZON,OCULTO};

void main(){
    //llama el juego a accion y los servidores/clientes en un caso y los peers en otros 
}

// classe Carta
class Carta {
int numero;
Palos palo;
sf::Image img;

void Carta()
{
    numero=0;
    palo= Palos::OCULTO;
    
    //Inciar la imagen como el reverso de una carta
    
}

void Carta(int aNum, Palos aPalo, sf::Image aImg)
{
    numero=aNum;
    palo=aPalo;
    img=aImg;
};

// Classe Jugador
class Player {
Carta carta1;
Carta carta2;
int efectivo;
int cantidadApostada;

void Player()
{
    //Si no se inician las variables Carta por defecto, forzarlo
    
    //Dar al jugador la cantidad de efectivo inicial
    cantidadApostada=0;
}

};

// Classe "taula" per fer funcionar el joc
//Part del header
class Crupier {
Carta[5] mesa;
std::vector<Player> players;
sf::Image bg;
int efectivoMesa;
void Crupier();
std::stack<Carta> IniciarPartida();
bool CartaAlta();
bool Pareja();
bool Trio();
bool DoblePareja();
bool Poker();
bool Full();
bool Escalera();
bool EscaleraColor();
void ComprovarMano();

// Part que anira al cpp
void Crupier::Crupier()
{
    
    //Efectivo de mesa a 0;
    efectivoMesa=0;
}
std::stack<Carta> Crupier::IniciarPartida()
{
    std::stack<Carta> baraja;
    
    //Crear array de cartas y mesclarlas
    
    //Meter las cartas en la baraja
    
    return baraja;
    
}

bool Crupier::CartaAlta()
{

}
};