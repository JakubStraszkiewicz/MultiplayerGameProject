#include "Settings.hpp"

Settings::Settings()
{
}

Settings::~Settings()
{
}

bool  Settings::initSettings()
{
	if (font.loadFromFile("Czcionki/arial.ttf"))
	{
		nicknameText.setFont(font);
		nicknameText.setString(L"Ustaw nick");
		nicknameText.setCharacterSize(60);
		nicknameText.setPosition(Vector2f(30, 50));

		nicknameValueText.setFont(font);
		nicknameValueText.setCharacterSize(60);
		nicknameValueText.setFillColor(Color::Blue);

		nicknameValueText.setPosition(Vector2f(200, 200));

		nicknameValueTextBox = nicknameValueText.getGlobalBounds();
		nicknameValueTextBox.width = 600;
		nicknameValueTextBox.height = 100;

		nicknameValueBackground.setSize(Vector2f(nicknameValueTextBox.width, nicknameValueTextBox.height));
		nicknameValueBackground.setPosition(Vector2f(200, 200));
		nicknameValueBackground.setFillColor(Color::White);

		backButton.setFont(font);
		backButton.setString(L"Wróæ");
		backButton.setCharacterSize(60);
		backButton.setPosition(Vector2f(30, 600));

		backButtonBox = backButton.getGlobalBounds();

		okButton.setFont(font);
		okButton.setString(L"OK");
		okButton.setCharacterSize(60);
		okButton.setPosition(Vector2f(500, 600));

		okButtonBox = okButton.getGlobalBounds();
		return true;
	}
	else
	{
		cout << "Font error in Settings";
		return false;
	}
}

void Settings::drawSettings(RenderWindow & window)
{
	window.draw(nicknameText);
	window.draw(nicknameValueBackground);
	window.draw(nicknameValueText);
	window.draw(backButton);
	window.draw(okButton);
}





