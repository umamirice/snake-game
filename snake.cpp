#include <sys/ioctl.h>
#include <termios.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>

using namespace std;

const int width = 20, height = 20;

int x,y;
int fruitX, fruitY;
int score;

int nTail;
int tailX[100], tailY[100];

bool gameOver;

enum eDirection {STOP=0, LEFT, RIGHT, UP, DOWN};
eDirection dir=STOP;

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

void enable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(0, TCSANOW, &term);
}

void disable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

bool kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

void changeFruit()
{
    srand(time(0));
    score += 100;
    fruitX = rand() % width;
    fruitY = rand() % height;
    nTail++;
}

void setup()
{
    nTail = 0;
    gameOver = false;
    score = 0;
    x = width/2;
    y = height/2;
    fruitX = rand()%width;
    fruitY = rand()%height;
    if(fruitX == 0) fruitX++;
    if(fruitY == 0) fruitY++;
}

void draw()
{
    cout << endl;
    cout << " ";
    for (int i = 0; i < width+2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0)
                cout << " #";
            if (i == y && j == x)
                cout << "O";
            else if (i == fruitY && j == fruitX)
                cout << "F";
            else
            {
                bool print = false;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        cout << "o";
                        print = true;
                    }
                }
                if (!print)
                    cout << " ";
            }

            if (j == width - 1)
                cout << "#";
        }
        cout << endl;
    }

    cout << " ";
    for (int i = 0; i < width+2; i++)
        cout << "#";
    cout << endl;
    cout << " Score:" << score << endl;
    usleep(1000000 * 0.1);
}

void input()
{

    enable_raw_mode();
    if(kbhit())
    {
        switch(getch())
        {
        case 'w':
            dir = UP;
            break;

        case 's':
            dir = DOWN;
            break;

        case 'a':
            dir = LEFT;
            break;

        case 'd':
            dir = RIGHT;
            break;

        case 'x':
            gameOver = 1;
        }
    }
    disable_raw_mode();
    tcflush(0, TCIFLUSH);
}

void logic()
{

    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch(dir)
    {
    case UP:
        y--;
        break;

    case DOWN:
        y++;
        break;

    case LEFT:
        x--;
        break;

    case RIGHT:
        x++;
        break;
    }

    if (x > width || x < 0 || y > height || y < 0)
        gameOver = true;

    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    if (x == fruitX && y == fruitY)
    {
        changeFruit();
    }
}


int main()
{
    setup();
    while(!gameOver)
    {
        draw();
        input();
        logic();
    }
    system("clear");
    cout <<'\t'<< "GAME OVER" << endl <<'\t'<< "Your score: " << score << endl;
    return 0;
}
