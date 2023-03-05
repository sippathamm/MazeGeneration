#include <iostream>
#include <stack>
#include <vector>
#include <thread>
#include <chrono>

#define GL_SILENCE_DEPRECATION
#include <glfw3.h>

#define DELAY(A) std::this_thread::sleep_for(std::chrono::milliseconds(A));

const int Width = 800;
const int Height = 800;

const int N = 20;
const float CellWidth = 2.0f / N;
const float Offset = CellWidth / 4.0f;

typedef struct Cell {
    int Direction = 0x00;
    bool Visited = false;
    bool InPath = false;
} Cell;

enum {
    TO_WEST = 0x01,
    TO_EAST = 0x02,
    TO_NORTH = 0x04,
    TO_SOUTH = 0x08
};

std::vector<std::vector<Cell>> Maze(N, std::vector<Cell> (N));
std::vector<std::pair<int, int>> Path;

void DFS (std::stack<std::pair<int, int>> &Stack, int &Visited) {
    while (Visited < N * N) {
        int X = Stack.top().first;
        int Y = Stack.top().second;

        std::vector<int> Neighbors;

        // Left
        if (X > 0 && !Maze[Y][X - 1].Visited) {
            Neighbors.push_back(TO_WEST);
        }

        // Right
        if (X < N - 1 && !Maze[Y][X + 1].Visited) {
            Neighbors.push_back(TO_EAST);
        }

        // Top
        if (Y > 0 && !Maze[Y - 1][X].Visited) {
            Neighbors.push_back(TO_NORTH);
        }

        // Bottom
        if (Y < N - 1 && !Maze[Y + 1][X].Visited) {
            Neighbors.push_back(TO_SOUTH);
        }

        if (!Neighbors.empty()) {
            int RandomNext = Neighbors[rand() % (int)Neighbors.size()];
            switch (RandomNext) {
                case TO_WEST:
                    Maze[Y][X - 1].Visited = true;
                    Maze[Y][X - 1].Direction |= TO_EAST;
                    Maze[Y][X].Direction |= TO_WEST;
                    Stack.push(std::make_pair(X - 1, Y + 0));
                    break;
                case TO_EAST:
                    Maze[Y][X + 1].Visited = true;
                    Maze[Y][X + 1].Direction |= TO_WEST;
                    Maze[Y][X].Direction |= TO_EAST;
                    Stack.push(std::make_pair(X + 1, Y + 0));
                    break;
                case TO_NORTH:
                    Maze[Y - 1][X].Visited = true;
                    Maze[Y - 1][X].Direction |= TO_SOUTH;
                    Maze[Y][X].Direction |= TO_NORTH;
                    Stack.push(std::make_pair(X + 0, Y - 1));
                    break;
                case TO_SOUTH:
                    Maze[Y + 1][X].Visited = true;
                    Maze[Y + 1][X].Direction |= TO_NORTH;
                    Maze[Y][X].Direction |= TO_SOUTH;
                    Stack.push(std::make_pair(X + 0, Y + 1));
                    break;
            }
            Visited++;
        } else {
            Stack.pop();
        }
    }
}

void GenerateMaze () {
    int Visited = 0;
    std::stack<std::pair<int, int>> Stack;

    int X = rand() % N;
    int Y = rand() % N;
    Stack.push(std::make_pair(X, Y));
    Maze[Y][X].Visited = true;
    Visited++;

    DFS(Stack, Visited);
}

void SolveMaze() {
    int X = Path.back().first;
    int Y = Path.back().second;

    if (X == N - 1 && Y == N - 1) {
        return;
    }

    Maze[Y][X].InPath = true;
    bool Stuck = true;

    if ((Maze[Y][X].Direction & TO_WEST) && !Maze[Y][X - 1].InPath) {
        Path.push_back(std::make_pair(X - 1, Y));
        Stuck = false;
    }
    if ((Maze[Y][X].Direction & TO_EAST) && !Maze[Y][X + 1].InPath) {
        Path.push_back(std::make_pair(X + 1, Y));
        Stuck = false;
    }
    if ((Maze[Y][X].Direction & TO_NORTH) && !Maze[Y - 1][X].InPath) {
        Path.push_back(std::make_pair(X, Y - 1));
        Stuck = false;
    }
    if ((Maze[Y][X].Direction & TO_SOUTH) && !Maze[Y + 1][X].InPath) {
        Path.push_back(std::make_pair(X, Y + 1));
        Stuck = false;
    }

    if (Stuck) {
        Path.pop_back();
    }
}

void DrawCell (float X, float Y, float SizeX, float SizeY, float Red, float Green, float Blue) {
    glColor3f(Red, Green, Blue);
    glBegin(GL_QUADS);
    glVertex2f(X, Y);
    glVertex2f(X + SizeX, Y);
    glVertex2f(X + SizeX, Y - SizeY);
    glVertex2f(X, Y - SizeY);
    glEnd();
}

void DrawMaze () {
    DELAY(30)

    float PathWidth = 2.0f * Offset;

    for (int X = 0; X < N; X++) {
        for (int Y = 0; Y < N; Y++) {
            float i = -1.0f + ((float)X * CellWidth);
            float j = 1.0f - ((float)Y * CellWidth);
            if (Maze[Y][X].Visited) {
                DrawCell(i + Offset, j - Offset, PathWidth, PathWidth, 1.0f, 1.0f, 1.0f);
            } else {
                DrawCell(i + Offset, j - Offset, PathWidth, PathWidth, 0.0f, 0.0f, 1.0f);
            }
            if (Maze[Y][X].Direction & TO_SOUTH) {
                DrawCell(i + Offset, j - (3 * Offset), PathWidth, PathWidth, 1.0f, 1.0f, 1.0f);
            }
            if (Maze[Y][X].Direction & TO_EAST) {
                DrawCell(i + (3 * Offset), j - Offset, PathWidth, PathWidth, 1.0f, 1.0f, 1.0f);
            }
        }
    }

    /*
    float X = -1.0f + ((float)Stack.top().first * CellWidth);
    float Y = 1.0f - ((float)Stack.top().second * CellWidth);
    DrawCell(X + Offset, Y - Offset, PathWidth, PathWidth, 0.0f, 1.0f, 0.0);
    */

    for (auto Each : Path) {
        int X = Each.first;
        int Y = Each.second;
        float i = -1.0f + ((float)X * CellWidth);
        float j = 1.0f - ((float)Y * CellWidth);
        DrawCell(i + Offset, j - Offset, PathWidth, PathWidth, 1.0f, 0.0f, 0.0);
        if (Maze[Y][X].Direction & TO_SOUTH) {
            DrawCell(i + Offset, j - (3 * Offset), PathWidth, PathWidth, 1.0f, 0.0f, 0.0f);
        }
        if (Maze[Y][X].Direction & TO_EAST) {
            DrawCell(i + (3 * Offset), j - Offset, PathWidth, PathWidth, 1.0f, 0.0f, 0.0f);
        }
    }
}

int main () {
    if (glfwInit() == false) {
        std::cout << "Unable to initialize\n";
        return -1;
    }

    GLFWwindow* Window = glfwCreateWindow(Width, Height, "Maze Solver", nullptr, nullptr);

    if (Window == nullptr) {
        std::cout << "Unable to create the window\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(Window);

    srand(clock());

    GenerateMaze();

    Path.push_back(std::make_pair(0, 0));

    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SolveMaze();
        DrawMaze();

        glfwSwapBuffers(Window);
    }

    glfwDestroyWindow(Window);
    glfwTerminate();

    return 0;
}
