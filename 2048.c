/**
 * @file 2048.c
 * @author SpaceSkyNet (spaceskynet@outlook.com)
 * @brief 2048 Game for Console, available on Windows 10 and Linux.
 * @version 0.1
 * @date 2022-03-15
 * 
 * @copyright Copyright (c) 2022 SpaceSkyNet
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termio.h>
#include <unistd.h>
#endif

#define IS_UP_DOWN(d) (!((d)>>1))
#define IS_UP_LEFT(d) (!((d)&1))
#define WIN_NUMBER 2048
#define true 1
#define false 0

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define BASE "\033["
#define PADDING "        "

typedef _Bool bool; //C99
typedef int NUMTYPE;
typedef enum _direction{UP,DOWN,LEFT,RIGHT,NONE}DIRECTION;
typedef enum _colorCode{
    FG_BLACK = 30,
    FG_BLUE = 34,
    FG_CYAN = 36,
    FG_DARK_GRAY = 90,
    FG_DEFAULT = 39,
    FG_GREEN = 32,
    FG_LIGHT_BLUE = 94,
    FG_LIGHT_CYAN = 96,
    FG_LIGHT_GRAY = 37,
    FG_LIGHT_GREEN = 92,
    FG_LIGHT_MAGENTA = 95,
    FG_LIGHT_RED = 91,
    FG_LIGHT_YELLOW = 93,
    FG_MAGENTA = 35,
    FG_RED = 31,
    FG_WHITE = 97,
    FG_YELLOW = 33,
}COLOR_CODE;
const COLOR_CODE COLOR[11] = {FG_RED, FG_GREEN, FG_YELLOW, FG_BLUE, FG_MAGENTA, FG_CYAN,
                              FG_LIGHT_GRAY, FG_DARK_GRAY, FG_LIGHT_RED, FG_LIGHT_GREEN, FG_LIGHT_YELLOW};
int BEST_SCORE = 0;

void clearScreen();
void setConsoleCursorPosition();
void showConsoleCursor(bool);
void printTitle();
void printTable(NUMTYPE*, int, int, int);
void randGenerate(NUMTYPE*, int);
void printColoredNumber(NUMTYPE);
int Log2(NUMTYPE);
int moveTable(DIRECTION, NUMTYPE*, int);
int getPos(DIRECTION, int, int, int);

bool startGame(NUMTYPE*, int);
bool cmpTable(NUMTYPE*, NUMTYPE*, int);
bool getConfirm(NUMTYPE*, int, int, int);
bool confirmQuit(NUMTYPE*, int, int, int);
bool confirmRestart(NUMTYPE*, int, int, int);
bool confirmWinQuit(NUMTYPE*, int, int, int);
bool checkWin(NUMTYPE*, int);
bool checkGameOver(NUMTYPE*, int);
char getCh();
DIRECTION directionMap(char);

int main()
{
    srand(time(NULL));
    int size = 4;
    printTitle();
    scanf("%d", &size);
    if(size <= 1) size = 4;
    int n = size * size;
    NUMTYPE *table = (NUMTYPE*)malloc(n * sizeof(NUMTYPE));
    while(true)
    {
        memset(table, 0, n * sizeof(NUMTYPE) / sizeof(char));
        if(!startGame(table, size)) break;
    }
    printf("\nThanks for playing!\n");
    free(table);
#ifdef _WIN32
    system("pause");
#else
    getchar();
#endif
    return 0;
}
void init(NUMTYPE *t, int size)
{
    randGenerate(t, size);
    randGenerate(t, size);
}
bool startGame(NUMTYPE *t, int size)
{
    int n = size * size, score = 0, moves = 0;
    NUMTYPE *before = (NUMTYPE*)malloc(n * sizeof(NUMTYPE));
    bool isWin = false, isRestart = false;
    init(t, size);
    clearScreen();
    printTable(t, size, score, moves);
    while(true)
    {
        char input = getCh();
        if(input == 'q' && confirmQuit(t, size, score, moves)) break;
        if(input == 'r' && confirmRestart(t, size, score, moves)){isRestart = true;break;}

        DIRECTION direction = directionMap(input);
        if(direction == NONE) continue;

        memcpy(before, t, n * sizeof(NUMTYPE) / sizeof(char));
        score += moveTable(direction, t, size);
        if(score > BEST_SCORE) BEST_SCORE = score;
        if(!cmpTable(before, t, n)) randGenerate(t, size), moves++;
        printTable(t, size, score, moves);

        if(!isWin && checkWin(t, n))
        {
            printf("\nYou are win!! Congratulations!\n");
            isWin = true;
            if(confirmWinQuit(t, size, score, moves)) 
            {
                if(confirmRestart(t, size, score, moves)) isRestart = true;
                break;
            }
        }

        if(checkGameOver(t, size))
        {
            printf("\nGame Over! Have another try?\n");
            isRestart = confirmRestart(t, size, score, moves);
            break;
        }
    }
    free(before);
    return isRestart;
}
void randGenerate(NUMTYPE *t, int size)
{
    int n = size * size;
    int pos = rand() % n;
    while(t[pos]) pos = rand() % n;
    NUMTYPE val = 1 << (((rand() % 10) / 9) + 1); // 2: 9/10, 4: 1/10
    t[pos] = val;
}
bool getConfirm(NUMTYPE *t, int size, int score, int moves)
{
    while(true)
    {
        char c = getCh();
        if(c == 'Y' || c == 'y') return true;
        else if(c == 'N' || c == 'n')
        {
            showConsoleCursor(false);
            clearScreen();
            printTable(t, size, score, moves);
            return false;
        }
    }
}
bool confirmQuit(NUMTYPE *t, int size, int score, int moves)
{
    showConsoleCursor(true);
    printf("\nAre you sure to quit? [Y/n] ");
    return getConfirm(t, size, score, moves);
}
bool confirmRestart(NUMTYPE *t, int size, int score, int moves)
{
    showConsoleCursor(true);
    printf("\nDo you want to restart? [Y/n] ");
    return getConfirm(t, size, score, moves);
}
bool confirmWinQuit(NUMTYPE *t, int size, int score, int moves)
{
    showConsoleCursor(true);
    printf("Continue playing current game? [Y/n] ");
    return getConfirm(t, size, score, moves);
}


bool checkWin(NUMTYPE *t, int len)
{
    for(int i = 0; i < len; i++)
    {
        if(t[i] == WIN_NUMBER) return true;
    }
    return false;
}
bool checkGameOver(NUMTYPE *t, int size)
{
    int n = size * size;
    for(int i = 0; i < n; i++)
    {
        if(!t[i]) return false;
    }
    
    NUMTYPE *testTable = (NUMTYPE*)malloc(n * sizeof(NUMTYPE) / sizeof(char));
    for(int i = 0; i < 4; i++)
    {
        memcpy(testTable, t, n * sizeof(NUMTYPE) / sizeof(char));
        if(moveTable((DIRECTION)i, testTable, size))
        {
            free(testTable);
            return false;
        }
    }
    free(testTable);
    return true;
}

bool cmpTable(NUMTYPE *s, NUMTYPE *t, int len)
{
    for(int i = 0; i < len; i++)
    {
        if(s[i] != t[i]) return false;
    }
    return true;
}
int Log2(NUMTYPE x)
{
    int ans = 0;
    while(x >>= 1) ++ans;
    return ans;
}
void printColoredNumber(NUMTYPE x)
{
    if(!x) printf("│      ");
    else
    {
        int y = Log2(x);
        int index = y < 12 ? y - 1 : y % 12 - 1;
        printf("│ %s%s%dm%4d%s ", BOLD, BASE, (int)COLOR[index], x, RESET);
    }
}
void printTable(NUMTYPE *t, int size, int score, int moves)
{
    setConsoleCursorPosition();
    printf("┌───────────────────────────┐\n");
    printf("│ %sSCORE: %18d%s │\n", BOLD, score, RESET);
    printf("│ %sBEST SCORE: %13d%s │\n", BOLD, BEST_SCORE, RESET);
    printf("│ %sMOVES: %18d%s │\n", BOLD, moves, RESET);
    printf("└───────────────────────────┘\n");
    printf("┌");
    for(int i = 0; i < size - 1; i++) printf("──────┬");
    printf("──────┐\n");
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            int pos = i * size + j;
            printColoredNumber(t[pos]);
        }
        printf("│\n");
        if(i == size - 1) break;
        printf("├");
        for(int i = 0; i < size - 1; i++) printf("──────┼");
        printf("──────┤\n");
    }
    printf("└");
    for(int i = 0; i < size - 1; i++) printf("──────┴");
    printf("──────┘\n");
}
DIRECTION directionMap(char c)
{
    if(c == 'w' || c == 'W') return UP;
    if(c == 's' || c == 'S') return DOWN;
    if(c == 'a' || c == 'A') return LEFT;
    if(c == 'd' || c == 'D') return RIGHT;
    return NONE;
}

/**
 * UP   = 00, DOWN  = 01, >> 1 = 0 -> (j, i)
 * LEFT = 10, RIGHT = 11, >> 1 = 1 -> (i, j)
 */
