#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

using namespace std;
using namespace sf;

class MainMenu
{

public:

	bool isMenuActiveWindow;
	bool isMusicActive;

	Sprite* mems[3];
	Texture textures[3];

	Font font;

	Text newGameText;
	FloatRect newGameTextBox;

	Text settingsText;
	FloatRect settingsTextBox;

	Text exitText;
	FloatRect exitTextBox;

	Music music;

	void drawMenu(RenderWindow& window);						// rysowanie menu
	bool init();												// ustawienie pól w menu
};
