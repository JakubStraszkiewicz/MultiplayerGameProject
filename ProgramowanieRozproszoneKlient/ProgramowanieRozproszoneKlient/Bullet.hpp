#pragma once
#include "Defines.hpp"

#define POWER 5

class Bullet
{
public:

	int identificator;
	int power;
	int direction;

	Music sound;
	Vector2f position;
	Texture texture;
	Sprite* sprite;
	FloatRect bulletBox;

	Bullet(Vector2f position, int direction, int identificator);
	~Bullet();
};