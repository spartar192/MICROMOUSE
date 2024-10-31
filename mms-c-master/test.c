#include "API.h"
#include "Stack.h"
#include "Maze.h"
#include <stdio.h>
#include <stdlib.h>


char direction = 'N';
int done = FALSE;


void logmess(char *text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

int isSafe(int cellX, int cellY) {
    if (cellX < 0 || cellX > MAZE_SIZE - 1 || cellY < 0 || cellY > MAZE_SIZE - 1)
        return FALSE;
    return TRUE;
}

int min(int a, int b, int c, int d) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    if (d < min) min = d;
    return min;
}

int isGoal(int cellX, int cellY) {
    if ((cellX == MAZE_SIZE/2 || cellX == MAZE_SIZE/2 - 1) 
    && (cellY == MAZE_SIZE/2 || cellY == MAZE_SIZE/2 - 1))
        return TRUE;
    return FALSE;
}

Cell *getFrontCell(Maze *maze, int x, int y){
    switch (direction) {
        case 'N':
            return maze->map[x][y + 1];
        case 'S':
            return maze->map[x][y - 1];
        case 'E':
            return maze->map[x + 1][y];
        case 'W':
            return maze->map[x - 1][y];
        default:
            return NULL;
    }
}

Cell *getBackCell(Maze *maze, int x, int y){
    if(maze->map[x][y]->prevVisitedCell != NULL) 
        return maze->map[x][y]->prevVisitedCell;
    return maze->map[x][y];
}


Cell *getLeftCell(Maze *maze, int x, int y){
    switch (direction) {
        case 'N':
            return maze->map[x - 1][y];
        case 'S':
            return maze->map[x + 1][y];
        case 'E':
            return maze->map[x][y + 1];
        case 'W':
            return maze->map[x][y - 1];
        default:
            return NULL;
    }
}
Cell *getRightCell(Maze *maze, int x, int y){
    switch (direction) {
        case 'N':
            return maze->map[x + 1][y];
        case 'S':
            return maze->map[x - 1][y];
        case 'E':
            return maze->map[x][y - 1];
        case 'W':
            return maze->map[x][y + 1];
        default:
            return NULL;
    }
}

// update the position of the robot
void updatePosition(int* x, int* y, char currentMove) {
    switch (direction) {
        case 'N':
            if (currentMove == 'l')
                direction = 'W';
            else if (currentMove == 'r')
                direction = 'E';
            else if (currentMove == 'f')
                ++(*y);
            break;
        case 'S':
            if (currentMove == 'l')
                direction = 'E';
            else if (currentMove == 'r')
                direction = 'W';
            else if (currentMove == 'f')
                --(*y);
            break;
        case 'E':
            if (currentMove == 'l')
                direction = 'N';
            else if (currentMove == 'r')
                direction = 'S';
            else if (currentMove == 'f')
                ++(*x);
            break;
        case 'W':
            if (currentMove == 'l')
                direction = 'S';
            else if (currentMove == 'r')
                direction = 'N';
            else if (currentMove == 'f')
                --(*x);
            break;
        default:
            x = 0, y = 0;
    }
}

// move the robot to neighbor has MinDistance
void moveInDirection(Maze *maze, int *x, int *y, char direct) {
    // getMinValueDirection(maze, *x, *y, &direct);
    int prevX = *x, prevY = *y;
    if (direct == 'f') {
        API_moveForward();
        updatePosition(x, y, 'f');
    }
    else if (direct == 'l') {
        API_turnLeft();
        updatePosition(x, y, 'l');
        API_moveForward();
        updatePosition(x, y, 'f');
    }
    else if (direct == 'r') {
        API_turnRight();
        updatePosition(x, y, 'r');
        API_moveForward();
        updatePosition(x, y, 'f');
    }
    else if (direct == 'b') {
        API_turnRight();
        updatePosition(x, y, 'r');
        API_turnRight();
        updatePosition(x, y, 'r');
        API_moveForward();
        updatePosition(x, y, 'f');
    }

    maze->map[*x][*y]->prevVisitedCell = maze->map[prevX][prevY];
}


