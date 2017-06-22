#pragma once

#include "Defines.hpp"

#define STARTING_HEALTH 100
#define UP 8
#define RIGHT 2
#define DOWN 4
#define LEFT 6
#define UP_RIGHT 1
#define DOWN_RIGHT 3
#define DOWN_LEFT 5
#define UP_LEFT 7
#define PISTOL 1
#define SHOTGUN 2
#define RIFLE 3
#define SHOTGUN2 4
#define TEXTURE_SIZE 42		//rozmiar tekstury gracza
#define WIDTH 1280	//trzeba bedzie zrobic porzadek z definamia te 3 s¹ te¿ w game.hpp
#define HIGH 720
#define SPEED 3

class Player
{
public:

	int points;
	int actualHealth;
	int ammunitionAmount;
	int weaponType;
	int direction;
	bool isAlive;
	int lives;
	string nickname;

	FloatRect playerBox;
	Texture texture;
	Vector2f position;
	Sprite* sprite;
	RectangleShape* healthBar;
	Text* nicknameLabel;

	Player();
	~Player();

	void move(int direction);
};

