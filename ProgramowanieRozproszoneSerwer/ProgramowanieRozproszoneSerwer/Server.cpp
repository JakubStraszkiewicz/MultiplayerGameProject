#include "Server.hpp"

Server::Server(int maxClients, Mutex *bulletsVectorMutex, int *bulletIdentificator, Mutex *bulletIdentificatorMutex, bool *endOfGame)
{
	//przypisanie przekazanych parametrów po kolei
	this->maxClients = maxClients;
	this->bulletsVectorMutex = bulletsVectorMutex;
	this->bulletIdentificator = bulletIdentificator;
	this->bulletIdentificatorMutex = bulletIdentificatorMutex;
	this->endOfGame = endOfGame;

	//inicjalizacja pól
	this->clientsPlayerNumberOfLifes = new int[maxClients];
	this->clientsPlayersHealth = new int[maxClients];
	this->clientsPlayersDirection = new int[maxClients];
	this->clientsPoints = new int[maxClients];
	this->clientsPlayersPosition = new Vector2f[maxClients]();
	this->clientsPlayersBoxesMutex = new Mutex();
	this->sockets = new TcpSocket*[maxClients];
	this->receivedPackets = new Packet*[maxClients];
	this->clientTasks = new ClientTask*[maxClients];
	this->clientThreads = new Thread*[maxClients];
	this->bullets = new vector<Bullet*>();
	this->bulletsBoxes = new vector<FloatRect*>();
	this->clientsPlayersBoxes = new FloatRect*[maxClients];

	//inicjalizacja poczatkowej wartosci zycia,punktow i liczby zyc graczy
	for (int i = 0; i < maxClients; i++)
	{
		clientsPlayersHealth[i] = STARTING_HEALTH;
		clientsPoints[i] = 0;
		clientsPlayerNumberOfLifes[i] = STARTING_LIFES;
	}

	//inicjalizacja poczatkowa boxow dla Playerow
	for (int i = 0; i < maxClients; i++)
	{
		clientsPlayersBoxes[i] = new FloatRect();
		clientsPlayersBoxes[i]->width = TEXTURE_SIZE;
		clientsPlayersBoxes[i]->height = TEXTURE_SIZE;
	}

	srand(time(NULL));
}

Server::~Server()
{
	delete[] sockets;
	delete[] receivedPackets;

	delete[] this->clientsPlayerNumberOfLifes;
	delete[] this->clientsPlayersHealth;
	delete[] this->clientsPoints;
	delete this->clientsPlayersBoxesMutex;

	delete[] this->clientTasks;
	delete[] this->clientThreads;
	delete this->bullets;
	delete this->bulletsBoxes;

	for (int i = 0; i < maxClients; i++)
	{
		delete clientsPlayersBoxes[i];
	}

	delete[] this->clientsPlayersBoxes;
}

void Server::stop()
{
	for (int i = 0; i < maxClients; i++)
	{
		clientThreads[i]->terminate();
		delete clientTasks[i];
		delete clientThreads[i];
	}
}

