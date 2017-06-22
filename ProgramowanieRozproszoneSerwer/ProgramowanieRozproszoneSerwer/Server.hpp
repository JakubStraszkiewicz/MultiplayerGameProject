#pragma once

#include <SFML/Network.hpp>
#include <Windows.h>

#include "ClientTask.hpp"
#include "Defines.hpp"

using namespace sf;

class Server
{
public:
	int *bulletIdentificator;
	bool *endOfGame;
	int maxClients;
	int *clientsPlayerNumberOfLifes;
	int *clientsPlayersHealth;
	int *clientsPoints;
	int *clientsPlayersDirection;
	Vector2f *clientsPlayersPosition;
	Mutex *bulletsVectorMutex;
	Mutex *clientsPlayersBoxesMutex;	
	Mutex *bulletIdentificatorMutex;
	vector<Bullet*> *bullets;
	vector<FloatRect*> *bulletsBoxes;
	FloatRect **clientsPlayersBoxes;
	TcpSocket **sockets;
	ClientTask **clientTasks;
	Packet **receivedPackets;
	sf::Thread **clientThreads;

	Server(int maxClients, Mutex *bulletsVectorMutex, int *bulletIdentificator, Mutex *bulletIdentificatorMutex, bool *endOfGame);
	~Server();
	void stop();
	void start(TcpSocket* sockets, Packet* receivedPacketsArray, int port);	
	void moveBullets(Packet deleteBulletPacket);
	void checkCollision(Packet deleteBulletPacket);
	void sendUpdatedBullets();
	void gameController();
	
};