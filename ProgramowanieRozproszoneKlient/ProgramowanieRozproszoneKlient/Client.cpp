#include "Client.hpp"

Client::Client(Player * players, int maxPlayers, string serverIpAddress, int *myIndex, Bullet *bulletToSend, vector<Bullet*> *bullets, Mutex *bulletsMutex, Mutex *bulletsToSendMutex, Mutex *sendFlagMutex, Mutex *playerMutex, Mutex *gameStateMutex, int* gameState)
{
	this->sendFlagMutex = sendFlagMutex;
	this->bulletToSend = bulletToSend;
	this->playerMutex = playerMutex;
	this->bulletsMutex = bulletsMutex;
	this->bulletsToSendMutex = bulletsToSendMutex;
	this->gameStateMutex = gameStateMutex;
	this->bullets = bullets;
	this->bulletToSend = bulletToSend;
	this->myIndex = myIndex;
	this->sendFlag = false;
	this->players = players;
	this->maxPlayers = maxPlayers;
	this->serverIpAddress = serverIpAddress;
	this->gameState = gameState;
	this->receivedPackets = new Packet[maxPlayers];

}

Client::~Client()
{
	delete[] receivedPackets;
}

int Client::connection(int port)
{
	if (socket.connect(serverIpAddress, port) == Socket::Error)
	{
		cout << "Blad podczas polaczenia z serwerem" << endl;
		return -1;
	}
	else
	{
		Packet initialPacket;
		string initialText;
		string nickname;
		int id;
		int lives;
		int health;
		float x, y;
		int direction;

		for (int i = 0; i < maxPlayers; i++)
		{
			initialPacket.clear();
			socket.receive(initialPacket);

			initialPacket >> initialText >> id >> nickname >> x >> y >> direction >> health >> lives;

			if (initialText == "It is me")
			{
				*myIndex = id;
			}

			playerMutex->lock();
			players[i].nickname = nickname;
			players[i].position = Vector2f(x, y);
			players[i].direction = direction;
			players[i].actualHealth = health;
			players[i].lives = lives;
			playerMutex->unlock();

		}

		playerMutex->lock();
		this->players[*myIndex].texture.loadFromFile("Image/myPlayer.png");
		this->players[*myIndex].sprite->setTexture(this->players[*myIndex].texture);
		playerMutex->unlock();

		return 0;
	}
}

void Client::sendNewNickname()
{

	Packet newNicknamePacket;

	playerMutex->lock();
	newNicknamePacket << "Player nickname";
	newNicknamePacket << *myIndex;
	newNicknamePacket << players[*myIndex].nickname;
	playerMutex->unlock();

	socket.send(newNicknamePacket);
}


void Client::disconnection()
{
	Packet disconnectPacket;

	disconnectPacket.clear();
	disconnectPacket << "Disconnect";
	socket.send(disconnectPacket);
	socket.disconnect();
	cout << "Rozlaczono z serwerem" << endl;
	exit(-1);
}

void Client::sendPacket()
{
	bool bulletToSendFlag = false;

	while (1)
	{
		playerMutex->lock();
		if (players[*myIndex].isAlive)
		{
			playerMutex->unlock();

			sendFlagMutex->lock();
			if (sendFlag == true)
			{
				sendFlagMutex->unlock();

				packet.clear();

				bulletsToSendMutex->lock();
				if (bulletToSend->position.x > -50)
				{
					bulletToSendFlag = true;
				}
				bulletsToSendMutex->unlock();

				if (bulletToSendFlag == false)
				{
					playerMutex->lock();

					packet << "Player position";
					packet << *myIndex;
					packet << players[*myIndex].nickname;
					packet << players[*myIndex].position.x;
					packet << players[*myIndex].position.y;
					packet << players[*myIndex].direction;

					playerMutex->unlock();
				}
				else
				{
					packet << "New bullet";
					packet << *myIndex;
					bulletsToSendMutex->lock();
					packet << bulletToSend->direction;
					packet << bulletToSend->position.x;
					packet << bulletToSend->position.y;
					bulletToSend->position = Vector2f(-100, -100);
					bulletsToSendMutex->unlock();
				}
				socket.send(packet);

				sendFlagMutex->lock();
				sendFlag = false;
				sendFlagMutex->unlock();

				bulletToSendFlag = false;
				Sleep(10);
			}
			else
			{
				sendFlagMutex->unlock();
			}
		}
		else
		{
			playerMutex->unlock();
		}
	}
}

