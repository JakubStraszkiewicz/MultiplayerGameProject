#include "Player.hpp"

Player::Player()
{
	this->actualHealth = STARTING_HEALTH;
	this->ammunitionAmount = 0;
	this->weaponType = PISTOL;
	this->texture.loadFromFile("Image/player.png");
	this->sprite = new Sprite(this->texture);
	this->nickname = "";
	this->sprite->setOrigin(Vector2f(TEXTURE_SIZE / 2, TEXTURE_SIZE / 2));
	this->playerBox = this->sprite->getGlobalBounds();
	this->isAlive = true;
	this->points = 0;
	this->healthBar = new RectangleShape();
	this->nicknameLabel = new Text();
}

Player::~Player()
{
	delete this->sprite;
	delete this->healthBar;
	delete this->nicknameLabel;
}

void Player::move(int direction)
{
	switch (direction)
	{
	case UP:
		if (this->position.y - SPEED - TEXTURE_SIZE / 2 > 0)
		{
			this->position.y -= SPEED;
			this->direction = UP;
		}
		break;
	case DOWN:
		if (this->position.y + SPEED + TEXTURE_SIZE / 2 < HIGH)
		{
			this->position.y += SPEED;
			this->direction = DOWN;
		}
		break;
	case LEFT:
		if (this->position.x - SPEED - TEXTURE_SIZE / 2 > 0)
		{
			this->position.x -= SPEED;
			this->direction = LEFT;
		}
		break;
	case RIGHT:
		if (this->position.x + SPEED + TEXTURE_SIZE / 2 < WIDTH)
		{
			this->position.x += SPEED;
			this->direction = RIGHT;
		}
		break;
	case UP_RIGHT:
		if (this->position.y - SPEED - TEXTURE_SIZE / 2 > 0 && this->position.x + SPEED + TEXTURE_SIZE / 2 < WIDTH)
		{
			this->position.x += SPEED;
			this->position.y -= SPEED;
			this->direction = UP_RIGHT;
		}
		break;
	case DOWN_RIGHT:
		if (this->position.y + SPEED + TEXTURE_SIZE / 2 < HIGH && this->position.x + SPEED + TEXTURE_SIZE / 2 < WIDTH)
		{
			this->position.x += SPEED;
			this->position.y += SPEED;
			this->direction = DOWN_RIGHT;
		}
		break;
	case DOWN_LEFT:
		if (this->position.y + SPEED + TEXTURE_SIZE / 2 < HIGH && this->position.x - SPEED - TEXTURE_SIZE / 2 > 0)
		{
			this->position.x -= SPEED;
			this->position.y += SPEED;
			this->direction = DOWN_LEFT;
		}
		break;
	case UP_LEFT:
		if (this->position.y - SPEED - TEXTURE_SIZE / 2 > 0 && this->position.x - SPEED - TEXTURE_SIZE / 2 > 0)
		{
			this->position.x -= SPEED;
			this->position.y -= SPEED;
			this->direction = UP_LEFT;
		}
		break;
	}
}