void moveToNeighbouringCell(Maze *maze, int *x, int *y, int targetX, int targetY) {
    switch (direction) {
        case 'N':
            if (targetX == (*x - 1))
                moveInDirection(maze, x, y, 'l');  // Move left
            else if (targetY == (*y + 1))
                moveInDirection(maze, x, y, 'f');  // Move forward
            else if (targetX == (*x + 1))
                moveInDirection(maze, x, y, 'r');  // Move right
            else if (targetY == (*y - 1))
                moveInDirection(maze, x, y, 'b');  // Move backward
            break;

        case 'S':
            if (targetX == (*x - 1))
                moveInDirection(maze, x, y, 'r');  // Move right
            else if (targetY == (*y + 1))
                moveInDirection(maze, x, y, 'b');  // Move backward
            else if (targetX == (*x + 1))
                moveInDirection(maze, x, y, 'l');  // Move left
            else if (targetY == (*y - 1))
                moveInDirection(maze, x, y, 'f');  // Move forward
            break;

        case 'E':
            if (targetX == (*x - 1))
                moveInDirection(maze, x, y, 'b');  // Move backward
            else if (targetY == (*y + 1))
                moveInDirection(maze, x, y, 'l');  // Move left
            else if (targetX == (*x + 1))
                moveInDirection(maze, x, y, 'f');  // Move forward
            else if (targetY == (*y - 1))
                moveInDirection(maze, x, y, 'r');  // Move right
            break;

        case 'W':
            if (targetX == (*x - 1))
                moveInDirection(maze, x, y, 'f');  // Move forward
            else if (targetY == (*y + 1))
                moveInDirection(maze, x, y, 'r');  // Move right
            else if (targetX == (*x + 1))
                moveInDirection(maze, x, y, 'b');  // Move backward
            else if (targetY == (*y - 1))
                moveInDirection(maze, x, y, 'l');  // Move left
            break;

        default:
            return;
    }
}


void exploreCell(Maze *maze, int x, int y) {
    maze->map[x][y]->hasBeenExplored = TRUE;
    API_setColor(x, y, 'R');
// first visit is straight, then right, then left, then back
    switch (direction) {
        case 'N':
            if (API_wallFront()){
                maze->map[x][y]->NorthWall = TRUE;
                API_setWall(x, y, 'n');
            }
            if (API_wallRight()){
                maze->map[x][y]->EastWall = TRUE;
                API_setWall(x, y, 'e');
            }
            if (API_wallLeft()){
                maze->map[x][y]->WestWall = TRUE;
                API_setWall(x, y, 'w');
            }
            break;
        case 'S':
            if (API_wallFront()){
                maze->map[x][y]->SouthWall = TRUE;
                API_setWall(x, y, 's');
            }
            if (API_wallRight()){
                maze->map[x][y]->WestWall = TRUE;
                API_setWall(x, y, 'w');
            }
            if (API_wallLeft()){
                maze->map[x][y]->EastWall = TRUE;
                API_setWall(x, y, 'e');
            }
            break;
        case 'W':
            if (API_wallFront()){
                maze->map[x][y]->WestWall = TRUE;
                API_setWall(x, y, 'w');
            }
            if (API_wallRight()){
                maze->map[x][y]->NorthWall = TRUE;
                API_setWall(x, y, 'n');
            }
            if (API_wallLeft()){
                maze->map[x][y]->SouthWall = TRUE;
                API_setWall(x, y, 's');
            }
            break;
        case 'E':
            if (API_wallFront()){
                maze->map[x][y]->EastWall = TRUE;
                API_setWall(x, y, 'e');
            }
            if (API_wallRight()){
                maze->map[x][y]->SouthWall = TRUE;
                API_setWall(x, y, 's');
            }
            if (API_wallLeft()){
                maze->map[x][y]->NorthWall = TRUE;
                API_setWall(x, y, 'n');
            }
            break;
        default:
            break;
    }
}


int getMinNeighborValue(Maze* maze, int x, int y) {
    int minValue = maze->map[x][y]->FloodFillValue; // as max as possible

    // check front, back, left, right
    if (isSafe(x, y + 1) && !maze->map[x][y]->NorthWall) {
        minValue = (maze->map[x][y + 1]->FloodFillValue < minValue) 
                    ? maze->map[x][y + 1]->FloodFillValue 
                    : minValue;
    }

    if (isSafe(x, y - 1) && !maze->map[x][y]->SouthWall) {
        minValue = (maze->map[x][y - 1]->FloodFillValue < minValue) 
                    ? maze->map[x][y - 1]->FloodFillValue 
                    : minValue;
    }

    if (isSafe(x - 1, y) && !maze->map[x][y]->WestWall) {
        minValue = (maze->map[x - 1][y]->FloodFillValue < minValue) 
                    ? maze->map[x - 1][y]->FloodFillValue 
                    : minValue;
    }

    if (isSafe(x + 1, y) && !maze->map[x][y]->EastWall) {
        minValue = (maze->map[x + 1][y]->FloodFillValue < minValue) 
                    ? maze->map[x + 1][y]->FloodFillValue 
                    : minValue;
    }

    return minValue; // return the min value
} 

