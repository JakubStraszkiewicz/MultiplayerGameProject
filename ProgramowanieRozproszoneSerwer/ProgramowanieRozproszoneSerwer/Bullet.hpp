#pragma once

#include <string>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

#define POWER 20

class Bullet
{
public:
	int identificator;
	Bullet(Vector2f position, int direction, int identificator,int senderClientId);
	int senderClientId;
	int power;
	int direction;
	Vector2f position;
};