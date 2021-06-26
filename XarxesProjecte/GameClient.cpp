#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include "../GameLib/PlayerInfo.h"
#include <string>
#include <cstring>
#include <iostream>
#include <thread>
#include <random>
using namespace sf;
using namespace std;

#define SERVER_PORT 50000
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define NUM_PLAYERS 2
#define BUFFER_SIZE 2000

static const string gameID = "GAME";
static const string chatID = "CHAT";
static const string tornID = "TORN";
static const string cartaID = "CARTA";
static const string separatorChar = "%";
static const string separatorCharTwo = "$";


enum Culturas
{
	ARABE = 0,
	BANTU = 1,
	CHINA = 2,
	ESQUIMAL = 3,
	INDIA = 4,
	MEXICANA = 5,
	TIROLESA = 6,
	SIZE = 7
};

enum Familias
{
	Abuelo = 0,
	Abuela = 1,
	Padre = 2,
	Madre = 3,
	Hijo = 4, 
	Hija = 5, 
	Size = 6
};


class Card
{
public:
	Card(Culturas cultura, Familias familia)
	{
		c = cultura;
		f = familia;
	}
	Culturas c;
	Familias f;
	
};

class Deck
{
public:
	Deck()
	{
		for (int c = 0; c < 7; c++)
		{
			for (int f = 0; f < 6; f++)
			{
				const auto card = Card(static_cast<Culturas>(c), static_cast<Familias>(f));
				DeckOfCards.push_back(card);
			}
		}
	}

	void Shuffle(int seed)
	{
		std::shuffle(DeckOfCards.begin(), DeckOfCards.end(), std::default_random_engine(seed));
	}

	std::vector<Card> DeckOfCards;
};



struct Directions
{
	string IP;
	unsigned short PORT;
	int torn;
};

struct Player
{
	int torn;
	bool isMyTurn = false;
	int seed;
	int contadorFamilies;

	int contadorCartesArab = 0;
	int contadorCartesBantu = 0;
	int contadorCartesChina = 0;
	int contadorCartesEsquimal = 0;
	int contadorCartesIndia = 0;
	int contadorCartesMexicana = 0;
	int contadorCartesTirolesa = 0;

	vector<Card> hand;

};



void ShowMyHand(vector<Card> hand, Player localPlayer)
{
	cout << "-------------------------------------------------------" << endl;
	cout << "This is your hand Player " << localPlayer.torn << " : " << endl;
	for (int i = 0; i < hand.size(); i++)
	{
		cout << "CULTURA-> " << hand[i].c << " and FAMILY-> " << hand[i].f << endl;
	}
}

void ShowScoreCounter(Player* player)
{
	cout << "-------------------------------------------------------" << endl;
	cout << "SCORE FAMÍLIES : " << player->contadorFamilies << endl;

}

void UpdateMyFamCounter(vector<Card>* myHand, Player* localPlayer)
{
	localPlayer->contadorCartesArab = 0;
	localPlayer->contadorCartesBantu = 0;
	localPlayer->contadorCartesChina = 0;
	localPlayer->contadorCartesEsquimal = 0;
	localPlayer->contadorCartesIndia = 0;
	localPlayer->contadorCartesMexicana = 0;
	localPlayer->contadorCartesTirolesa = 0;
	localPlayer->contadorFamilies = 0;

	for (int i = 0; i < myHand->size(); i++)
	{

		if (myHand->at(i).c == Culturas::ARABE) localPlayer->contadorCartesArab++;
		else if (myHand->at(i).c == Culturas::BANTU) localPlayer->contadorCartesBantu++;
		else if (myHand->at(i).c == Culturas::CHINA) localPlayer->contadorCartesChina++;
		else if (myHand->at(i).c == Culturas::ESQUIMAL) localPlayer->contadorCartesEsquimal++;
		else if (myHand->at(i).c == Culturas::INDIA) localPlayer->contadorCartesIndia++;
		else if (myHand->at(i).c == Culturas::MEXICANA) localPlayer->contadorCartesMexicana++;
		else if (myHand->at(i).c == Culturas::TIROLESA) localPlayer->contadorCartesTirolesa++;

		if (localPlayer->contadorCartesArab == 6)
		{
			localPlayer->contadorFamilies++;
		}
		else if (localPlayer->contadorCartesBantu == 6)
		{
			localPlayer->contadorFamilies++;
		}
		else if (localPlayer->contadorCartesChina == 6)
		{
			localPlayer->contadorFamilies++;
		}
		else if (localPlayer->contadorCartesEsquimal == 6)
		{
			localPlayer->contadorFamilies++;
		}
		else if (localPlayer->contadorCartesIndia == 6)
		{
			localPlayer->contadorFamilies++;
		}
		else if (localPlayer->contadorCartesMexicana == 6)
		{
			localPlayer->contadorFamilies++;
		}
		else if (localPlayer->contadorCartesTirolesa == 6)
		{
			localPlayer->contadorFamilies++;
		}

		
	}
	ShowScoreCounter(localPlayer);
}

