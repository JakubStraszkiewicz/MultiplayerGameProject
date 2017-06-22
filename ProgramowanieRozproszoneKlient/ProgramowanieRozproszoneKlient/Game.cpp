#include "Game.hpp"

Game::Game(RenderWindow* window, MainMenu* menu, int maxPlayers, int *myIndex, Bullet *bulletToSend, vector<Bullet*> *bullets, Mutex *bulletsToSendMutex, Mutex* sendFlagMutex, Mutex *playerMutex, Mutex *gameStateMutex)
{
	this->bulletsToSendMutex = bulletsToSendMutex;
	this->gameStateMutex = gameStateMutex;
	this->sendFlagMutex = sendFlagMutex;
	this->bullets = bullets;
	this->bulletToSend = bulletToSend;
	this->myIndex = myIndex;
	this->window = window;
	this->menu = menu;
	this->maxPlayers = maxPlayers;
	this->playerMutex = playerMutex;
	this->font.loadFromFile("Czcionki/arial.ttf");
	this->players = new Player[maxPlayers];
	this->loadMap();

	srand(time(NULL));
}

Game::~Game()
{
	delete[] players;
}

void Game::loadMap()
{
	gameMap = new Sprite**[WIDTH / GRAPHIC_SIZE];
	for (int i = 0; i < WIDTH / GRAPHIC_SIZE; i++)
	{
		gameMap[i] = new Sprite*[HIGH / GRAPHIC_SIZE];
	}

	this->grassTexture.loadFromFile("Image/grass2.jpg");

	for (int i = 0; i < WIDTH / GRAPHIC_SIZE; i++)
	{
		for (int j = 0; j < HIGH / GRAPHIC_SIZE; j++)
		{
			gameMap[i][j] = new Sprite(this->grassTexture);
			this->gameMap[i][j]->setPosition(i*GRAPHIC_SIZE, j*GRAPHIC_SIZE);
		}
	}
}

void Game::drawPlayers()
{
	for (int i = 0; i < maxPlayers; i++)
	{
		if (players[i].isAlive)
		{
			players[i].sprite->setPosition(players[i].position.x, players[i].position.y);
			window->draw(*players[i].sprite);
			players[i].sprite->setRotation(45.0*players[i].direction);

			players[i].healthBar->setFillColor(Color::Red);
			players[i].healthBar->setPosition(players[i].position.x - TEXTURE_SIZE / 2, players[i].position.y + 25);
			players[i].healthBar->setSize(Vector2f((players[i].actualHealth / 100.0)*TEXTURE_SIZE, 5));
			window->draw(*(players[i].healthBar));

			players[i].nicknameLabel->setString(players[i].nickname);
			players[i].nicknameLabel->setPosition(players[i].position.x - TEXTURE_SIZE / 2, players[i].position.y - TEXTURE_SIZE);
			players[i].nicknameLabel->setScale(0.5, 0.5);
			players[i].nicknameLabel->setFont(font);
			window->draw(*(players[i].nicknameLabel));
		}
	}
}
void Game::drawBullets()
{
	for (int i = 0; i < bullets->size(); i++)
	{
		(*bullets)[i]->sprite->setPosition((*bullets)[i]->position.x, (*bullets)[i]->position.y);
		window->draw(*(*bullets)[i]->sprite);
	}
}

void Game::drawMap()
{
	for (int i = 0; i < WIDTH / GRAPHIC_SIZE; i++)
	{
		for (int j = 0; j < HIGH / GRAPHIC_SIZE; j++)
		{
			window->draw(*gameMap[i][j]);
		}
	}
}

