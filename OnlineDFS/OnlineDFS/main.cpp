#include <iostream>
#include <fstream>
#include <stack>
#include <thread>
#include <chrono>
#include <queue>
#include <map>

using namespace std;
using namespace chrono;
using namespace this_thread;

struct coordinates {
    int x;
    int y;
};

struct node {
    coordinates coords;
    stack<string> actions;
    int pCost = 0;
};

//GLOBAL VARIABLES
coordinates start = { 0, 0 };
coordinates startSave = { 0, 0 };
const int maxRow = 100;
const int maxCol = 200;
const string UP = "up";
const string DOWN = "down";
const string LEFT = "left";
const string RIGHT = "right";
const string STOP = "stop";
const char complete = 'c';
char maze[maxRow][maxCol] = {0};
char input;
int row = 0;
int col = 0;
bool hasItem;

//Online DFS variables
coordinates s = { -1, -1 };
string a = "";
vector<node> untried;
stack<coordinates> unbacktracked;
stack<string> finalActions;
map<coordinates, pair<string, coordinates>> results;


//Functions
node Actions(coordinates state);
string OnlineDFSAgent(coordinates state);
coordinates backtrack(coordinates current);
string PathToCoord(coordinates to, coordinates from);
string GetOppositeAction(string action);
bool getSuccessor(coordinates current);
bool GoalTest(coordinates state);
void startMaze(coordinates start);
bool lookUp(int x, int y);
bool lookDown(int x, int y);
bool lookLeft(int x, int y);
bool lookRight(int x, int y);
void goUp(int x, int y);
void goDown(int x, int y);
void goLeft(int x, int y);
void goRight(int x, int y);

//Input/Output
void readFile(char maze[maxRow][maxCol], string fileName);
void countFile(string fileName);
void ClearScreen();
void printFile();

int main(int argc, const char * argv[]) {
    string fileName;
    cout << endl << "Please enter the file name: ";
    cin >> fileName;
    countFile(fileName);
    
    cout << endl << "Reading File..." << endl;
    readFile(maze, fileName);
    cout << endl << "Press ENTER to iterate or C to complete." << endl;
    ClearScreen();
    startMaze(start);
    cout << endl <<  "COMPLETED!" << endl;
    return 0;
}

void startMaze(coordinates start) {
    string action;
    while (action != "GOAL") {
        action = OnlineDFSAgent(start);
        finalActions.push(GetOppositeAction(action));
        if (action == UP) {
            goUp(start.x, start.y);
            start.x -= 1;
        } else if (action == DOWN) {
            goDown(start.x, start.y);
            start.x += 1;
        } else if (action == LEFT) {
            goLeft(start.x, start.y);
            start.y -= 1;
        } else if (action == RIGHT) {
            goRight(start.x, start.y);
            start.y += 1;
        }
        printFile();
        ClearScreen();
    }
    while (!unbacktracked.empty()) {
        cout << "BACKTRACKING" << endl;
        coordinates temp = unbacktracked.top();
        if (temp.x == start.x && temp.y == start.y) {
            unbacktracked.pop();
            temp = unbacktracked.top();
        }
        unbacktracked.pop();
        action = PathToCoord(temp, start);

        if (action == UP) {
            goUp(start.x, start.y);
            start.x -= 1;
        } else if (action == DOWN) {
            goDown(start.x, start.y);
            start.x += 1;
        } else if (action == LEFT) {
            goLeft(start.x, start.y);
            start.y -= 1;
        } else if (action == RIGHT) {
            goRight(start.x, start.y);
            start.y += 1;
        }
        printFile();
        ClearScreen();
    }
    
}

void printFile() {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if(maze[i][j] == 'W')
                cout << "\u2588";
            else if(maze[i][j] == 'e')
                cout << 'e';
            else if(maze[i][j] == '*')
                cout << '*';
            else if(maze[i][j] == 'x')
                cout << 'x';
            else
                cout << ' ';
        }
        cout << endl;
    }
}

string GetOppositeAction(string action) {
    string newAction;
    if (action == UP) {
        newAction = DOWN;
    } else if(action == DOWN) {
        newAction = UP;
    } else if(action == LEFT) {
        newAction = RIGHT;
    } else if(action == RIGHT) {
        newAction = LEFT;
    } else {
        newAction = "NOTHING";
    }
    return newAction;
}

void countFile(string fileName) {
    ifstream file;
    string line;
    file.open(fileName);
    while (getline(file, line)) { row++; }
    col = (int)line.length();
    file.close();
}

