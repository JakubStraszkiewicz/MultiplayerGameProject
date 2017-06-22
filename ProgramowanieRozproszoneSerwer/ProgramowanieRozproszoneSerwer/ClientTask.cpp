#include "ClientTask.hpp"

ClientTask::ClientTask(int clientId, int maxClients, TcpSocket **sockets, Packet **receivedPackets, vector<Bullet*> *bullets, Mutex *bulletsVectorMutex, int *bulletIdentificator, 
					   Mutex *bulletIdentificatorMutex, FloatRect **clientsPlayersBoxes, Mutex *clientsPlayersBoxesMutex, vector<FloatRect*> *bulletsBoxes)
{
	//przypisanie przekazanych parametrów po kolei
	this->clientId = clientId;
	this->maxClients = maxClients;
	this->sockets = sockets;
	this->receivedPackets = receivedPackets;
	this->bullets = bullets;
	this->bulletsVectorMutex = bulletsVectorMutex;
	this->bulletIdentificator = bulletIdentificator;
	this->bulletIdentificatorMutex = bulletIdentificatorMutex;
	this->myClientPlayerBox = clientsPlayersBoxes[clientId];
	this->clientsPlayersBoxesMutex = clientsPlayersBoxesMutex;
	this->bulletsBoxes = bulletsBoxes;

	//dalsze przypisanie wartoœci pól
	this->mySocket = sockets[clientId];
	this->myPacket = receivedPackets[clientId];

	//inicjalizacja pól
	this->sendBullets= new bool(false);	
}

ClientTask::~ClientTask()
{
	delete this->sendBullets;
}

void ClientTask::comunicateTask()
{
	//zmienne sluzace do rozpakowania pakietow
	float x, y;
	int index;
	int direction;
	int myPacketClientId;
	int myPacketClientDirection;
	string nickname;
	string packetLabel;
	string myPacketNickname;
	Vector2f myPacketClientPosition;
	
	while (1)
	{
		//odebranie nowego pakietu na socketcie danego klienta
		myPacket->clear();
		mySocket->receive(*myPacket);
	
		//////////////////////////////////////////
		//sprawdzenie etykiety pakietu - jego typu
		//////////////////////////////////////////
		*myPacket >> packetLabel;
		if (packetLabel == "Player position")		//pakiet aktualizujacy pozycje danego gracza
		{		
			*myPacket >> myPacketClientId >> myPacketNickname >> myPacketClientPosition.x >> myPacketClientPosition.y >> myPacketClientDirection;	

			//zaktualizowanie wartoœci playerBoxa danego klienta
			clientsPlayersBoxesMutex->lock();	
			myClientPlayerBox->top = myPacketClientPosition.y - (myClientPlayerBox->height / 2);
			myClientPlayerBox->left = myPacketClientPosition.x - (myClientPlayerBox->width / 2);
			clientsPlayersBoxesMutex->unlock();

			//wyslanie tego pakietu do wszystkich pozostalych klientow
			for (int i = 0; i < maxClients; i++)
			{
				if (clientId != i)
				{
					sockets[i]->send(*myPacket);
				}
			}
		}
		else if (packetLabel == "New bullet")		//pakiet zglaszajacy nowo wystrzelon¹ kulê
		{
			*myPacket >> index >>direction>> x >> y;

			//dodanie nowej kuli do wektora i jej boxa do wektora boxow
			bulletsVectorMutex->lock();
			bulletIdentificatorMutex->lock();
			bullets->push_back(new Bullet(Vector2f(x, y), direction, *bulletIdentificator, index));
			bulletsBoxes->push_back(new FloatRect(x- BULLET_TEXTURE_SIZE/2,y- BULLET_TEXTURE_SIZE/2, BULLET_TEXTURE_SIZE, BULLET_TEXTURE_SIZE));
			(*bulletIdentificator)++;
			bulletIdentificatorMutex->unlock();
			bulletsVectorMutex->unlock();

			//wyslanie polecenia stworzenia nowej kuli do klientow
			myPacket->clear();
			*myPacket << "Bullets positions";
			bulletsVectorMutex->lock();		
			*myPacket << "New bullet" << bullets->size() << (*bullets)[bullets->size() - 1]->identificator << (*bullets)[bullets->size()-1]->direction << (*bullets)[bullets->size()-1]->position.x << (*bullets)[bullets->size()-1]->position.y;
			bulletsVectorMutex->unlock();

			for (int i = 0; i < maxClients; i++)
			{
				sockets[i]->send(*myPacket);
			}

		}
		else if (packetLabel == "Player nickname")		//Pakiet zg³aszaj¹cy aktualizacjie nicknamu
		{
			//wyslanie tego pakietu do wszystkich pozostalych klientow
			for (int i = 0; i < maxClients; i++)
			{
				if (clientId != i)
				{
					sockets[i]->send(*myPacket);
				}
			}
		}
		else if (packetLabel == "Disconnect")		//Pakiet zg³aszaj¹cy roz³¹czenie klienta
		{
			myPacket->clear();
			*myPacket << "Disconnect";

			for (int i = 0; i < maxClients; i++)
			{
				sockets[i]->send(*myPacket);
			}
		}
	}
}



