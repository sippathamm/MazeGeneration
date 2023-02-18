#include <iostream>
#include <stack>
#include <vector>
#include <thread>
#include <chrono>

#include <glfw3.h>

const int Width = 800;
const int Height = 800;

const int N = 20;
const float CellWidth = 2.0f / N;
const float Offset = CellWidth / 4.0f;

typedef struct Cell {
    int Direction = 0x00;
    bool Visited = false;
} Cell;

enum {
    TO_WEST = 0x01,
    TO_EAST = 0x02,
    TO_NORTH = 0x04,
    TO_SOUTH = 0x08
};

int Visited = 0;
std::vector<std::vector<Cell>> Maze(N, std::vector<Cell> (N));
std::stack<std::pair<int, int>> Stack;

void GenerateMaze () {
    if (Visited >= N * N) {
        return;
    }

    int X = Stack.top().first;
    int Y = Stack.top().second;

    std::vector<int> Neighbor;

    // Left
    if (X > 0 && !Maze[Y][X - 1].Visited) {
        Neighbor.push_back(TO_WEST);
    }

    // Right
    if (X < N - 1 && !Maze[Y][X + 1].Visited) {
        Neighbor.push_back(TO_EAST);
    }

    // Top
    if (Y > 0 && !Maze[Y - 1][X].Visited) {
        Neighbor.push_back(TO_NORTH);
    }

    // Bottom
    if (Y < N - 1 && !Maze[Y + 1][X].Visited) {
        Neighbor.push_back(TO_SOUTH);
    }

    if (!Neighbor.empty()) {
        int Next = Neighbor[rand() % (int)Neighbor.size()];
        switch (Next) {
            case TO_WEST:
                Maze[Y][X - 1].Visited = true;
                Maze[Y][X - 1].Direction |= TO_EAST;
                Maze[Y][X].Direction |= TO_WEST;
                Stack.push(std::make_pair(Stack.top().first - 1, Stack.top().second + 0));
                break;
            case TO_EAST:
                Maze[Y][X + 1].Visited = true;
                Maze[Y][X + 1].Direction |= TO_WEST;
                Maze[Y][X].Direction |= TO_EAST;
                Stack.push(std::make_pair(Stack.top().first + 1, Stack.top().second + 0));
                break;
            case TO_NORTH:
                Maze[Y - 1][X].Visited = true;
                Maze[Y - 1][X].Direction |= TO_SOUTH;
                Maze[Y][X].Direction |= TO_NORTH;
                Stack.push(std::make_pair(Stack.top().first + 0, Stack.top().second - 1));
                break;
            case TO_SOUTH:
                Maze[Y + 1][X].Visited = true;
                Maze[Y + 1][X].Direction |= TO_NORTH;
                Maze[Y][X].Direction |= TO_SOUTH;
                Stack.push(std::make_pair(Stack.top().first + 0, Stack.top().second + 1));
                break;
        }
        Visited++;
    } else {
        Stack.pop();
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
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    float PathWidth = 2.0f * Offset;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float X = -1.0f + ((float)j * CellWidth);
            float Y = 1.0f - ((float)i * CellWidth);
            if (Maze[i][j].Visited) {
                DrawCell(X + Offset, Y - Offset, PathWidth, PathWidth, 1.0f, 1.0f, 1.0f);
            } else {
                DrawCell(X + Offset, Y - Offset, PathWidth, PathWidth, 0.0f, 0.0f, 1.0f);
            }
            if (Maze[i][j].Direction & TO_SOUTH) {
                DrawCell(X + Offset, Y - (3 * Offset), PathWidth, PathWidth, 1.0f, 1.0f, 1.0f);
            }
            if (Maze[i][j].Direction & TO_EAST) {
                DrawCell(X + (3 * Offset), Y - Offset, PathWidth, PathWidth, 1.0f, 1.0f, 1.0f);
            }
        }
    }

    float X = -1.0f + ((float)Stack.top().first * CellWidth);
    float Y = 1.0f - ((float)Stack.top().second * CellWidth);
    DrawCell(X + Offset, Y - Offset, PathWidth, PathWidth, 0.0f, 1.0f, 0.0);
}

int main () {
    if (glfwInit() == false) {
        std::cout << "Unable to initialize\n";
        return -1;
    }

    GLFWwindow* Window = glfwCreateWindow(Width, Height, "Maze Generation", nullptr, nullptr);

    if (Window == nullptr) {
        std::cout << "Unable to create the window\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(Window);

    srand(clock());

    int X = rand() % N;
    int Y = rand() % N;
    Stack.push(std::make_pair(X, Y));
    Maze[Y][X].Visited = true;
    Visited++;

    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GenerateMaze();
        DrawMaze();

        glfwSwapBuffers(Window);
    }

    glfwDestroyWindow(Window);
    glfwTerminate();

    return 0;
}