void Server::start(TcpSocket* socketsArray, Packet* receivedPacketsArray, int port)
{
	int counterClient = 0;
	sf::TcpListener listener;

	listener.listen(port);

	///////////////////////////////////////////////////////////////////
	//Nas³uchiwanie na porcie do czasu a¿ pod³¹cz¹ siê wszyscy klienci
	///////////////////////////////////////////////////////////////////
	while (counterClient != maxClients)
	{
		if (listener.accept(socketsArray[counterClient]) == sf::TcpListener::Done)
		{
			//zapisanie do tablicy socketow socketu dla kazdego klienta
			sockets[counterClient] = &socketsArray[counterClient];
			receivedPackets[counterClient] = &receivedPacketsArray[counterClient];

			cout << "Klient o IP " << sockets[counterClient]->getRemoteAddress() << " i numerze portu: " << sockets[counterClient]->getRemotePort() << " polaczyl sie z serwerem " << endl;

			counterClient++;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Wyslanie do kazdego klienta inicjalnego pakietu zawierajacego wiadomosc o tym którym indexem w tablicy playerow on jest
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Packet initialPacket;			

	//wylosowanie inicjalnej pozycji dla kazdego gracza
	for (int i = 0; i < maxClients; i++)
	{
		clientsPlayersPosition[i] = Vector2f(rand() % WIDTH, rand() % HIGH);
		clientsPlayersDirection[i] = rand() % 8 + 1;

		//ustawienie boxow klienta
		clientsPlayersBoxesMutex->lock();
		clientsPlayersBoxes[i]->top = clientsPlayersPosition[i].y - (clientsPlayersBoxes[i]->height / 2);
		clientsPlayersBoxes[i]->left = clientsPlayersPosition[i].x - (clientsPlayersBoxes[i]->width / 2);
		clientsPlayersBoxesMutex->unlock();
	}

	for (int i = 0; i < maxClients; i++)
	{
		for (int j = 0; j < maxClients; j++)
		{
			initialPacket.clear();

			if (i == j)
			{
				initialPacket << "It is me";
			}
			else
			{
				initialPacket << "It is others";
			}

			string playerInitialNick = "Player ";
			playerInitialNick += to_string(j + 1);

			initialPacket << j << playerInitialNick <<clientsPlayersPosition[j].x << clientsPlayersPosition[j].y << clientsPlayersDirection[j] << STARTING_HEALTH << STARTING_LIFES;

			sockets[i]->send(initialPacket);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////
	//Utworzenie nowego watku send/receive dla kazdego zaakceptowanego wczesniej klienta
	///////////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < maxClients; i++)
	{
		clientTasks[i] = new ClientTask(i, maxClients, sockets, receivedPackets, bullets, bulletsVectorMutex, bulletIdentificator, bulletIdentificatorMutex, clientsPlayersBoxes, clientsPlayersBoxesMutex, bulletsBoxes);
		clientThreads[i] = new  sf::Thread(&ClientTask::comunicateTask, clientTasks[i]);
		clientThreads[i]->launch();
	}

	cout << "Klienci sie podlaczyli, zaczynamy gre" << endl;
}

void Server::moveBullets(Packet deleteBulletPacket)
{
	////////////////////////////////////////////
	//Przesuniecie pozycji kul i ni¿ej ich boxów
	////////////////////////////////////////////
	for (int i = 0; i < bullets->size(); i++)
	{
		//je¿eli kula nadal znajduje siê na planszy
		if ((*bullets)[i]->position.y < HIGH && (*bullets)[i]->position.y > 0 && (*bullets)[i]->position.x < WIDTH && (*bullets)[i]->position.x >0)
		{
			//uaktualnienie pozycji kul
			switch ((*bullets)[i]->direction)
			{
			case DOWN:
				(*bullets)[i]->position = Vector2f((*bullets)[i]->position.x, (*bullets)[i]->position.y + SPEED);
				break;
			case UP:
				(*bullets)[i]->position = Vector2f((*bullets)[i]->position.x, (*bullets)[i]->position.y - SPEED);
				break;
			case LEFT:
				(*bullets)[i]->position = Vector2f((*bullets)[i]->position.x - SPEED, (*bullets)[i]->position.y);
				break;
			case RIGHT:
				(*bullets)[i]->position = Vector2f((*bullets)[i]->position.x + SPEED, (*bullets)[i]->position.y);
				break;
			case UP_LEFT:
				(*bullets)[i]->position = Vector2f((*bullets)[i]->position.x - SPEED, (*bullets)[i]->position.y - SPEED);
				break;
			case UP_RIGHT:
				(*bullets)[i]->position = Vector2f((*bullets)[i]->position.x + SPEED, (*bullets)[i]->position.y - SPEED);
				break;
			case DOWN_LEFT:
				(*bullets)[i]->position = Vector2f((*bullets)[i]->position.x - SPEED, (*bullets)[i]->position.y + SPEED);
				break;
			case DOWN_RIGHT:
				(*bullets)[i]->position = Vector2f((*bullets)[i]->position.x + SPEED, (*bullets)[i]->position.y + SPEED);
				break;
			}

			//uaktualnienie wartosci boxow
			(*bulletsBoxes)[i]->width = BULLET_TEXTURE_SIZE;
			(*bulletsBoxes)[i]->height = BULLET_TEXTURE_SIZE;
			(*bulletsBoxes)[i]->top = (*bullets)[i]->position.y - BULLET_TEXTURE_SIZE / 2;
			(*bulletsBoxes)[i]->left = (*bullets)[i]->position.x - BULLET_TEXTURE_SIZE / 2;
		}
		else
		{	//jezeli kula wyszla poza plansze

			//wyslanie do klientow wiadomosci o usunieciu danej kuli
			deleteBulletPacket.clear();
			deleteBulletPacket << "Bullets positions";
			deleteBulletPacket << "Delete bullet" << i << (*bullets)[i]->identificator << (*bullets)[i]->direction << (*bullets)[i]->position.x << (*bullets)[i]->position.y;

			for (int j = 0; j < maxClients; j++)
			{
				sockets[j]->send(deleteBulletPacket);
			}

			//usuniecie danej kuli z wektora pociskow
			delete (*bullets)[i];
			(*bullets).erase((*bullets).begin() + i);

			//usuniecie danej kuli z wektora boxow
			delete (*bulletsBoxes)[i];
			(*bulletsBoxes).erase((*bulletsBoxes).begin() + i);
		}
	}
}

void Server::checkCollision(Packet deleteBulletPacket)
{
	////////////////////////////////////
	//Sprawdzenie kolizji kul z graczami
	////////////////////////////////////
	clientsPlayersBoxesMutex->lock();
	for (int i = 0; i < maxClients; i++)
	{
		//sprawdzenie czy ten klient zyje, tylko jesli zyje to sprawdz kolizje z nim
		if (clientsPlayersHealth[i] > 0 && clientsPlayerNumberOfLifes[i] > 0)
		{
			for (int j = 0; j < bullets->size(); j++)
			{
				//Jezeli nastapila kolizja to wyslij ta wiadomosc do graczy i usun z wektorow
				if (clientsPlayersBoxes[i]->intersects(*(*bulletsBoxes)[j]))
				{
					///////////////////////////////////////////////////////////////////////////////////////
					//Update zycia klienta z ktorym wystapila kolizja i wyslanie tej wiadomosci do klientow
					///////////////////////////////////////////////////////////////////////////////////////

					//odjecie zycia dangego klienta
					clientsPlayersHealth[i] -= (*bullets)[j]->power;

					if (clientsPlayersHealth[i] <= 0 && clientsPlayerNumberOfLifes[i] > 1)	
					{
						//jezeli klient juz nie zyje to wyslij odpowiedni pakiet

						clientsPlayersHealth[i] = STARTING_HEALTH;
						clientsPlayerNumberOfLifes[i]--;

						//dodanie punktow graczowi ktory wyslal ta kule i zabil gracza
						clientsPoints[(*bullets)[j]->senderClientId] += 100;

						//stworzenie pakietu
						Packet killClientPacket;
						killClientPacket.clear();

						//wylosowanie nowej pozycji gracza
						Vector2f newPosition = Vector2f(rand() % WIDTH, rand() % HIGH);
						int newDirection = rand() % 8 + 1;

						//zupdatowanie boxa gracza dla nowej pozycji
						clientsPlayersBoxes[i]->top = newPosition.y - (clientsPlayersBoxes[i]->height / 2);
						clientsPlayersBoxes[i]->left = newPosition.x - (clientsPlayersBoxes[i]->width / 2);

						//etykieta,index,pozycja x, pozycja y, zycie, liczba zyc
						killClientPacket << "Player death" << i << newPosition.x << newPosition.y << newDirection << clientsPlayersHealth[i] << clientsPlayerNumberOfLifes[i];

						//wyslanie go do wszysktich klientow
						for (int k = 0; k < maxClients; k++)
						{
							sockets[k]->send(killClientPacket);
						}
					}
					else if (clientsPlayersHealth[i] > 0)
					{
						//jezeli klient zyje to wyslij update jego zycia

						//dodanie punktow graczowi ktory wyslal ta kule i ranil innego gracza
						clientsPoints[(*bullets)[j]->senderClientId] += 10;

						//stworzenie pakietu
						Packet updateClientHealthPacket;
						updateClientHealthPacket.clear();
						updateClientHealthPacket << "Update health" << i << clientsPlayersHealth[i];

						//wyslanie go do wszysktich klientow
						for (int k = 0; k < maxClients; k++)
						{
							sockets[k]->send(updateClientHealthPacket);
						}
					}
					else if (clientsPlayersHealth[i] <= 0 && clientsPlayerNumberOfLifes[i] <= 1)
					{
						//jezeli klient ostatecznie zginal

						clientsPlayersHealth[i] = 0;
						clientsPlayerNumberOfLifes[i]--;

						//dodanie punktow graczowi ktory wyslal ta kule i zabil gracza
						clientsPoints[(*bullets)[j]->senderClientId] += 100;

						//stworzenie pakietu
						Packet permanentlyClientDeathPacket;
						permanentlyClientDeathPacket.clear();

						//etykieta,index, zycie, liczba zyc
						permanentlyClientDeathPacket << "Player permanently death" << i << clientsPlayersHealth[i] << clientsPlayerNumberOfLifes[i];

						//wyslanie go do wszysktich klientow
						for (int k = 0; k < maxClients; k++)
						{
							sockets[k]->send(permanentlyClientDeathPacket);
						}
					}

					////////////////////////////////////////////////////////////
					//Wyslanie punktow wszystkich graczy do wszystkich klientow
					////////////////////////////////////////////////////////////

					//stworzenie pakietu z punktami wszystkich graczy
					Packet updatePointsPacket;
					updatePointsPacket.clear();

					updatePointsPacket << "Update points";
					for (int k = 0; k < maxClients; k++)
					{
						updatePointsPacket << k << clientsPoints[k];
					}

					//wyslanie go do wszysktich klientow
					for (int k = 0; k < maxClients; k++)
					{
						sockets[k]->send(updatePointsPacket);
					}

					///////////////////////////////////////////////////////////////
					//Sprawdzenie czy gra dobiegla konca (liczba zywych grazy <=1)
					///////////////////////////////////////////////////////////////
					int numberOfAlivePlayers = maxClients;
					for (int k = 0; k < maxClients; k++)
					{
						if (clientsPlayerNumberOfLifes[k] <= 0)
						{
							numberOfAlivePlayers--;
						}
					}

					//wyslanie pakietu o koncu gry
					if (numberOfAlivePlayers <= 1)
					{
						*endOfGame = true;

						Packet endOfGamePacket;

						endOfGamePacket << "End of game";

						//wyslanie go do wszysktich klientow
						for (int k = 0; k < maxClients; k++)
						{
							sockets[k]->send(endOfGamePacket);
						}
					}

					////////////////////////////////////////////////////////
					//wyslanie do klientow wiadomosci o usunieciu danej kuli
					////////////////////////////////////////////////////////
					deleteBulletPacket.clear();
					deleteBulletPacket << "Bullets positions";
					deleteBulletPacket << "Delete bullet" << j << (*bullets)[j]->identificator << (*bullets)[j]->direction << (*bullets)[j]->position.x << (*bullets)[j]->position.y;

					for (int k = 0; k < maxClients; k++)
					{
						sockets[k]->send(deleteBulletPacket);
					}

					//usuniecie z wektora pociskow
					delete (*bullets)[j];
					(*bullets).erase((*bullets).begin() + j);

					//usuniecie z wektora boxow
					delete (*bulletsBoxes)[j];
					(*bulletsBoxes).erase((*bulletsBoxes).begin() + j);
				}
			}
		}
	}
	clientsPlayersBoxesMutex->unlock();
}

void Server::sendUpdatedBullets()
{
	//////////////////////////////////////////////////
	//Wyslanie do klientow zaktualizownych pozycji kul
	//////////////////////////////////////////////////
	if ((*bullets).empty() == false)
	{
		Packet bulletsPacket;

		bulletsPacket.clear();
		bulletsPacket << "Bullets positions";

		for (int j = 0; j < bullets->size(); j++)
		{	
			bulletsPacket << "Update bullet" << j << (*bullets)[j]->identificator << (*bullets)[j]->direction << (*bullets)[j]->position.x << (*bullets)[j]->position.y;
		}

		//Wyslanie zaktualizowanych pozycji kul do wszystkich klientow
		for (int i = 0; i < maxClients; i++)
		{
			clientTasks[i]->mySocket->send(bulletsPacket);
		}
	}
}

void Server::gameController()
{
	Packet deleteBulletPacket;
	
	//Przesuniecie pozycji kul i ni¿ej ich boxów
	moveBullets(deleteBulletPacket);

	//Sprawdzenie kolizji kul
	checkCollision(deleteBulletPacket);

	//Wyslanie do klientow zaktualizownych pozycji kul
	sendUpdatedBullets();	
}

