#pragma once
#include <iostream>
#include <windows.h>
#include <vector>
#include <mutex>
#include <string>
#include <thread>
#include <chrono>

enum directions {
	up = 1,
	down = 2,
	left = 3,
	right = 4
};

namespace COLOR {
	enum COLORS {
		FG_BLACK = 0x0000,
		FG_DARK_BLUE = 0x0001,
		FG_DARK_GREEN = 0x0002,
		FG_DARK_CYAN = 0x0003,
		FG_DARK_RED = 0x0004,
		FG_DARK_MAGENTA = 0x0005,
		FG_DARK_YELLOW = 0x0006,
		FG_GREY = 0x0007,
		FG_DARK_GREY = 0x0008,
		FG_BLUE = 0x0009,
		FG_GREEN = 0x000A,
		FG_CYAN = 0x000B,
		FG_RED = 0x000C,
		FG_MAGENTA = 0x000D,
		FG_YELLOW = 0x000E,
		FG_WHITE = 0x000F,
	};
}

//struct that snake object use to store snkae segments positions
struct pos {
	int x = 0;
	int y = 0;
	pos() {
		x = 0;
		y = 0;
	}
	pos(int _x, int _y) {
		x = _x;
		y = _y;
	}
};

class gameEngine
{
private:
	//number of cols
	int screenWidth;
	//number of rows
	int screenHeight;
	//width of cell
	int fontWidth;
	//height of cell
	int fontHeight; 
	//width of border
	int borderWidth;
	//screen buffer
	CHAR_INFO *buffScreen;
	//console handle
	HANDLE hConsole;
	//window sizing
	SMALL_RECT rectWindow;
	//setting color of specific cell
	void draw(int, int, int);
	//drawning game field with border
	void drawGround();
	//drawning text on the specific position
	void drawText(int,int,std::wstring);
	//get snake controls from user
	void getUserInput();
	//get if player wants to play more or not
	bool getUserDecision();
	//drawning death screen
	void deathScreen();
	//drawning welcome screen
	void welcomeScreen();
	//last pressed kry by user(only snake controls)
	int lastInput;
	//apple class
	class apple {
	friend class gameEngine;
	private:
		//apple X position
		int posX;
		//apple Y position
		int posY;
		//apple color
		int col;
	public:
		apple();
		//check if apple position is not equal to snake tail position
		bool isPositionOK(gameEngine &obj);
		//updating and calculating new apple values
		bool update(gameEngine&);
		//drawning apple to the screen buffer
		void drawApple(gameEngine&);

	};
	//apple object
	apple *jablko;
	//snake class
	class snake {
	friend class gameEngine;
	private:
		//true if alive and flase if not
		bool alive;
		//vector that contains snake segments positions
		std::vector<pos> tail;
		//direction that snake is heading
		int direction;
		//color of snake
		int col;
	public:
		snake();
		//updating and calculating new snake values
		void update(gameEngine&,int,bool);
		//drawning snake to the screen buffer
		void drawSnake(gameEngine&);
		//seting snake variables to their initial values
		void reset(gameEngine&);
	};
	//snake object
	snake *waz;
public:
	//construct console with
	void constructConsole(int, int, int, int);
	//game loop
	void start();
	gameEngine();
	~gameEngine();
};

