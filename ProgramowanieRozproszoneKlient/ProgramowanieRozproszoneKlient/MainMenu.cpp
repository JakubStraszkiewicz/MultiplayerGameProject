#include "MainMenu.hpp"

bool MainMenu::init()
{
	if (font.loadFromFile("Czcionki/arial.ttf"))
	{
		newGameText.setFont(font);
		newGameText.setString(L"Graj");
		newGameText.setCharacterSize(60);
		
		newGameText.setPosition(Vector2f(30, 50));
		newGameTextBox = newGameText.getGlobalBounds();

		settingsText.setFont(font);
		settingsText.setString(L"Ustawienia");
		settingsText.setCharacterSize(60);
	
		settingsText.setPosition(Vector2f(30, 100));
		settingsTextBox = settingsText.getGlobalBounds();

		exitText.setFont(font);
		exitText.setString(L"Wyjœcie");
		exitText.setCharacterSize(60);

		exitText.setPosition(Vector2f(30, 150));
		exitTextBox = exitText.getGlobalBounds();

		music.openFromFile("Sounds/Theme.ogg");
		music.setLoop(true);
		isMusicActive = true;
		music.play();

		textures[0].loadFromFile("Image/kappa.png");
		mems[0] = new Sprite(textures[0]);
		mems[0]->setPosition(Vector2f(300, 100));

		textures[1].loadFromFile("Image/trollface.png");
		mems[1] = new Sprite(textures[1]);
		mems[1]->setPosition(Vector2f(400, 300));

		textures[2].loadFromFile("Image/notbad.png");
		mems[2] = new Sprite(textures[2]);
		mems[2]->setPosition(Vector2f(800, 0));
		
		return true;
	}
	else
	{
		cout << "Font error in Main Menu";
		return false;
	}
}
void MainMenu::drawMenu(RenderWindow& window)
{	
		window.draw(newGameText);		
		window.draw(settingsText);
		window.draw(exitText);
		window.draw(*mems[0]);
		window.draw(*mems[1]);
		window.draw(*mems[2]);
}
