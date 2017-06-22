#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

#include "Bullet.hpp"
#include "Defines.hpp"
#include <string>

using namespace sf;
using namespace std;
class ClientTask
{
	
public:
	int clientId;
	int maxClients;
	int *bulletIdentificator;
	bool *sendBullets;
	Mutex *bulletsVectorMutex;
	Mutex *bulletIdentificatorMutex;
	Mutex *clientsPlayersBoxesMutex;
	FloatRect *myClientPlayerBox;	
	TcpSocket *mySocket;
	TcpSocket **sockets;
	Packet *myPacket;
	Packet **receivedPackets;
	vector<Bullet*> *bullets;
	vector<FloatRect*> *bulletsBoxes;
	

	ClientTask(int clientId, int maxClients, TcpSocket **sockets, Packet **receivedPackets, vector<Bullet*> *bullets, Mutex *bulletsVectorMutex, int *bulletIdentificator,
			   Mutex *bulletIdentificatorMutex, FloatRect **clientsPlayersBoxes, Mutex *clientsPlayersBoxesMutex, vector<FloatRect*> *bulletsBoxes);
	~ClientTask();
	void comunicateTask();
	
};