inline int getPos(DIRECTION direction, int i, int j, int size)
{
    if(IS_UP_DOWN(direction)) return j * size + i;
    else return i * size + j;
}

/**
 * UP   = 00, LEFT  = 10, & 1 = 0 -> from 0 to size-1, step +1
 * DOWN = 01, RIGHT = 11, & 1 = 1 -> from size-1 to 0, step -1
 */
int moveTable(DIRECTION direction, NUMTYPE *t, int size)
{
    int start, end, step, score = 0;
    
    if(IS_UP_LEFT(direction)) start = 0, end = size - 1, step = 1;
    else start = size - 1, end = 0, step = -1;

    for(int i = 0; i < size; i++)
    {
        int p = start;
        for(int j = p + step; step > 0 ? j <= end : j >= end; j += step)
        {
            int ij = getPos(direction, i, j, size), ip = getPos(direction, i, p, size);
            if(!t[ij]) continue;
            if(t[ip] && t[ip] ^ t[ij])
            {
                p += step; t[getPos(direction, i, p, size)] = t[ij];
                if(p ^ j) t[ij] = 0;
            }
            else
            {
                if(!t[ip]) t[ip] = t[ij];
                else t[ip] <<= 1, score += t[ip], p += step;
                t[ij] = 0;
            }
        }
    }
    return score;
}

