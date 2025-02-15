#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <ctime>
#include <sstream>
#include <windows.h>
using namespace std;

const int boardWidth = 50;
const int boardHeight = 20;
int currentScore = 0;
int playerX, playerY, fruitX, fruitY;
bool isGameOver;
enum Direction { HALT = 0, LEFT, RIGHT, UP, DOWN };
Direction playerDirection;
int tailPosX[100], tailPosY[100];
int tailLength = 0;

struct Gamer {
    string username;
    int topScore;
};

void hideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void initializeGame()
{
    isGameOver = false;
    playerDirection = HALT;
    playerX = boardWidth / 2;
    playerY = boardHeight / 2;
    fruitX = rand() % boardWidth;
    fruitY = rand() % boardHeight;
    currentScore = 0;
    tailLength = 0;
}

void renderBoard()
{
    COORD cursor_Position = { 0, 5 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor_Position);

    for (int i = 0; i < boardWidth + 2; i++)
        cout << "X";
    cout << endl;

    for (int i = 0; i < boardHeight; i++)
    {
        cout << "X";
        for (int j = 0; j < boardWidth; j++)
        {
            if (i == playerY && j == playerX)
                cout << "O";
            else if (i == fruitY && j == fruitX)
                cout << "*";
            else
            {
                bool isTailPart = false;
                for (int k = 0; k < tailLength; k++)
                {
                    if (tailPosX[k] == j && tailPosY[k] == i)
                    {
                        cout << "o";
                        isTailPart = true;
                    }
                }
                if (!isTailPart)
                    cout << " ";
            }
        }
        cout << "X";
        cout << endl;
    }
    for (int i = 0; i < boardWidth + 2; i++)
        cout << "X";
    cout << endl;
    cout << "Score: " << currentScore << endl;
}

void getInput()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'a':
            playerDirection = LEFT;
            break;
        case 'd':
            playerDirection = RIGHT;
            break;
        case 'w':
            playerDirection = UP;
            break;
        case 's':
            playerDirection = DOWN;
            break;
        case 'l':
            isGameOver = true;
            break;
        }
    }
}

void updateGameLogic()
{
    int previousX = tailPosX[0];
    int previousY = tailPosY[0];
    int previous2X, previous2Y;
    tailPosX[0] = playerX;
    tailPosY[0] = playerY;
    for (int i = 1; i < tailLength; i++)
    {
        previous2X = tailPosX[i];
        previous2Y = tailPosY[i];
        tailPosX[i] = previousX;
        tailPosY[i] = previousY;
        previousX = previous2X;
        previousY = previous2Y;
    }

    switch (playerDirection)
    {
    case LEFT:
        playerX--;
        break;
    case RIGHT:
        playerX++;
        break;
    case UP:
        playerY--;
        break;
    case DOWN:
        playerY++;
        break;
    }
    if (playerX >= boardWidth) playerX = 0; else if (playerX < 0) playerX = boardWidth - 1;
    if (playerY >= boardHeight) playerY = 0; else if (playerY < 0) playerY = boardHeight - 1;

    for (int i = 0; i < tailLength; i++)
    {
        if (tailPosX[i] == playerX && tailPosY[i] == playerY)
            isGameOver = true;
    }
    if (playerX == fruitX && playerY == fruitY)
    {
        currentScore += 10;
        fruitX = rand() % boardWidth;
        fruitY = rand() % boardHeight;
        tailLength++;
    }
}

Gamer getGamerData(const string& gamerName, Gamer gamers[], int& numGamers, int& maxGameScore)
{
    Gamer currentGamer;
    bool gamerFound = false;
    string filename = "game_scores.txt";
    ifstream file(filename, ios::in);
    numGamers = 0;
    maxGameScore = 0;

    if (file.is_open())
    {
        string line;
        bool isFirstLine = true;
        while (getline(file, line))
        {
            stringstream ss(line);
            string name;
            int highestScore;
            if (isFirstLine)
            {
                ss >> name >> maxGameScore;
                isFirstLine = false;
                continue;
            }
            if (ss >> name >> highestScore)
            {
                gamers[numGamers].username = name;
                gamers[numGamers].topScore = highestScore;
                numGamers++;

                if (name == gamerName)
                {
                    currentGamer.username = name;
                    currentGamer.topScore = highestScore;
                    gamerFound = true;
                }
            }
        }
        file.close();
    }
    else
    {
        cerr << "Failed to open the file for reading." << endl;
    }

    if (!gamerFound)
    {
        currentGamer.username = gamerName;
        currentGamer.topScore = 0;
        cout << "No existing data for " << gamerName << ". Starting a new game." << endl;
    }

    return currentGamer;
}

void saveGamerData(const Gamer gamers[], int numGamers, int maxGameScore)
{
    string filename = "game_scores.txt";
    ofstream file(filename, ios::out);

    if (file.is_open())
    {
        file << "MaxGameScore " << maxGameScore << endl;
        for (int i = 0; i < numGamers; ++i)
        {
            file << gamers[i].username << " " << gamers[i].topScore << endl;
        }
        file.close();
        cout << "All gamer data saved successfully." << endl;
    }
    else
    {
        cout << "Failed to open the file for writing." << endl;
    }
}

int main()
{
    hideCursor();
    srand(time(NULL));

    string gamerName;
    cout << "Enter your name: ";
    getline(cin, gamerName);

    Gamer gamers[100];
    int numGamers;
    int maxGameScore;
    Gamer currentGamer = getGamerData(gamerName, gamers, numGamers, maxGameScore);
    cout << "\t\t\tWelcome.., " << currentGamer.username << "! Your highest score is: " << currentGamer.topScore << endl;
    cout << "\t\t\tThe highest score among all players is: " << maxGameScore << endl;

    while (true)
    {
        initializeGame();
        while (!isGameOver)
        {
            renderBoard();
            getInput();
            updateGameLogic();
            int sleepDuration = max(1, 100 - (currentScore / 10));
            Sleep(sleepDuration);
        }

        if (currentScore > currentGamer.topScore)
        {
            currentGamer.topScore = currentScore;
            bool gamerUpdated = false;
            for (int i = 0; i < numGamers; ++i)
            {
                if (gamers[i].username == currentGamer.username)
                {
                    gamers[i].topScore = currentGamer.topScore;
                    gamerUpdated = true;
                    break;
                }
            }

            if (!gamerUpdated)
            {
                gamers[numGamers] = currentGamer;
                numGamers++;
            }
        }

        if (currentScore > maxGameScore)
        {
            maxGameScore = currentScore;
            cout << "Congratulations! You have the highest score among all players now.\n";
        }

        saveGamerData(gamers, numGamers, maxGameScore);

        cout << "Do you want to play again? Please click 'y' for Yes and 'n' for No: ";
        char choice;
        cin >> choice;
        cin.ignore();
        if (choice != 'y' && choice != 'Y')
            break;

        currentScore = 0;
        isGameOver = false;
        tailLength = 0;
    }

    return 0;
}
