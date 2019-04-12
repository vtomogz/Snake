#include "gameEngine.h"

gameEngine::gameEngine() {
	//game engine default constructor
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE); 
	screenWidth = 120;
	screenHeight = 30;
	fontWidth = 8;
	fontHeight = 8;
	borderWidth = 2;
	lastInput = 0;
	jablko = new apple;
	waz = new snake;
	//run getUserInput() in separated thread
	std::thread([this] { this->getUserInput(); }).detach();
}

gameEngine::~gameEngine()
{
	delete jablko;
	delete waz;
	delete[] buffScreen;
}

void gameEngine::constructConsole(int _screenWidth, int _screenHeight, int _fontWidth, int _fontHeight) {
	//set console properties
	screenWidth = _screenWidth;
	screenHeight = _screenHeight;
	rectWindow = { 0,0,1,1 };
	SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);
	COORD coord = { (short)screenWidth, (short)screenHeight };
	SetConsoleScreenBufferSize(hConsole, coord);
	SetConsoleActiveScreenBuffer(hConsole);
	//set font properties
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = _fontWidth;
	cfi.dwFontSize.Y = _fontHeight;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	wcscpy_s(cfi.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(hConsole, false, &cfi);
	//set window size properties
	rectWindow = { 0, 0, (short)screenWidth - 1, (short)screenHeight - 1 };
	SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);
	//create screen buffer
	buffScreen = new CHAR_INFO[screenWidth*screenHeight];
	memset(buffScreen, 0, screenWidth*screenHeight*sizeof(CHAR_INFO));
	//set console window title
	SetConsoleTitle(L"Console Snake Game!");
}

void gameEngine::start() {
	//seting clock
	using clock = std::chrono::steady_clock;
	//getting present time
	auto next_frame = clock::now();	
	//game loop
	while (true) {
		//set this frame length to 100 miliseconds
		next_frame += std::chrono::milliseconds(100);
		//clean screen buffer
		//memset(buffScreen, 0, screenWidth*screenHeight*sizeof(CHAR_INFO));
		//draw game field
		drawGround();
		//if snake alive
		if (waz->alive) {
			//update snake and apple object
			waz->update(*this, lastInput, jablko->update(*this));
		}
		//draw apple
		jablko->drawApple(*this);
		//draw snake
		waz->drawSnake(*this);
		//if snake is dead
		if (!waz->alive)
			//draw death screen
			deathScreen();
		//if new game was initialized
		if (lastInput == 0) {
			//draw welcome screen
			welcomeScreen();
		}
		//display content of screen buffer
		WriteConsoleOutput(hConsole, buffScreen, { (short)screenWidth, (short)screenHeight }, { 0,0 }, &rectWindow);
		//if snake is dead
		if (!waz->alive) {
			//if user want to play more
			if (getUserDecision()) {
				//initialize new game
				waz->reset(*this);
			}
			//else end game loop
			else {
				return;
			}
		}
		//sleep for time that left
		std::this_thread::sleep_until(next_frame);
	}
}

gameEngine::apple::apple() {
	//apple default constructor
	posX = 15;
	posY = 15;
	col = COLOR::FG_DARK_RED;
}

bool gameEngine::apple::update(gameEngine &obj) {
	//check if apple position == snake head position (that means snake ate apple)
	if (posX == obj.waz->tail.back().x && posY == obj.waz->tail.back().y) {
		do {
			//pick new apple position values
			posX = rand() % (obj.screenWidth - 2 * obj.borderWidth) + obj.borderWidth;
			posY = rand() % (obj.screenHeight - 2 * obj.borderWidth) + obj.borderWidth;
		} while (!isPositionOK(obj)); //as long as it wont be correct
		//return true when snake ate apple
		return true;
	}
	//return false when snake hasnt ate apple
	return false;
}

void gameEngine::apple::drawApple(gameEngine &obj) {
	//drawning apple
	obj.draw(posX, posY, col);
}

void gameEngine::draw(int _posx, int _posy, int _col) {
	//setting attributes and char at specified position
	buffScreen[_posy*screenWidth + _posx].Attributes = _col;
	buffScreen[_posy*screenWidth + _posx].Char.UnicodeChar = 0x2588;
}

void gameEngine::drawGround() {
	//for number of rows
	for (int i = 0; i < screenHeight; i++) {
		//for number of cols
		for (int j = 0; j < screenWidth; j++) {
			//check if [i,j] is part of a border
			if((i<borderWidth||i>screenHeight-borderWidth-1)||(j<borderWidth || j>screenWidth - borderWidth-1)) 
				//if true draw grey square
				draw(j, i, COLOR::FG_DARK_GREY);
			else
				//else draw black square
				draw(j, i, COLOR::FG_BLACK);
		}
	}
}

gameEngine::snake::snake() {\
	//snake default constructor
	tail.push_back(pos(40, 20));
	col = COLOR::FG_DARK_GREEN;
	direction = 0;
	alive = true;
}

void gameEngine::getUserInput() {
	std::mutex mtx;
	while (true) {
		//if up arrow key pressed
		if (GetAsyncKeyState(VK_UP) < 0) {
			mtx.lock();
			//last input set to up
			lastInput = up;
			mtx.unlock();
		}
		//else if down arrow key pressed
		else if (GetAsyncKeyState(VK_DOWN) < 0) {
			mtx.lock();
			//last input set to down
			lastInput = down;
			mtx.unlock();
		}
		//esle if left arrow key pressed
		else if (GetAsyncKeyState(VK_LEFT) < 0) {
			mtx.lock();
			//last input set to low
			lastInput = left;
			mtx.unlock();
		}
		//else if right arrow key pressed
		else if (GetAsyncKeyState(VK_RIGHT) < 0) {
			mtx.lock();
			//last input set to right
			lastInput = right;
			mtx.unlock();
		}
	}
}