void Game::keyboardControl(Event event, int* timer)
{
	switch (event.key.code)
	{
	case Keyboard::Key::Numpad8:
	case Keyboard::Key::Up:
		this->players[*myIndex].move(UP);
		break;
	case  Keyboard::Key::Numpad2:
	case Keyboard::Key::Down:
		this->players[*myIndex].move(DOWN);
		break;
	case Keyboard::Key::Numpad4:
	case Keyboard::Key::Left:
		this->players[*myIndex].move(LEFT);
		break;
	case Keyboard::Key::Numpad6:
	case Keyboard::Key::Right:
		this->players[*myIndex].move(RIGHT);
		break;
	case Keyboard::Key::Numpad7:
		this->players[*myIndex].move(UP_LEFT);
		break;
	case Keyboard::Key::Numpad9:
		this->players[*myIndex].move(UP_RIGHT);
		break;
	case Keyboard::Key::Numpad1:
		this->players[*myIndex].move(DOWN_LEFT);
		break;
	case Keyboard::Key::Numpad3:
		this->players[*myIndex].move(DOWN_RIGHT);
		break;
	case Keyboard::Key::Num1:
		this->players[*myIndex].weaponType = (PISTOL);
		break;
	case Keyboard::Key::Num2:
		this->players[*myIndex].weaponType = (SHOTGUN);
		break;
	case Keyboard::Key::Num3:
		this->players[*myIndex].weaponType = (RIFLE);
		break;
	case Keyboard::Key::Num4:
		this->players[*myIndex].weaponType = (SHOTGUN2);
		break;

	}
	if (GetTickCount() - *timer >= SHOOTINTERVAL)
	{
		if (event.key.code == Keyboard::Key::Space && this->players[*myIndex].weaponType == PISTOL)
		{
			this->pistolShoot();
		}
		*timer = 0;
	}

	gameStateMutex->lock();
	if (event.key.code == Keyboard::Key::Escape && gameState == START_GAME)
	{
		menu->isMenuActiveWindow = true;
		gameState = NO_GAME;
	}
	else if (event.key.code == Keyboard::Key::Escape && gameState == NO_GAME)
	{
		menu->isMenuActiveWindow = false;
		gameState = START_GAME;
	}
	gameStateMutex->unlock();
}

void Game::mouseControl(Event event, int* timer)
{
	if (GetTickCount() - *timer >= SHOOTINTERVAL)
	{
		if (event.mouseButton.button == Mouse::Left && this->players[*myIndex].weaponType == PISTOL)
			this->pistolShoot();
		*timer = 0;
	}
}

void Game::pistolShoot()
{
	int x = this->players[*myIndex].position.x, y = this->players[*myIndex].position.y;
	int direction = this->players[*myIndex].direction;

	bulletsToSendMutex->lock();
	if (direction == RIGHT)
	{
		bulletToSend->position = Vector2f(x + BULLETDISTANCESMALL, y);
		bulletToSend->direction = direction;
	}
	if (direction == LEFT)
	{
		bulletToSend->position = Vector2f(x - BULLETDISTANCESMALL, y);
		bulletToSend->direction = direction;
	}
	if (direction == UP)
	{
		bulletToSend->position = Vector2f(x, y - BULLETDISTANCESMALL);
		bulletToSend->direction = direction;
	}
	if (direction == DOWN)
	{
		bulletToSend->position = Vector2f(x, y + BULLETDISTANCESMALL);
		bulletToSend->direction = direction;
	}
	if (direction == UP_RIGHT)
	{
		bulletToSend->position = Vector2f(x + BULLETDISTANCESMALL, y - BULLETDISTANCESMALL);
		bulletToSend->direction = direction;
	}
	if (direction == UP_LEFT)
	{
		bulletToSend->position = Vector2f(x - BULLETDISTANCESMALL, y - BULLETDISTANCESMALL);
		bulletToSend->direction = direction;
	}
	if (direction == DOWN_RIGHT)
	{
		bulletToSend->position = Vector2f(x + BULLETDISTANCESMALL, y + BULLETDISTANCESMALL);
		bulletToSend->direction = direction;
	}
	if (direction == DOWN_LEFT)
	{
		bulletToSend->position = Vector2f(x - BULLETDISTANCESMALL, y + BULLETDISTANCESMALL);
		bulletToSend->direction = direction;
	}
	bulletsToSendMutex->unlock();
}

void Game::setPlayerNickname(string newNickname)
{
	this->players[*myIndex].nickname = newNickname;
}