void getMinValueDirection(Maze *maze, int x, int y, char *minDirection) {
    int minValue = getMinNeighborValue(maze, x, y);
    if (!API_wallRight()) {
        Cell *right = getRightCell(maze, x, y);
        if (right->FloodFillValue == minValue) {
            // *minValue = right->FloodFillValue;
            *minDirection = 'r';
        }
    }
    if (!API_wallLeft()) {
        Cell *left = getLeftCell(maze, x, y);
        if (left->FloodFillValue == minValue) {
            // *minValue = left->FloodFillValue;
            *minDirection = 'l';
        }
    }
    if (maze->map[x][y]->prevVisitedCell != NULL) {
        Cell *back = getBackCell(maze, x, y);
        if (back->FloodFillValue == minValue) {
            // *minValue = back->FloodFillValue;
            *minDirection = 'b';
        }
    }
    if (!API_wallFront()) {
       Cell *front = getFrontCell(maze, x, y);
       if (front->FloodFillValue == minValue) {
        //    *minValue = front->FloodFillValue;
           *minDirection = 'f';
        }
    }
}

void floodFillNeighbours(Maze* maze, int x, int y) {
    Stack *floodStack = createStack();
    push(floodStack, maze->map[x][y]);

    while (!isEmpty(floodStack)) {
        int cellX = top(floodStack)->x;
        int cellY = top(floodStack)->y;
        pop(floodStack);

        if (cellX == MAZE_SIZE/2 && cellY == MAZE_SIZE/2)
            continue;

        Cell* cell = maze->map[cellX][cellY];
        int minNeighborValue = getMinNeighborValue(maze, cellX, cellY);

        // update the value of the cell
        if (cell->FloodFillValue != 1 +  minNeighborValue) {
            cell->FloodFillValue = minNeighborValue + 1;
            API_setNumber(cellX, cellY, cell->FloodFillValue);
            // push theo thứ tự up, left, down, right

            if (isSafe(cellX, cellY + 1) && maze->map[cellX][cellY + 1]->FloodFillValue != 0) {
                push(floodStack, maze->map[cellX][cellY + 1]);
                // push(visitedStack, maze->map[cellX][cellY + 1]);
            }
            if (isSafe(cellX - 1, cellY) && maze->map[cellX - 1][cellY]->FloodFillValue != 0) {
                push(floodStack, maze->map[cellX - 1][cellY]);
                // push(visitedStack, maze->map[cellX - 1][cellY]);
            }
            if (isSafe(cellX, cellY - 1) && maze->map[cellX][cellY - 1]->FloodFillValue != 0) {
                push(floodStack, maze->map[cellX][cellY - 1]);
                // push(visitedStack, maze->map[cellX][cellY - 1]);
            }
            if (isSafe(cellX + 1, cellY) && maze->map[cellX + 1][cellY]->FloodFillValue != 0) {
                push(floodStack, maze->map[cellX + 1][cellY]);
                // push(visitedStack, maze->map[cellX + 1][cellY]);
            }
        }
    }
   
    free(floodStack); 
}


void findStartToGoal(Maze *maze,int x, int y) {
    int destinationFound = FALSE;
    char minDirection;

    while (!destinationFound) {
        exploreCell(maze, x, y); // explore current cell
        int minDistance = getMinNeighborValue(maze, x, y); // get neighbour with lowest distance
        
        // if goal fount-> exit while loop
        if (x == MAZE_SIZE/2 && y == MAZE_SIZE/2) {
            destinationFound = TRUE;
        }
        if (!destinationFound) {
            getMinValueDirection(maze, x, y, &minDirection);
            // check if reflooding is required
            if ((maze->map[x][y]->FloodFillValue != 1 + minDistance)) {
                // reflood
                floodFillNeighbours(maze,x, y);
                logmess("Reflooding...");

                // after reflooding get new min distance neighbours
                getMinValueDirection(maze, x, y, &minDirection); //get neighbour with lowest distance
            }
            moveInDirection(maze, &x, &y, minDirection);

            logmess("Moving...");
        }
    }

}




int main(int argc, char *argv[]) {
    int x = 0, y = 0;
    // Create maze
    Maze *maze = createMaze();
    API_setColor(0, 0, 'Y');

    logmess("Started flood fill to explore  the maze...");

    findStartToGoal(maze, x, y);
    logmess("Flood fill completed, now returning to start position...");

    logmess("go back to start position");


    

}

