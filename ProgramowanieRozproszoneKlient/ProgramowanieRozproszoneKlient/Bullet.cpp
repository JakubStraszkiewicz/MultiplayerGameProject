#include "Bullet.hpp"

Bullet::Bullet(Vector2f position,int direction, int identificator)
{
	this->identificator = identificator;
	this->power = POWER;
	this->texture.loadFromFile("Image/blueberryBullet.png");
	this->sprite = new Sprite(this->texture);
	this->position = position;
	this->direction = direction;
	this->sprite->setPosition(position);
	this->sprite->setOrigin(Vector2f(7, 5));
	this->sound.openFromFile("Sounds/shoot.ogg");
	this->bulletBox = this->sprite->getGlobalBounds();
}
Bullet::~Bullet()
{
	delete this->sprite;
}