void ShowOtherPlayerHands(vector<Player> players)
{
	cout << "-------------------------------------------------------" << endl;
	cout << "HANDS FROM OTHER PLAYERS:" << endl;
	for (int i = 0; i < players.size(); i++)
	{
		cout << "Player " << players[i].torn << endl;
		for (int j = 0; j < players[i].hand.size(); j++)
		{
			cout << "CULTURA-> " << players[i].hand[j].c << " and FAMILY-> " << players[i].hand[j].f << endl;
		}
	}
}

void ShowFamiliesCounter(Player localPlayer)
{
	cout << "-------------------------------------------------------" << endl;
	cout << "FAMILIES COUNTER:" << endl;
	cout << "0-ARAB " << localPlayer.contadorCartesArab << endl;
	cout << "1-BANTU " << localPlayer.contadorCartesBantu << endl;
	cout << "2-CHINA " << localPlayer.contadorCartesChina << endl;
	cout << "3-ESQUIMAL " << localPlayer.contadorCartesEsquimal << endl;
	cout << "4-INDIA " << localPlayer.contadorCartesIndia << endl;
	cout << "5-MEXICANA " << localPlayer.contadorCartesMexicana << endl;
	cout << "6-TIROLESA " << localPlayer.contadorCartesTirolesa << endl;

}

void ReceivedFunction(vector<sf::TcpSocket*> socket, vector<string>* aMensajes, vector<string>* gMensajes, sf::SocketSelector* ss, 
	int* currentTorn, vector<Card>* myHand, Player player, vector<Player>* Players)
{
	char buffer[BUFFER_SIZE];
	string msn;
	size_t received;
	while (ss->wait())
	{
		for (int i = 0; i < socket.size(); i++)
		{
			if (ss->isReady(*socket[i]))
			{
				
				sf::Socket::Status st = socket[i]->receive(buffer, BUFFER_SIZE, received);
				if (st == sf::Socket::Status::Done)
				{											
					msn = buffer;
					//DIFERENCIAR ENTRE XAT I GAME
					
					const auto pos = msn.find(separatorChar);

					const auto header = msn.substr(0, pos);

					const auto body = msn.substr(pos + 1,msn.size());

					if (header == chatID)
					{
						aMensajes->push_back(body);
						if (aMensajes->size() > 25)
						{
							aMensajes->erase(aMensajes->begin(), aMensajes->begin() + 1);
						}
					}
					else if (header == gameID)
					{
						
						const auto header2 = body.substr(0, 1);

						const auto subbody = body.substr(1, body.size());

						if (header2 == to_string(player.torn))
						{
							
							const auto cultura = subbody.substr(0, 1);
							const auto familia = subbody.substr(1, 2);

							int tempCultura = stoi(cultura);
							int tempFamilia = stoi(familia);

							for (int i = 0; i < myHand->size(); i++)
							{
								
								if (myHand->at(i).c == tempCultura && myHand->at(i).f == tempFamilia)
								{
									cout << "TINC LA CARTA " << tempCultura << " i " << tempFamilia << endl; 

									//TO DO copying in local new card for the player
									/*for (int j = 0; j < Players->size(); i++)
									{
										if (Players->at(j).torn == stoi(header2))
										{
											Culturas culturaVal = static_cast<Culturas>(tempCultura);
											Familias familiaVal = static_cast<Familias>(tempFamilia);
											Card tempCard(culturaVal, familiaVal);
											Players->at(j).hand.push_back(tempCard);
										}
									}*/

									myHand->erase(myHand->begin() + i);
									cout << "-------------------------" << endl;
									cout << "T'han robat una nova carta!" << endl;
									UpdateMyFamCounter(myHand, &player);
									ShowFamiliesCounter(player);
									

								}
								//else cout << "NO TINC LA CARTA" << tempCultura << " i " << tempFamilia << endl;
							}


						}


						gMensajes->push_back(subbody);
						if (gMensajes->size() > 25)
						{
							gMensajes->erase(gMensajes->begin(), gMensajes->begin() + 1);
						}

					}
					else if (header == tornID)
					{
						*currentTorn = std::stoi(body);
						cout << "Aquest es el nou torn A " << *currentTorn << endl;

					}
				}
			}
		}
	}
}


