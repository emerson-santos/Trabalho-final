#include <time.h>
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <process.h>
#include <conio.h>

#define WALL -3
#define MISSILE -2
#define SHIP -1
#define NOTHING 0

#define MAX 100
#define WIDTH 100
#define HEIGHT 30
#define SPEED_BOUND 3
#define SPEED 2


#define NEUTRAL 0
#define RIGHT 1
#define UP 2
#define LEFT 3
#define DOWN 4
#define EXIT -1
static int dx[5] = {0, 1, 0, -1, 0 };
static int dy[5] = {0, 0, -1, 0, 1 };

using namespace std;

int input = NEUTRAL;
int item = NOTHING;

void clearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;


	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;


	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;


	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;


	SetConsoleCursorPosition(hStdOut, homeCoords);
}

void gotoxy(int column, int row)
{
	HANDLE hStdOut;
	COORD coord;

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	coord.X = column;
	coord.Y = row;
	SetConsoleCursorPosition(hStdOut, coord);
}

struct Coordinate
{
	int x, y;
};

class game
{
private:
	int ground[MAX][MAX];
	int direction;
	Coordinate ship;
	Coordinate missile;
	Coordinate speedMissile;
	int counter;
public:
	void initGround();
	void initShip();
	void initMissile();
	void firstDraw();
	void updateShip(int delay);
	void updateMissile();
	int getCounter();
};

void game::initShip()
{
	ship.x = WIDTH / 2;
	ship.y = HEIGHT / 2;
	counter=0;
	direction = input;

	ground[ship.y][ship.x] = SHIP;
}

void game::updateShip(int delay)
{
	Coordinate prev;
	prev.x = ship.x;
	prev.y = ship.y;

	if (input != EXIT)
		direction = input;

	ship.x = prev.x+dx[direction];
	ship.y = prev.y+dy[direction];

	if (ground[ship.y][ship.x] < SHIP)
	{
		item = -1;
		return;
	}

	ground[prev.y][prev.x] = NOTHING;
    gotoxy(prev.x, prev.y);
    cout << " ";
    ground[prev.y+1][prev.x] = NOTHING;
    gotoxy(prev.x, prev.y+1);
    cout << " ";
    ground[prev.y][prev.x+1] = NOTHING;
    gotoxy(prev.x+1, prev.y);
    cout << " ";
    ground[prev.y-1][prev.x] = NOTHING;
    gotoxy(prev.x, prev.y-1);
    cout << " ";
    ground[prev.y][prev.x-1] = NOTHING;
    gotoxy(prev.x-1, prev.y);
    cout << " ";

	gotoxy(ship.x, ship.y);
	cout << "O";
    ground[ship.y][ship.x] = SHIP;
    gotoxy(ship.x, ship.y+1);
	cout << "O";
    ground[ship.y+1][ship.x] = SHIP;
    gotoxy(ship.x+1, ship.y);
	cout << "O";
    ground[ship.y][ship.x+1] = SHIP;
    gotoxy(ship.x, ship.y-1);
	cout << "O";
    ground[ship.y-1][ship.x] = SHIP;
    gotoxy(ship.x-1, ship.y);
	cout << "O";
    ground[ship.y][ship.x-1] = SHIP;

	Sleep(delay);
	counter++;

	return;
}

void game::initMissile()
{
	missile.x = 0;
	missile.y = 0;
	speedMissile.x=0;
	speedMissile.x=0;

	ground[missile.y][missile.x] = MISSILE;
}

void game::updateMissile()
{
	Coordinate prev;
	Coordinate accel;
	prev.x = missile.x;
	prev.y = missile.y;

    accel.x=(ship.x-missile.x)/5;
    accel.y=(ship.y-missile.y)/5;
    speedMissile.x+=accel.x;
    if(speedMissile.x>SPEED_BOUND){
        speedMissile.x=SPEED_BOUND;
    }else if(speedMissile.x<(-SPEED_BOUND)){
        speedMissile.x=-SPEED_BOUND;
    }

	speedMissile.y+=accel.y;
    if(speedMissile.y>SPEED_BOUND){
        speedMissile.y=SPEED_BOUND;
    }else if(speedMissile.y<(-SPEED_BOUND)){
        speedMissile.y=-SPEED_BOUND;
    }

	missile.x=prev.x+speedMissile.x;
	missile.y=prev.y+speedMissile.y;


	if (ground[missile.y][missile.x] < SHIP)
	{
		missile.x = 0;
	    missile.y = 0;
    	speedMissile.x=0;
    	speedMissile.x=0;
    	counter+=100;
	}else if (ground[missile.y][missile.x] == SHIP)
	{
        item = -1;
		return;
	}

	ground[prev.y][prev.x] = NOTHING;
    gotoxy(prev.x, prev.y);
    cout << " ";


	gotoxy(missile.x, missile.y);
	cout << "+";

    ground[missile.y][missile.x] = MISSILE;

	return;
}

void game::initGround()
{
	int i, j;
	for (i = 0; i < MAX; i++)
		for (j = 0; j < MAX; j++)
			ground[i][j] = 0;

	for (i = 0; i <= WIDTH + 1; i++)
	{
		//top bottom wall
		ground[0][i] = WALL;
		ground[HEIGHT + 1][i] = WALL;
	}
	for (i = 0; i <= HEIGHT + 1; i++)
	{
		//right left wall
		ground[i][0] = WALL;
		ground[i][WIDTH + 1] = WALL;
	}
}

void game::firstDraw()
{
	clearScreen();
	int i, j;
	for (i = 0; i <= HEIGHT + 1; i++)
	{
		for (j = 0; j <= WIDTH + 1; j++)
		{
			switch (ground[i][j])
			{
			case NOTHING:
				cout << " "; break;
			case WALL:
				if ((i == 0 && j == 0)
					|| (i == 0 && j == WIDTH + 1)
					|| (i == HEIGHT + 1 && j == 0)
					|| (i == HEIGHT + 1 && j == WIDTH + 1))
					cout << "+";
				else
					if (j == 0 || j == WIDTH + 1)
						cout << "|";
					else
						cout << "-";
				break;
			case MISSILE:
				cout << "+";
				break;
			case SHIP:
				cout << "O";
				break;
			default:
				cout << "\u2022";
			}
		}
		cout << endl;
	}
}

int game::getCounter()
{
	return counter;
}

void userInput(void* id)
{
	do
	{
		int c = _getch();
		switch (c)
		{
		case 'W': case 'w': input = UP   ; break;
		case 'S': case 's': input = DOWN ; break;
		case 'D': case 'd': input = RIGHT; break;
		case 'A': case 'a': input = LEFT ; break;
		case 27:	    input = EXIT ; break;
		default: input = NEUTRAL;
		}
	} while (input != EXIT && item >= 0);

	_endthread();
	return;
}

int main()
{
	int delay = 100;
	srand(time(NULL));
	game jogo;
	jogo.initGround();
	jogo.initShip();
	jogo.initMissile();
	jogo.firstDraw();
	_beginthread(userInput, 0, (void*)0);

	do
	{
		jogo.updateShip(delay);
		jogo.updateMissile();
	} while (item >= 0 && input != EXIT);

	gotoxy(WIDTH / 2 - 5, HEIGHT / 2 - 2);
	cout << "GAME OVER";
	gotoxy(WIDTH / 2 - 8, HEIGHT / 2 + 2);
	cout << "Your score is " << jogo.getCounter() << "!" << endl;
	gotoxy(WIDTH / 2, HEIGHT / 2);

	_getch();
	return 0;
}
