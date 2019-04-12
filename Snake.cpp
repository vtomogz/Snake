#include "gameEngine.h"
#include <iostream>

using namespace std;

int main()
{
	gameEngine snake;
	snake.constructConsole(80, 40, 15, 15);
	snake.start();
	return 0;
}

