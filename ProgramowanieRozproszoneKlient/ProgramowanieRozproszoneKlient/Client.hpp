#pragma once

#include "Game.hpp"

using namespace sf;

class Client
{
public:

	int *myIndex;
	int *gameState;
	bool sendFlag;
	int maxPlayers;
	string serverIpAddress;

	TcpSocket socket;
	Packet packet;
	Packet *receivedPackets;

	Player *players;
	Bullet *bulletToSend;
	vector<Bullet*> *bullets;

	Mutex* playerMutex;
	Mutex* sendFlagMutex;
	Mutex* bulletsMutex;
	Mutex* bulletsToSendMutex;
	Mutex *gameStateMutex;

	Client(Player * players, int maxPlayers, string serverIpAddress, int * myIndex, Bullet *bulletToSend, vector<Bullet*>* bullets, Mutex * bulletsMutex, Mutex * bulletsToSendMutex, Mutex* sendFlagMutex, Mutex *playerMutex, Mutex *gameStateMutex, int *gameState);
	~Client();

	int connection(int port);
	void sendNewNickname();
	void disconnection();
	void sendPacket();
	void receivePackets();
};