void Client::receivePackets()
{
	float x, y;
	int direction;
	int currentId;
	int clientPoints;
	int bulletDirection;
	int vectorBulletId;
	int bulletIdentificator;
	int lives;
	int health;
	int bulletToDeleteIndex = 0;
	string packetNickname;
	Packet receivedPacket;
	string packetLabel;
	string bulletAction;
	Vector2f bulletPosition;

	while (1)
	{
		receivedPacket.clear();
		socket.receive(receivedPacket);
		receivedPacket >> packetLabel;

		if (packetLabel == "Player position")
		{
			receivedPacket >> currentId >> packetNickname >> x >> y >> direction;

			playerMutex->lock();
			players[currentId].position = Vector2f(x, y);
			players[currentId].direction = direction;
			players[currentId].nickname = packetNickname;
			playerMutex->unlock();

		}
		else if (packetLabel == "Bullets positions")
		{
			while (receivedPacket.endOfPacket() != true)
			{
				bulletsMutex->lock();

				receivedPacket >> bulletAction;
				receivedPacket >> vectorBulletId >> bulletIdentificator >> bulletDirection >> bulletPosition.x >> bulletPosition.y;

				if (bulletAction == "New bullet")
				{
					bullets->push_back(new Bullet(bulletPosition, bulletDirection, bulletIdentificator));
				}
				else if (bulletAction == "Update bullet")
				{
					if (vectorBulletId < (*bullets).size())
					{
						(*bullets)[vectorBulletId]->position = bulletPosition;
						(*bullets)[vectorBulletId]->direction = bulletDirection;
					}
				}
				else if (bulletAction == "Delete bullet")
				{
					if (vectorBulletId < (*bullets).size())
					{
						delete (*bullets)[vectorBulletId];
						(*bullets).erase((*bullets).begin() + vectorBulletId);
					}
				}

				bulletsMutex->unlock();
			}
		}
		else if (packetLabel == "Update health")
		{
			receivedPacket >> currentId >> health;

			playerMutex->lock();
			players[currentId].actualHealth = health;
			playerMutex->unlock();
		}
		else if (packetLabel == "Player death")
		{
			receivedPacket >> currentId >> x >> y >> direction >> health >> lives;

			playerMutex->lock();
			players[currentId].position = Vector2f(x, y);
			players[currentId].direction = direction;
			players[currentId].actualHealth = health;
			players[currentId].lives = lives;
			playerMutex->unlock();

		}
		else if (packetLabel == "Player permanently death")
		{
			receivedPacket >> currentId >> health >> lives;

			playerMutex->lock();
			players[currentId].actualHealth = health;
			players[currentId].lives = lives;
			players[currentId].isAlive = false;
			playerMutex->unlock();
		}
		else if (packetLabel == "Player nickname")
		{
			receivedPacket >> currentId >> packetNickname;

			playerMutex->lock();
			players[currentId].nickname = packetNickname;
			playerMutex->unlock();
		}
		else if (packetLabel == "Update points")
		{
			while (receivedPacket.endOfPacket() != true)
			{
				receivedPacket >> currentId >> clientPoints;

				playerMutex->lock();
				players[currentId].points = clientPoints;
				playerMutex->unlock();
			}
		}
		else if ("End of game")
		{
			gameStateMutex->lock();
			*gameState = END_GAME;
			gameStateMutex->unlock();
		}
		else if (packetLabel == "Disconnect")
		{
			cout << "Disconnect " << endl;
			disconnection();
		}
	}
}
