#include "Bullet.hpp"

Bullet::Bullet(Vector2f position,int direction, int identificator, int senderClientId)
{
	this->identificator = identificator;
	this->power = POWER;
	this->senderClientId = senderClientId;
	this->position = position;
	this->direction = direction;
}