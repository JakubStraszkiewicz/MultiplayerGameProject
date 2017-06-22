#pragma once

#include "Defines.hpp"

class Settings
{
public:

	bool isSettingsActiveWindow;
	bool isNicknameChangingEnabled;

	Font font;
	Text nicknameText;
	Text nicknameValueText;						// tekst pola tekstowego
	FloatRect nicknameValueTextBox;				// obszar kolizji pola tekstowego do ustawiania nicka
	RectangleShape nicknameValueBackground;		// tlo pod pole tekstowe
	Text backButton;							// tekst przycisku wróæ
	FloatRect backButtonBox;					// obszar kolizji przycisku wróæ
	Text okButton;								// tekst przycisku OK
	FloatRect okButtonBox;						// obszar kolizji przycisku OK

	Settings();
	~Settings();

	bool initSettings();													// ustawienie wartoœci pól w ustawieniach
	void drawSettings(RenderWindow& window);								// narysowanie pól ustawieñ	
};