#ifndef _WIN32
void setBufferedInput(bool enable)
{
	static bool enabled = true;
	static struct termios OLD;
	struct termios NEW;

	if (enable && !enabled) 
    {
		// restore the former settings
		tcsetattr(STDIN_FILENO, TCSANOW, &OLD);
		// set the new state
		enabled = true;
	}
    else if (!enable && enabled) 
    {
		// get the terminal settings for standard input
		tcgetattr(STDIN_FILENO, &NEW);
		// we want to keep the old setting to restore them at the end
		OLD = NEW;
		// disable canonical mode (buffered i/o) and local echo
		NEW.c_lflag &= (~ICANON & ~ECHO);
		// set the new settings immediately
		tcsetattr(STDIN_FILENO, TCSANOW, &NEW);
		// set the new state
		enabled = false;
	}
}
#endif
char getCh()
{
#ifdef _WIN32
    return getch();
#else
    setBufferedInput(false);
    char c = getchar();
    setBufferedInput(true);
    return c;
#endif
}
void showConsoleCursor(bool show)
{
#ifdef _WIN32
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(consoleHandle, &cci);
    cci.bVisible = show; // show/hide cursor
    SetConsoleCursorInfo(consoleHandle, &cci);
#else
    printf(show ? "\033[?25h" : "\033[?25l"); // show/hide cursor
#endif
}
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif    
}
void setConsoleCursorPosition()
{
#ifdef _WIN32
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cur = {0, 0};
    showConsoleCursor(false);
    SetConsoleCursorPosition(consoleHandle, cur);
#else
    printf("\033[0m\033[2J\033c");
#endif
}

void printTitle()
{
    #ifdef _WIN32
    system("chcp 65001"); // UTF-8
    #endif
    clearScreen();
    printf(PADDING"-----------------------------------\n%s%dm", BASE, (int)FG_LIGHT_GREEN);
    printf(PADDING" █████    █████      ███    █████  \n");
    printf(PADDING"██   ██  ██   ██    ████   ██  ██  \n");
    printf(PADDING"█     █  █    ██    █ ██   ██   ██ \n");
    printf(PADDING"█     █  █     █   ██ ██   ██   ██ \n");
    printf(PADDING"██   ██  █     █   █  ██    █████  \n");
    printf(PADDING"███ ██   █     █  ██  ██   ███████ \n");
    printf(PADDING"   ██    █     █ ██   ██   █    ██ \n");
    printf(PADDING"  ██     █     █ ████████ ██     █ \n");
    printf(PADDING"███      █    ██      ██  ██     █ \n");
    printf(PADDING"███      ███ ███      ██   ██   ██ \n");
    printf(PADDING"███████   █████       ██    █████  %s\n", RESET);
    printf(PADDING"-------- By: SpaceSkyNet ----------\n\n");
    printf("Join the numbers and get to the %s%dm2048%s tile!\n", BASE, (int)FG_LIGHT_GREEN, RESET);
    printf("You can press %s%dm`q`%s to quit and press %s%dm`r`%s to restart!\n", BASE, (int)FG_LIGHT_RED, RESET, BASE, (int)FG_LIGHT_RED, RESET);
    printf("You can use %s%dm`WASD`%s to move in playing!\n", BASE, (int)FG_LIGHT_RED, RESET);
    printf("Now, please input the size of the game: ");
}
