#include "serwerMain.hpp"

int main(int argc, char *argv[])
{
	//sprawdzenie parametrow wejsciowych
	int port;
	if (argc == 3)
	{
		port = atoi(argv[2]);
	}
	else if (argc == 2)
	{
		port = 1500;
	}
	else
	{
		cout << "Nie podano wystarczajacej ilosci parametrow wejsciowych";
		return -1;
	}

	int maxClients = atoi(argv[1]);

	cout << "Serwer startuje" << endl;

	bool *endOfGame = new bool(false);
	int *bulletIdentificator = new int(0);
	Mutex *bulletIdentificatorMutex = new Mutex();
	Mutex *bulletsVectorMutex = new Mutex();
	Packet* receivedPackets = new Packet[maxClients];
	TcpSocket* sockets = new TcpSocket[maxClients];
	FloatRect *clientsPlayersBoxes = new FloatRect[maxClients];

	Server server(maxClients, bulletsVectorMutex, bulletIdentificator, bulletIdentificatorMutex, endOfGame);
	server.start(sockets, receivedPackets, port);

	while (1)
	{
		bulletsVectorMutex->lock();
		if ((*endOfGame) == true)
		{
			break;
		}
		server.gameController();
		bulletsVectorMutex->unlock();

		Sleep(20);
	}

	server.stop();
	delete bulletIdentificatorMutex;
	delete bulletIdentificator;
	delete bulletsVectorMutex;

	delete[] clientsPlayersBoxes;

	delete[] receivedPackets;
	delete[] sockets;
	return 0;
}