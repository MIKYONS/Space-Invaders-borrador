#include "Menu.hpp"

Menu::Menu(int userChoice) {
	this->userChoice = userChoice;
}

/****************************************************************
* Class Function: displayMenu()                                 *
* Date Created: 4/17/2021                                       *
* Date Last Modified: 4/17/2021                                 *
* Description: Displays the menu waiting for user input         *
* Input parameters: RenderWindow*, int&, int&, bool&            *
* Returns: Nothing                                              *
* Preconditions: RenderWindow*, int&, int&, bool&               *
* Postconditions: The menu has been displayed                   *
*****************************************************************/
void Menu::displayMenu(RenderWindow* window, int& score, int& lives, bool& indicator, bool& no_test)
{
	window->clear();
	bool x = true;

	Font font;
	font.loadFromFile("font.ttf");

	Text options, instructions;
	options.setFont(font);
	options.setCharacterSize(20);
	options.setPosition(0.f, 0.f);
	options.setFillColor(Color::White);
	options.setString("Bienvenido a Space Invader\n Proyecto de estructura de datos.\n1. jugar juego\n2. Testear casos \n3. Salir del juego");

	window->draw(options);
	window->display();
	if (Keyboard::isKeyPressed(Keyboard::Num1)) {
		userChoice = 1;
		indicator = false;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Num2)) {
		userChoice = 2;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Num3)) {
		indicator = false;
		userChoice = 3;
	}

	if (userChoice == 1) {
		score = 0;
		lives = 5;
	}

	else if (userChoice == 2)
		no_test = false;

	else if (userChoice == 3)
		window->close();
}

int Menu::getUserChoice() const {
	return userChoice;
}

void Menu::setUserChoice(int choice) {
	userChoice = choice;
}
