#pragma once


#include "Player.hpp"
#include "MainMenu.hpp"
#include "Bullet.hpp"

#define SPEED 3
#define WIDTH 1280
#define HIGH 720
#define GRAPHIC_SIZE 40	//rozmiar textury mapy
#define BULLETDISTANCESMALL 30
#define BULLETDISTANCEMIDDLE 45
#define BULLETDISTANCELONG 75
#define SHOOTINTERVAL 400

class Game
{
public:

	int *myIndex;
	int maxPlayers;
	int gameState;

	RenderWindow* window;
	MainMenu* menu;
	Font font;
	Texture grassTexture;
	Sprite ***gameMap;

	Player* players;
	vector<Bullet*> vectorOfBullets;
	Bullet *bulletToSend;
	vector<Bullet*> *bullets;

	Mutex* sendFlagMutex;
	Mutex *playerMutex;
	Mutex *gameStateMutex;
	Mutex *bulletsToSendMutex;

	Game(RenderWindow* window, MainMenu* menu, int maxPlayers, int *myIndex, Bullet *bulletToSend, vector<Bullet*> *bullets, Mutex *bulletsToSendMutex, Mutex* sendFlagMutex, Mutex *playerMutex, Mutex *gameStateMutex);
	~Game();

	void drawPlayers();
	void drawBullets();
	void drawMap();
	void pistolShoot();
	void loadMap();

	void setPlayerNickname(string newNickname);
	void keyboardControl(Event event, int* timer);
	void mouseControl(Event event, int* timer);
};