void readFile(char maze[maxRow][maxCol], string fileName) {
    ifstream file;
    char fileChar;
    int i = 0;
    int j = 0;
    file.open(fileName);
    while (!file.eof()) {
        file.get(fileChar);
        if (fileChar == 'e') {
            start.x = i;
            start.y = j;
            startSave.x = start.x;
            startSave.y = start.y;
        }
        maze[i][j] = fileChar;
        j++;
        if (j > col) {
            j = 0;
            i++;
        }
    }
    file.close();
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if(maze[i][j] == 'W')
                cout << "\u2588";
            else if(maze[i][j] == 'e')
                cout << 'e';
            else if(maze[i][j] == '*')
                cout << '*';
            else
                cout << ' ';
        }
        cout << endl;
    }
}

bool lookUp(int x, int y) {
    if ((maze[x - 1][y] != ' ' && maze[x - 1][y] != 'x')) { return false; }
    return true;
}

bool lookDown(int x, int y) {
    if ((maze[x + 1][y] != ' '&& maze[x + 1][y] != 'x')) { return false; }
    return true;
}

bool lookLeft(int x, int y) {
    if ((maze[x][y - 1] != ' ' && maze[x][y - 1] != 'x')) { return false; }
    return true;
}

bool lookRight(int x, int y) {
    if ((maze[x][y + 1] != ' ' && maze[x][y + 1] != 'x')) { return false; }
    return true;
}

void goRight(int x, int y) {
    if (maze[x][y] != 'e') {
        if(maze[x][y + 1] != 'x') {
            maze[x][y + 1] = '*';
        }
        maze[x][y] = ' ';
    }
}

void goLeft(int x, int y) {
    if (maze[x][y] != 'e') {
        if(maze[x][y - 1] != 'x') {
            maze[x][y - 1] = '*';
        }
        maze[x][y] = ' ';
    }
}

void goUp(int x, int y) {
    if (maze[x][y] != 'e') {
        if(maze[x - 1][y] != 'x') {
            maze[x - 1][y] = '*';
        }
        maze[x][y] = ' ';
    }
}

void goDown(int x, int y) {
    if (maze[x][y] != 'e') {
        if(maze[x + 1][y] != 'x') {
            maze[x + 1][y] = '*';
        }
        maze[x][y] = ' ';
    }
    
}

node Actions(coordinates state) {
    node tempNode;
    string skip;
    skip = GetOppositeAction(a);
    if (lookUp(state.x, state.y) && skip != UP) {
        tempNode.actions.push(UP);
    }
    if (lookDown(state.x, state.y) && skip != DOWN) {
        tempNode.actions.push(DOWN);
    }
    if (lookLeft(state.x, state.y) && skip != LEFT) {
        tempNode.actions.push(LEFT);
    }
    if (lookRight(state.x, state.y) && skip != RIGHT) {
        tempNode.actions.push(RIGHT);
    }
    tempNode.coords = state;
    return tempNode;
}

bool GoalTest(coordinates state) {
    if (maze[state.x][state.y] == 'x') {
        return true;
    }
    return false;
}

bool Contains(coordinates current) {
    int index = -1;
    for (int i = 0; i < untried.size(); i++) {
        if (current.x == untried[i].coords.x && current.y == untried[i].coords.y) {
            index = i;
        }
    }
    if (index != -1) {
        return true;
    }
    return false;
}

bool NoUntriedActions(coordinates current) {
    int index = -1;
    for (int i = 0; i < untried.size(); i++) {
        if (current.x == untried[i].coords.x && current.y == untried[i].coords.y) {
            index = i;
        }
    }
    if (index == -1 || untried[index].actions.empty()) {
        return true;
    }
    return false;
}

string PathToCoord(coordinates to, coordinates from) {
    string path;
    if (from.x == to.x) {
        if (from.y < to.y) {
            path = RIGHT;
        } else
            path = LEFT;
    } else {
        if (from.x < to.x) {
            path = DOWN;
        } else
            path = UP;
    }
    return path;
}

string FindUntriedAction(coordinates current) {
    string action;
    int index =  -1;
    for (int i = 0; i < untried.size(); i++) {
        if (current.x == untried[i].coords.x && current.y == untried[i].coords.y) {
            index = i;
        }
    }
    action = untried[index].actions.top();
    untried[index].actions.pop();
    return action;
}

string OnlineDFSAgent(coordinates current) {
    if (GoalTest(current)) {
        return "GOAL";
    }
    if (!Contains(current)) {
        untried.push_back(Actions(current));
    }
    if (s.x != -1 && s.y != -1) {
        unbacktracked.push(current);
    }
    if (NoUntriedActions(current)) {
        if (unbacktracked.empty()) {
            cout <<"STOP";
        } else {
            cout << "BACKTRACK" << endl;
            coordinates temp = unbacktracked.top();
            if (temp.x == current.x && temp.y == current.y) {
                unbacktracked.pop();
                temp = unbacktracked.top();
            }
            unbacktracked.pop();
            a = PathToCoord(temp, current);
        }
    } else {
        a = FindUntriedAction(current);
    }
    s = current;
    return a;
}

void ClearScreen() {
    cout << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;
    sleep_for(nanoseconds(150000000));
}