int main()
{
	cout << "Enter your nickname to start the chat: ";
	string nickname;
	cin >> nickname;
	cout << endl;

	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	sf::TcpSocket socketServer;
	TcpListener peerListener;

	SocketSelector ss;
	char connectionType, mode;
	char buffer[2000];
	std::size_t received;
	sf::Socket::Status socketStatus;
	std::string text = "Connected to: ";

	vector<Directions> directionList;
	vector<TcpSocket*> socketList;

	Packet p;
	Player localPlayer;

	if (!nickname.empty())
	{
		//CAS CLIENT

		socketServer.connect(ip, SERVER_PORT);
		socketStatus = socketServer.receive(p);

		p >> localPlayer.seed;

		if (socketStatus != Socket::Done)
		{
			cout << "No se ha podido conectar" << endl;
		}
		else
		{

			int l;
			p >> l;

			if (l != 0)
			{


				for (int j = 0; j < l; j++)
				{
					socketStatus = socketServer.receive(p);

					if (socketStatus != sf::TcpSocket::Status::Done)
					{
						cout << "Error" << endl;
					}
					else
					{

						Directions d;


						p >> d.IP >> d.PORT >> localPlayer.torn;

						++localPlayer.torn;
						cout << localPlayer.torn << endl;
						d.torn = localPlayer.torn;

						directionList.push_back(d);
						
					}
				}

				for (int i = 0; i < directionList.size(); i++)
				{
					TcpSocket *socket = new TcpSocket;

					socket->connect(directionList[i].IP, directionList[i].PORT);

					ss.add(*socket);

					socketList.push_back(socket);



				}
			}
			else
			{
				cout << "ets el primer en connectar" << endl;
				localPlayer.torn = 1;
				localPlayer.isMyTurn = true;
				cout << localPlayer.torn << endl;
			}

			cout << "ERES EL CHATEADOR " << l + 1 << " de " << NUM_PLAYERS << " CHATEANTES" << endl;

		}
	}

	int localPort = socketServer.getLocalPort();

	socketServer.disconnect();

	peerListener.listen(localPort);


	for (int i = socketList.size(); i < NUM_PLAYERS - 1; i++)
	{
		TcpSocket* nSocket = new TcpSocket();

		peerListener.accept(*nSocket);

		socketList.push_back(nSocket);

		ss.add(*nSocket);
	}

	peerListener.close();


	char data[100];

	int currentTorn = 1;

	std::vector<std::string> aMensajes;
	std::vector<std::string> gMensajes;
	

	//WINOW CHAT
	sf::RenderWindow window;
	window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chat_Client -> user: " + nickname);

	sf::RenderWindow windowGame;
	windowGame.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "CARD GAME : " + nickname);
	

	sf::Font font;
	if (!font.loadFromFile("arial_narrow_7.ttf"))
	{
		cout << "Can't load the font file" << std::endl;
	}

	
	

	std::string mensaje = "" + nickname + " > ";
	string logOutMensaje = " " + nickname + " has disconected the chat.";
	

	string wordEntry = "";

	vector<string> wordEscrit;

	sf::Text chattingText(mensaje, font, 14);
	chattingText.setFillColor(sf::Color(0, 160, 0));
	chattingText.setStyle(sf::Text::Bold);

	sf::Text chattingGameText(mensaje, font, 14);
	chattingGameText.setFillColor(sf::Color(0, 160, 0));
	chattingGameText.setStyle(sf::Text::Bold);


	sf::Text text1(mensaje, font, 14);
	text1.setFillColor(sf::Color(0, 160, 0));
	text1.setStyle(sf::Text::Bold);
	text1.setPosition(0, 560);

	sf::Text text2(wordEntry, font, 14);
	text2.setFillColor(sf::Color(0, 160, 0));
	text2.setStyle(sf::Text::Bold);
	text2.setPosition(0, 560);



	sf::RectangleShape separator(sf::Vector2f(800, 5));
	separator.setFillColor(sf::Color(200, 200, 200, 255));
	separator.setPosition(0, 550);


	Packet logOutPacket;
	Socket::Status logOutStatus;




	Text wordText(wordEntry, font, 14);
	wordText.setFillColor(sf::Color(0, 160, 0));
	wordText.setStyle(sf::Text::Bold);

	Deck DeckGame;

	DeckGame.Shuffle(localPlayer.seed);

	
	vector<Player> Players;
	vector<Card> myHand;

	for (int i = 1; i <= socketList.size()+1; i++)
	{
		
		if (localPlayer.torn != i)
		{
			Player tempPlayer;
			tempPlayer.torn = i;
			Players.push_back(tempPlayer);
			cout << "He afegit un player al llistat de Players amb ID " << tempPlayer.torn << endl;

		}
		
	}

	int contador = 1;

	for (int i = 0; i < DeckGame.DeckOfCards.size(); i++)
	{

		if (contador > NUM_PLAYERS)
		{
			contador = 1;
		}

		if (contador == localPlayer.torn)
		{
			myHand.push_back(DeckGame.DeckOfCards[i]);

		}
		else
		{
			for (int j = 0; j < Players.size(); j++)
			{
				if (contador == Players[j].torn)
				{
					Players[j].hand.push_back(DeckGame.DeckOfCards[i]);
				}
			}
		}
		contador++;
	}
	   
	thread t1(ReceivedFunction, socketList, &aMensajes, &gMensajes, &ss, &currentTorn, &myHand, localPlayer, &Players);

	ShowMyHand(myHand, localPlayer);
	
	ShowOtherPlayerHands(Players);
	
	UpdateMyFamCounter(&myHand, &localPlayer);

	ShowFamiliesCounter(localPlayer);

	while (window.isOpen() && windowGame.isOpen())
	{
		if (currentTorn > NUM_PLAYERS)
		{
			currentTorn = 1;
		}

		if (currentTorn == localPlayer.torn)
		{
			localPlayer.isMyTurn = true;
			
		}
		else localPlayer.isMyTurn = false;



		//GAME WINDOW
		sf::Event eventoGame;
		while (windowGame.pollEvent(eventoGame))
		{
			switch (eventoGame.type)
			{
			case sf::Event::Closed:
				window.close();
				windowGame.close();
				break;
			case sf::Event::KeyPressed:
				if (eventoGame.key.code == sf::Keyboard::Escape)
					windowGame.close();
				else if (eventoGame.key.code == sf::Keyboard::Return)
				{
					if (localPlayer.isMyTurn)
					{
						gMensajes.push_back(wordEntry);


						if (gMensajes.size() > 25)
						{
							gMensajes.erase(gMensajes.begin(), gMensajes.begin() + 1);
						}

						for (sf::TcpSocket* s : socketList)
						{
							//ENVIO CADENA DE JOC
							wordEntry = gameID + separatorChar + wordEntry;
							socketStatus = s->send(wordEntry.c_str(), wordEntry.size() + 1);
							if (socketStatus != sf::TcpSocket::Status::Done)
							{
								cout << "Error" << endl;
							}
							
							const auto pos = wordEntry.find(separatorChar);

							const auto body = wordEntry.substr(pos + 1, wordEntry.size());

							

							const auto player = body.substr(0, 1);
							const auto cultura = body.substr(1, 1);
							const auto familia = body.substr(2, 3);

							int tempPlayer = stoi(player);
							int tempCultura = stoi(cultura);
							int tempFamilia = stoi(familia);
							 
							for (int i = 0; i < Players.size(); i++)
							{
								if (Players[i].torn == tempPlayer)
								{
									for (int j = 0; j < Players[i].hand.size(); j++)
									{
										if (Players[i].hand[j].c == tempCultura && Players[i].hand[j].f == tempFamilia)
										{
											Culturas culturaVal = static_cast<Culturas>(tempCultura);
											Familias familiaVal = static_cast<Familias>(tempFamilia);
											Card tempCard(culturaVal, familiaVal);
											myHand.push_back(tempCard);
											cout << "-------------------------" << endl;
											cout << "Has robat una nova carta!" << endl;
											ShowMyHand(myHand, localPlayer);
											UpdateMyFamCounter(&myHand, &localPlayer);
											ShowFamiliesCounter(localPlayer);
											Players[i].hand.erase(Players[i].hand.begin() + j);
										}
									}
								}
							}

							//ENVIO ACTUALITZACIÓ DEL TORN
							currentTorn++;
							string gTorn = tornID + separatorChar + std::to_string(currentTorn);
							socketStatus = s->send(gTorn.c_str(), gTorn.size() + 1);
							if (socketStatus != sf::TcpSocket::Status::Done)
							{
								cout << "Error" << endl;
							}
							
						}

						wordEntry = "";
					}
					else
					{
						cout << "No es el teu torn" << endl;
						cout << "Aquest es el torn del joc: " << currentTorn << " i el teu es " << localPlayer.torn << endl;
						wordEntry = "";
					}
					

				}
				break;

			case sf::Event::TextEntered:

				//Introduir caracters
				if (eventoGame.text.unicode >= 32 && eventoGame.text.unicode <= 126)
					wordEntry += (char)eventoGame.text.unicode;
				else if (eventoGame.text.unicode == 8 && wordEntry.length() > 0)
					wordEntry.erase(wordEntry.length() - 1, wordEntry.length());
				break;
			}

		}

		windowGame.draw(separator);
			

		//PINTA FINESTRA CHAT
		for (size_t i = 0; i < gMensajes.size(); i++)
		{
			std::string chattingGame = gMensajes[i];
			chattingGameText.setPosition(sf::Vector2f(0, 20 * i));
			chattingGameText.setString(chattingGame);
			windowGame.draw(chattingGameText);
		}


		string mensajeGame_ = " Type your action: " + wordEntry + "_";
		text2.setString(mensajeGame_);
		windowGame.draw(text2);


		windowGame.display();
		windowGame.clear();




		string mensajeWin;

		sf::Event evento;
		while (window.pollEvent(evento)) //EVENT CHAT
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

					aMensajes.push_back(mensaje);


					if (aMensajes.size() > 25)
					{
						aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
					}

					for (sf::TcpSocket* s : socketList)
					{
						mensaje = chatID + separatorChar + mensaje;
						socketStatus = s->send(mensaje.c_str(),mensaje.size() + 1);
						if (socketStatus != sf::TcpSocket::Status::Done)
						{
							cout << "Error" << endl;
						}
					}

					mensaje = "" + nickname + " > ";
				}
				break;
			case sf::Event::TextEntered:
				if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
					mensaje += (char)evento.text.unicode;
				else if (evento.text.unicode == 8 && mensaje.length() > 0)
					mensaje.erase(mensaje.length() - 1, mensaje.length());
				break;
			}
		}
		
		//LINIA SEPARADORA PER INTRODUIR CARACTERS
		window.draw(separator);
	
		//PINTA FINESTRA CHAT
		for (size_t i = 0; i < aMensajes.size(); i++)
		{
			std::string chatting = aMensajes[i];
			chattingText.setPosition(sf::Vector2f(0, 20 * i));
			chattingText.setString(chatting);
			window.draw(chattingText);
		}


		std::string mensaje_ = mensaje + "_";
		text1.setString(mensaje_);
		window.draw(text1);

		window.display();
		window.clear();
	}

	if (!window.isOpen())
	{
		mensaje = nickname + " has disconnected.";

		for (sf::TcpSocket* s : socketList)
		{
			socketStatus = s->send(mensaje.c_str(), mensaje.size() + 1);
			if (socketStatus != sf::TcpSocket::Status::Done)
			{
				cout << "Error" << endl;
			}
		}
	}

	ss.clear();
	directionList.clear();
	for (sf::TcpSocket* s : socketList)
	{
		s->disconnect();
	}
	socketList.clear();

	t1.join();

	socketServer.disconnect();

	return 0;
}