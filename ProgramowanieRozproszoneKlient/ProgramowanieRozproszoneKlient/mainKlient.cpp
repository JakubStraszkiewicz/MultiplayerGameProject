#include "Defines.hpp"
#include "MainMenu.hpp"
#include "Settings.hpp"
#include "Client.hpp"


int main(int argc, char *argv[])
{
	int port;
	if (argc == 4)
	{
		port = atoi(argv[3]);
	}
	else if (argc == 3)
	{
		port = 1500;
	}
	else
	{
		cout << "Nie podano wystarczajacej ilosci parametrow wejsciowych";
		return -1;
	}

	int maxClients = atoi(argv[1]);
	string serverIpAddress = argv[2];
	string nicknameString;

	int myIndex;
	int timer = 0;
	MainMenu mainMenu;
	Settings settings;
	Bullet *bulletToSend = new Bullet(Vector2f(-100, -100), -10, -1);
	vector<Bullet*> bullets;

	mainMenu.isMenuActiveWindow = true;
	settings.isSettingsActiveWindow = false;
	settings.isNicknameChangingEnabled = false;
	settings.initSettings();

	Mutex *bulletsMutex = new Mutex();
	Mutex *bulletsToSendMutex = new Mutex();
	Mutex *sendFlagMutex = new Mutex();
	Mutex *playerMutex = new Mutex();
	Mutex *gameStateMutex = new Mutex();

	RenderWindow window;
	Game game(&window, &mainMenu, maxClients, &myIndex, bulletToSend, &bullets, bulletsToSendMutex, sendFlagMutex, playerMutex, gameStateMutex);
	Client client(game.players, maxClients, serverIpAddress, &myIndex, bulletToSend, &bullets, bulletsMutex, bulletsToSendMutex, sendFlagMutex, playerMutex, gameStateMutex, &game.gameState);

	game.gameState = NO_GAME;

	if (client.connection(port) == -1)
	{
		return -1;
	}

	Thread threadPacketSender(&Client::sendPacket, &client);
	threadPacketSender.launch();

	Thread threadPacketsReceiver(&Client::receivePackets, &client);
	threadPacketsReceiver.launch();

	window.create(VideoMode(1280, 720), "Przetwarzanie rozproszone");
	if (mainMenu.init())
	{
		int test_iterator = 0;
		while (window.isOpen())
		{
			if (timer == 0)
				timer = GetTickCount();
			test_iterator++;
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					client.disconnection();
					window.close();
				}
				else if (event.type == Event::MouseButtonPressed)
				{
					gameStateMutex->lock();
					if (game.gameState == START_GAME)
						game.mouseControl(event, &timer);
					gameStateMutex->unlock();

					if (event.mouseButton.button == Mouse::Left)
					{
						Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);
						if (mainMenu.isMenuActiveWindow)
						{
							if (mainMenu.newGameTextBox.contains(mousePosition))
							{
								mainMenu.isMenuActiveWindow = false;
								settings.isSettingsActiveWindow = false;

								gameStateMutex->lock();
								game.gameState = START_GAME;
								gameStateMutex->unlock();
							}
							else if (mainMenu.settingsTextBox.contains(mousePosition))
							{
								mainMenu.isMenuActiveWindow = false;
								settings.isSettingsActiveWindow = true;
							}
							else if (mainMenu.exitTextBox.contains(mousePosition))
							{
								mainMenu.isMenuActiveWindow = false;
								settings.isSettingsActiveWindow = false;
								client.disconnection();
								window.close();
							}
						}
						else if (settings.isSettingsActiveWindow)
						{
							if (settings.nicknameValueTextBox.contains(mousePosition))
							{
								settings.isNicknameChangingEnabled = true;
							}
							else if (settings.backButtonBox.contains(mousePosition))
							{
								mainMenu.isMenuActiveWindow = true;
								settings.isSettingsActiveWindow = false;
								settings.isNicknameChangingEnabled = false;
							}
							else if (settings.okButtonBox.contains(mousePosition))
							{
								mainMenu.isMenuActiveWindow = true;
								settings.isSettingsActiveWindow = false;
								settings.isNicknameChangingEnabled = false;

								playerMutex->lock();
								game.setPlayerNickname(nicknameString);
								client.sendNewNickname();
								playerMutex->unlock();
							}
							else
							{
								settings.isNicknameChangingEnabled = false;
							}
						}
					}
				}
				else if (event.type == Event::MouseWheelMoved)
				{
					gameStateMutex->lock();
					if (game.gameState == START_GAME)
						game.mouseControl(event, &timer);
					gameStateMutex->unlock();
				}
				else if (event.type == Event::KeyPressed)
				{
					gameStateMutex->lock();
					if (game.gameState == START_GAME)
					{
						playerMutex->lock();
						game.keyboardControl(event, &timer); 
						playerMutex->unlock();

						sendFlagMutex->lock();
						client.sendFlag = true;
						sendFlagMutex->unlock();
					}
					gameStateMutex->unlock();
				}
				else if (event.type == Event::TextEntered)
				{
					if (settings.isNicknameChangingEnabled)
					{
						if (event.text.unicode <= 'z')
						{
							if (event.text.unicode != '\b')
							{
								if (nicknameString.length() < 20)
									nicknameString += (char)(event.text.unicode);
							}
							else
							{
								if (nicknameString.length() > 0)
									nicknameString.erase(nicknameString.end() - 1);
							}
							settings.nicknameValueText.setString(nicknameString);
						}
					}
				}
			}
			window.clear(Color::Black);
			if (mainMenu.isMenuActiveWindow)
			{
				mainMenu.drawMenu(window);
				if (!mainMenu.isMusicActive)
				{
					mainMenu.isMusicActive = true;
					(&mainMenu.music)->play();
				}
			}
			else if (settings.isSettingsActiveWindow)
			{
				settings.drawSettings(window);
			}
			else
			{
				mainMenu.isMusicActive = false;
				(&mainMenu.music)->stop();
			}

			gameStateMutex->lock();
			if (game.gameState == START_GAME)
			{
				gameStateMutex->unlock();

				game.drawMap();

				playerMutex->lock();
				game.drawPlayers();
				playerMutex->unlock();

				bulletsMutex->lock();
				game.drawBullets();
				bulletsMutex->unlock();
			}
			else if (game.gameState == END_GAME)
			{
				gameStateMutex->unlock();

				window.clear(sf::Color::Black);
				Text* playersNicknames = new Text[maxClients];
				Text* playersPoints = new Text[maxClients];
				Text score;
				Font font;

				font.loadFromFile("Czcionki/arial.ttf");
				score.setPosition(Vector2f((WIDTH / 2) - 30, 50));
				score.setString("Score");
				score.setFont(font);
				score.setScale(2, 2);
				window.draw(score);
				for (int i = 0; i < maxClients; i++)
				{
					playersNicknames[i].setPosition(Vector2f((WIDTH / 2) - game.players[i].nickname.size() * 10, 20 + 60 * (i + 2)));
					playersNicknames[i].setString(game.players[i].nickname);
					playersNicknames[i].setFont(font);
					playersPoints[i].setPosition(Vector2f(((WIDTH / 2) + game.players[i].nickname.size() * 20), 20 + 60 * (i + 2)));
					playersPoints[i].setString(to_string(game.players[i].points));
					playersPoints[i].setFont(font);
					window.draw(playersNicknames[i]);
					window.draw(playersPoints[i]);
				}

				delete[] playersNicknames;
				delete[] playersPoints;
			}
			else
			{
				gameStateMutex->unlock();
			}		
			window.display();
		}
	}
	else
		cout << "Init error";

	threadPacketSender.terminate();
	threadPacketsReceiver.terminate();


	delete bulletToSend;
	delete bulletsMutex;
	delete bulletsToSendMutex;
	delete sendFlagMutex;
	delete playerMutex;
	delete gameStateMutex;

	return 0;
}