void gameEngine::snake::update(gameEngine &obj, int newDirection, bool ate) {
	//chech if user want to go in the opposite direction
	//if user want to go in the opposite direction ignore his call
	//if user want to go in any diffrent direction update direction value
	if (direction == up && newDirection != down) {
		direction = newDirection;
	}
	else if (direction == down && newDirection != up) {
		direction = newDirection;
	}
	else if (direction == left && newDirection != right) {
		direction = newDirection;
	}
	else if (direction == right && newDirection != left) {
		direction = newDirection;
	}
	else if (direction == 0) {
		direction = newDirection;
	}
	switch (direction) {
	case 0:
		break;
	//if user wants to go up
	case up:
		//check if user hit top border
		if (tail.back().y - 1 < 0 + obj.borderWidth) {
			//if hit,snake died
			alive = false;
		}
		//if snake still alive
		else {
			//add new snake tail segment
			tail.push_back(pos(tail.back().x, tail.back().y - 1));
			//check if snake ate apple
			if (!ate) {
				//if not, delete last segment of snake
				tail.erase(tail.begin());
			}
		}
		break;
	//if user wants to go down
	case down:
		if (tail.back().y + 1 > obj.screenHeight - obj.borderWidth - 1) {
			alive = false;
		}
		else {
			tail.push_back(pos(tail.back().x, tail.back().y + 1));
			if (!ate) {
				tail.erase(tail.begin());
			}
		}
		break;
	//if user wants to go left
	case left:
		if (tail.back().x - 1 < 0 + obj.borderWidth) {
			alive = false;
		}
		else {
			tail.push_back(pos(tail.back().x - 1, tail.back().y));
			if (!ate) {
				tail.erase(tail.begin());
			}
		}
		break;
	//if user wants to go right
	case right:
		if (tail.back().x + 1 > obj.screenWidth - obj.borderWidth - 1) {
			alive = false;
		}
		else {
			tail.push_back(pos(tail.back().x + 1, tail.back().y));
			if (!ate) {
				tail.erase(tail.begin());
			}
		}
		break;
	}
	//check if snake is not eating himself
	for (int i = 0; i < tail.size() - 1; i++) {
		//if snake head hit any tail segment
		if (tail[i].x == tail.back().x&&tail[i].y == tail.back().y) {
			//snake died
			alive = false;
		}
	}
}


void gameEngine::snake::drawSnake(gameEngine &obj) {
	//for tail size
	for (int i = 0; i < tail.size(); i++) {
		//draw segment of snake
		obj.draw(tail[i].x, tail[i].y, col);
	}
}

void gameEngine::deathScreen() {
	//setting death screen messages
	std::wstring message = L"YOU DIED.";
	//drawning messages to the screen buffer
	drawText((screenWidth - message.length()) / 2, screenHeight / 2, message);
	message = L"YOUR SCORE: " + std::to_wstring(waz->tail.size());
	drawText((screenWidth - message.length()) / 2, screenHeight / 2 + 1, message);
	message = L"PRESS SPACE TO TRY AGAIN";
	drawText((screenWidth - message.length()) / 2, screenHeight/2+2, message);
	message = L"OR ESC TO QUIT!";
	drawText((screenWidth - message.length()) / 2, screenHeight/2+3, message);
}

void gameEngine::drawText(int _posx,int _posy,std::wstring message) {
	//for message length
	for (int i = 0; i < message.length(); i++) {
		//set char color
		buffScreen[_posy*screenWidth + _posx + i].Attributes = COLOR::FG_WHITE;
		//draw next characters of message to the screen buffer
		buffScreen[_posy*screenWidth + _posx + i].Char.UnicodeChar=message[i];
	}
}

bool gameEngine::getUserDecision() {
	while (true) {
		//check if space key pressed
		if (GetAsyncKeyState(VK_SPACE) < 0)
			//user wants to play more
			return true;
		//check if escape key pressed
		if (GetAsyncKeyState(VK_ESCAPE) < 0)
			//user want to stop
			return false;
	}
}

void gameEngine::snake::reset(gameEngine &obj) {
	//setting snake properies to their initial value
	tail.resize(0);
	tail.push_back(pos(40, 20));
	col = COLOR::FG_DARK_GREEN;
	direction = 0;
	alive = true;
	obj.lastInput = 0;
}

void gameEngine::welcomeScreen() {
	//setting welcome messages
	std::wstring message = L"PRESS";
	//drawning them to the screen buffer
	drawText((screenWidth - message.length()) / 2, 3, message);
	message = L"← ↑ → ↓";
	drawText((screenWidth - message.length()) / 2, 4, message);
	message = L"TO START PLAYING";
	drawText((screenWidth - message.length()) / 2, 5, message);
}

bool gameEngine::apple::isPositionOK(gameEngine &obj) {
	//for tail size
	for (int i = 0; i < obj.waz->tail.size(); i++) {
		//check if apple possition == tail segment position
		if (obj.waz->tail[i].x == posX && obj.waz->tail[i].y == posY) {
			//apple possition == tail segment so apple position is wrong
			return false;
		}
	}
	//apple possition != tail segments so apple position OK
	return true;
}
