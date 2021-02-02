#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <stack>
#include <fstream>
#include <windows.h>

using namespace std;

int Gcols, Grows, startX, startY, goalX, goalY;
char **grid, **temp, **temp3;

struct compare
{
	bool operator()(const double& l, const double& r)
	{
		return l > r;
	}
};

struct Node
{
	int x, y;
	double depth = 0;
	string path = "";
	int pathCost = 0;
	string actions = "";
};

struct BFSNode
{
	int x, y;
	string path = "";
	int pathCost = 0;
	string actions = "";
};

struct UCSNode
{
	int x, y, cost = 0;
	double heuristic = 0;
	string path = "";
	string actions = "";
	UCSNode()
		: x(0), y(0), cost(0), heuristic(0)
	{
	}
	UCSNode(int x, int y, int cost, double h)
		: x(x), y(y), cost(cost), heuristic(h)
	{
	}
};

struct CompareCost {
	bool operator()(UCSNode const& p1, UCSNode const& p2)
	{
		return p1.heuristic > p2.heuristic;
	}
};


void displayPath(string algo, string path, int cost)
{
	cout << "Calculating Path using " << algo << "\n";
	cout << "Goal Path is: " << path << "\n";
	cout << "Total Cost of path is: " << cost << "\n\n";
}

void changeCoordinates()
{
	startY = Grows - startY - 1;
	goalY = Grows - goalY - 1;
}

void readFile()
{
	ifstream indata("grid.txt");
	string data;
	//getline(indata, data);
	indata >> Gcols >> Grows;
	indata >> startX >> startY;
	indata >> goalX >> goalY;
	grid = new char*[Grows];
	temp = new char*[Grows];
	for (int j = 0; j < Grows; j++)
	{
		grid[j] = new char[Gcols];
		temp[j] = new char[Gcols];
	}
	for (int i = 0; i < Grows; i++)
	{
		for (int j = 0; j < Gcols; j++)
		{
			indata >> grid[i][j];
			temp[i][j] = grid[i][j];
		}
	}
	/*startY = Grows - startY - 1;
	goalY = Grows - goalY - 1;*/
	changeCoordinates();
}

void display(char **arr, int rows, int cols)
{
	/*for (int i = 0; i < rows; i++)
	{
	for (int j = 0; j < cols; j++)
	{
	cout << arr[i][j] << "  ";
	}
	cout << endl;
	}
	*/
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (arr[i][j] == '#')
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << arr[i][j] << "  ";
			}
			else
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);  //12 red, 11, 13,7
				cout << arr[i][j] << "  ";
			}
		}
		cout << endl;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
}

void ActionsPerformed(string actions){
	cout << "The Sequence of Actions Performed from 'S' to 'G': " << actions << endl << endl;
}

char** copyArray(char **temp2)
{
	temp2 = new char*[Grows];
	for (int i = 0; i < Grows; i++)
	{
		temp2[i] = new char[Gcols];
	}
	for (int i = 0; i < Grows; i++)
	{
		for (int j = 0; j < Gcols; j++)
		{
			temp2[i][j] = temp[i][j];
		}
	}
	return temp2;
}

void DisplayPathGrid(string path, char **temp2)
{

	int startBracket = 0, endBracket = -1;
	int length = path.length();
	while (startBracket = path.find("(", endBracket + 1) != -1)
	{
		startBracket = path.find("(", endBracket + 1);
		int comma = path.find(",", startBracket + 1);
		endBracket = path.find(")", comma);
		string col = path.substr(startBracket + 1, comma - startBracket - 1);
		string row = path.substr(comma + 1, endBracket - comma - 1);
		int rpos = Grows - stoi(row) - 1;
		int cpos = stoi(col);
		if (rpos == startY && stoi(col) == startX)
		{
			temp2[rpos][cpos] = 'S';
		}
		else if (rpos == goalY && stoi(col) == goalX)
			temp2[rpos][cpos] = 'G';
		else
			temp2[rpos][cpos] = '#';
	}
}

void deleteArray()
{
	for (int i = 0; i < Grows; i++)
	{
		delete[]temp3[i];
	}
	delete[]temp3;

}

void resetGrid()
{
	for (int i = 0; i < Grows; i++)
	{
		for (int j = 0; j < Gcols; j++)
		{
			grid[i][j] = temp[i][j];
		}
	}
}

string appendString(string path, int x, int y)
{
	y = Grows - y - 1;
	path = path + "(";
	path = path + to_string(x);
	path = path + ",";
	path = path + to_string(y);
	path = path + "), ";
	return path;
}

string appendActions(string path, string action)
{
	path = path + "->";
	path = path + action;
	return path;
}


double getHeuristicValue(int currentX, int currentY, int calX, int calY)
{
	double value = sqrt(pow((calX - currentX), 2) + pow((calY - currentY), 2));
	return value;
}

bool GreedyBestfirstSearch()
{
	resetGrid();
	priority_queue<UCSNode, std::vector<UCSNode>, CompareCost> Gqueue;
	UCSNode current;
	int lastrow, lastcol, Ccost; //to check for up,right,rightdiagonal
	current.x = startX;
	current.y = startY;
	current.cost = 0;
	current.path = appendString("", current.x, current.y);
	current.actions = "";
	Gqueue.push(current);
	bool GoalCheck = false;

	resetGrid();
	while (!Gqueue.empty() && GoalCheck != true)
	{

		current = Gqueue.top();
		lastrow = current.y;
		lastcol = current.x;
		Ccost = current.cost;
		string Cpath = current.path;
		string actions = current.actions;
		Gqueue.pop();
		grid[lastrow][lastcol] = '#';   //path followed
		if (lastrow - 1 == goalY&&lastcol + 1 == goalX)    //jo akhri Goal ke leye check kiya hay uska actions dalna hay 
		{
			current.y = lastrow - 1; current.x = lastcol + 1;
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Right Diagonal");
			current.cost += 2;
			GoalCheck = true;
		}
		else if (lastrow == goalY&&lastcol + 1 == goalX)
		{
			current.y = lastrow; current.x = lastcol + 1;
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Right");
			current.cost += 3;
			GoalCheck = true;
		}
		else if (lastrow - 1 == goalY&&lastcol == goalX)
		{
			current.y = lastrow - 1; current.x = lastcol;
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Up");
			current.cost += 1;
			GoalCheck = true;
		}
		if (GoalCheck)
		{
			Gqueue.push(current);
			displayPath("Greedy Best First Search", current.path, current.cost);
			ActionsPerformed(current.actions);
			temp3 = copyArray(temp3);
			DisplayPathGrid(current.path, temp3);
			display(temp3, Grows, Gcols);
			deleteArray();
			break;  //to end the loop here if goal is found. As goal check is applied afterwards so if it's G then true to stop the loop
		}

		if ((lastrow - 1) >= 0 && (lastcol + 1) >= 0 && (lastrow - 1) < Grows && (lastcol + 1) < Gcols&&grid[lastrow - 1][lastcol + 1] == '0')
		{

			int temp = Ccost + 2;
			double HeuristicCost = getHeuristicValue(lastcol + 1, Grows - lastrow - 1, goalX,Grows- goalY);
			current.y = lastrow - 1; current.x = lastcol + 1; current.cost = temp;
			current.heuristic = HeuristicCost;  //to push the heuristic cost
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Right Diagonal");
			Gqueue.push(current);
			grid[lastrow - 1][lastcol + 1] = 'P';               //node pushed in queue

		}  //right diagonal

		if ((lastrow) >= 0 && (lastcol + 1) >= 0 && (lastrow) < Grows && (lastcol + 1 < Gcols) && grid[lastrow][lastcol + 1] == '0')
		{

			int temp = Ccost + 3;
			double HeuristicCost = getHeuristicValue(lastcol + 1, Grows - lastrow, goalX, Grows- goalY);
			current.y = lastrow; current.x = lastcol + 1; current.cost = temp;
			current.heuristic = HeuristicCost;  //to push the heuristic cost
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Right");
			Gqueue.push(current);
			grid[lastrow][lastcol + 1] = 'P';      //node pushed in stack
		}   //right 

		if ((lastrow - 1) >= 0 && (lastcol) >= 0 && (lastrow - 1) < Grows && (lastcol < Gcols) && grid[lastrow - 1][lastcol] == '0')
		{
			int x = lastcol;
			int y = Grows - lastrow - 1;
			int temp = Ccost + 1;
			double HeuristicCost = getHeuristicValue(lastcol, Grows - lastrow - 1, goalX, Grows- goalY);
			current.y = lastrow - 1; current.x = lastcol; current.cost = temp;
			current.heuristic = HeuristicCost; //to push the heuristic cost
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Up");
			Gqueue.push(current);
			grid[lastrow - 1][lastcol] = 'P';      //node pushed in stack

		} //up

	}

	return GoalCheck;
}


bool AStar()
{
	resetGrid();
	priority_queue<UCSNode, std::vector<UCSNode>, CompareCost> Gqueue;
	UCSNode current;
	int lastrow, lastcol, Ccost; //to check for up,right,rightdiagonal
	current.x = startX;
	current.y = startY;
	current.cost = 0;
	current.path = appendString("", current.x, current.y);
	current.actions = "";
	Gqueue.push(current);
	bool GoalCheck = false;

	resetGrid();
	while (!Gqueue.empty() && GoalCheck != true)
	{

		current = Gqueue.top();
		lastrow = current.y;
		lastcol = current.x;
		Ccost = current.cost;
		string Cpath = current.path;
		string actions = current.actions;
		Gqueue.pop();
		grid[lastrow][lastcol] = '#';   //path followed
		if (lastrow - 1 == goalY&&lastcol + 1 == goalX)    //jo akhri Goal ke leye check kiya hay uska actions dalna hay 
		{
			current.y = lastrow - 1; current.x = lastcol + 1;
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Right Diagonal");
			current.cost += 2;
			GoalCheck = true;
		}
		else if (lastrow == goalY&&lastcol + 1 == goalX)
		{
			current.y = lastrow; current.x = lastcol + 1;
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Right");
			current.cost += 3;
			GoalCheck = true;
		}
		else if (lastrow - 1 == goalY&&lastcol == goalX)
		{
			current.y = lastrow - 1; current.x = lastcol;
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Up");
			current.cost += 1;
			GoalCheck = true;
		}
		if (GoalCheck)
		{
			Gqueue.push(current);
			displayPath("A*", current.path, current.cost);
			ActionsPerformed(current.actions);
			temp3 = copyArray(temp3);
			DisplayPathGrid(current.path, temp3);
			display(temp3, Grows, Gcols);
			deleteArray();
			break;  //to end the loop here if goal is found. As goal check is applied afterwards so if it's G then true to stop the loop
		}

		if ((lastrow - 1) >= 0 && (lastcol + 1) >= 0 && (lastrow - 1) < Grows && (lastcol + 1) < Gcols&&grid[lastrow - 1][lastcol + 1] == '0')
		{

			int temp = Ccost + 2;
			double HeuristicCost = getHeuristicValue(lastcol + 1, Grows - lastrow - 1, goalX, Grows- goalY);
			double funcValue = temp + HeuristicCost;
			current.y = lastrow - 1; current.x = lastcol + 1; current.cost = temp;
			current.heuristic = funcValue;  //to push the func cost
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Right Diagonal");
			Gqueue.push(current);
			grid[lastrow - 1][lastcol + 1] = 'P';               //node pushed in queue

		}  //right diagonal

		if ((lastrow) >= 0 && (lastcol + 1) >= 0 && (lastrow) < Grows && (lastcol + 1 < Gcols) && grid[lastrow][lastcol + 1] == '0')
		{

			int temp = Ccost + 3;
			double HeuristicCost = getHeuristicValue(lastcol + 1, Grows - lastrow, goalX, Grows- goalY);
			double funcValue = temp + HeuristicCost;
			current.y = lastrow; current.x = lastcol + 1; current.cost = temp;
			current.heuristic = funcValue;  //to push the func cost
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Right");
			Gqueue.push(current);
			grid[lastrow][lastcol + 1] = 'P';      //node pushed in stack
		}   //right 

		if ((lastrow - 1) >= 0 && (lastcol) >= 0 && (lastrow - 1) < Grows && (lastcol < Gcols) && grid[lastrow - 1][lastcol] == '0')
		{
			int x = lastcol;
			int y = Grows - lastrow - 1;
			int temp = Ccost + 1;
			double HeuristicCost = getHeuristicValue(lastcol, Grows - lastrow - 1, goalX, Grows- goalY);
			double funcValue = temp + HeuristicCost;
			current.y = lastrow - 1; current.x = lastcol; current.cost = temp;
			current.heuristic = funcValue; //to push the func cost
			current.path = appendString(Cpath, current.x, current.y);
			current.actions = appendActions(actions, "Up");
			Gqueue.push(current);
			grid[lastrow - 1][lastcol] = 'P';      //node pushed in stack

		} //up

	}

	return GoalCheck;
}

priority_queue <double, vector<double>, compare> fillQueue()
{
	resetGrid();
	stack<BFSNode> Gstack;
	BFSNode current;
	priority_queue <double, vector<double>, compare> Gqueue;
	int lastrow, lastcol, Ccost; //to check for up,right,rightdiagonal
	current.x = startX;
	current.y = startY;
	current.pathCost = 0;
	Gstack.push(current);
	bool GoalCheck = false;

	while (!Gstack.empty() && GoalCheck != true)
	{

		current = Gstack.top();
		lastrow = current.y;
		lastcol = current.x;
		Ccost = current.pathCost;
		Gstack.pop();
		grid[lastrow][lastcol] = '#';
		if (lastcol <= goalX&&lastrow >= goalY)
		{

			if ((lastrow - 1) >= 0 && (lastcol + 1) >= 0 && (lastrow - 1) < Grows && (lastcol + 1) < Gcols&&grid[lastrow - 1][lastcol + 1] == '0')
			{
				int temp = Ccost + 2;
				current.y = lastrow - 1; current.x = lastcol + 1; current.pathCost = temp;
				Gqueue.push(getHeuristicValue(lastcol, Grows - lastrow, goalX, Grows - goalY) + Ccost);
				Gstack.push(current);

			}  //right diagonal

			if ((lastrow) >= 0 && (lastcol + 1) >= 0 && (lastrow) < Grows && (lastcol + 1 < Gcols) && grid[lastrow][lastcol + 1] == '0')
			{
				int temp = Ccost + 3;
				current.y = lastrow; current.x = lastcol + 1; current.pathCost = temp;
				Gqueue.push(getHeuristicValue(lastcol, Grows - lastrow, goalX, Grows - goalY) + Ccost);
				Gstack.push(current);
			}   //right 

			if ((lastrow - 1) >= 0 && (lastcol) >= 0 && (lastrow - 1) < Grows && (lastcol < Gcols) && grid[lastrow - 1][lastcol] == '0')
			{
				int temp = Ccost + 1;
				current.y = lastrow - 1; current.x = lastcol; current.pathCost = temp;
				Gqueue.push(getHeuristicValue(lastcol, Grows - lastrow, goalX, Grows - goalY) + Ccost);
				Gstack.push(current);
			} //up

		}
	}
	return Gqueue;
}


bool IterativeDeepeningAStar()
{
	priority_queue <double, vector<double>, compare>Gqueue = fillQueue();
	stack<Node>Gstack;   //Stack
	Node current;
	int lastrow, lastcol, lastDepth; //to check for up,right,rightdiagonal

	double depthLimit = 0;
	bool GoalCheck = false;
	while (!GoalCheck&&!Gqueue.empty())
	{
		while (!Gstack.empty())
			Gstack.pop();
		current.x = startX;
		current.y = startY;
		current.depth = getHeuristicValue(startX, Grows - startY, goalX, Grows - goalY);
		current.path = appendString("", current.x, current.y);
		current.pathCost = 0;
		current.actions = "";
		Gstack.push(current);
		resetGrid();
		depthLimit = Gqueue.top();
		Gqueue.pop();
		while (!Gstack.empty() && GoalCheck != true)
		{

			current = Gstack.top();
			lastrow = current.y;
			lastcol = current.x;
			lastDepth = current.depth;
			string Cpath = current.path;
			int CurrCost = current.pathCost;
			string actions = current.actions;
			Gstack.pop();
			grid[lastrow][lastcol] = '#';   //path followed

			if (lastrow - 1 == goalY&&lastcol + 1 == goalX)    //jo akhri Goal ke leye check kiya hay uska actions dalna hay 
			{
				current.y = lastrow - 1; current.x = lastcol + 1;
				current.path = appendString(Cpath, current.x, current.y);
				current.pathCost = CurrCost + 2;
				current.actions = appendActions(actions, "Right Diagonal");
				GoalCheck = true;

			}
			else if (lastrow == goalY&&lastcol + 1 == goalX)
			{
				current.y = lastrow; current.x = lastcol + 1;
				current.path = appendString(Cpath, current.x, current.y);
				current.pathCost = CurrCost + 3;
				current.actions = appendActions(actions, "Right");
				GoalCheck = true;
			}
			else if (lastrow - 1 == goalY&&lastcol == goalX)
			{
				current.y = lastrow - 1; current.x = lastcol;
				current.path = appendString(Cpath, current.x, current.y);
				current.pathCost = CurrCost + 1;
				current.actions = appendActions(actions, "Up");
				GoalCheck = true;
			}
			if (GoalCheck)
			{
				Gstack.push(current);
				displayPath("Iterative Deepening A* Search", current.path, current.pathCost);
				ActionsPerformed(current.actions);
				temp3 = copyArray(temp3);
				DisplayPathGrid(current.path, temp3);
				display(temp3, Grows, Gcols);
				deleteArray();
				break;  //to end the loop here if goal is found. As goal check is applied afterwards so if it's G then true to stop the loop
			}
			if (lastDepth <= depthLimit && (lastrow) >= 0 && (lastcol + 1) >= 0 && (lastrow) < Grows && (lastcol + 1 < Gcols) && grid[lastrow][lastcol + 1] == '0')
			{
				int temp = CurrCost + 3;
				double value = getHeuristicValue(lastcol + 1, Grows - lastrow, goalX, Grows - goalY) + temp;
				current.y = lastrow; current.x = lastcol + 1; current.depth = value;
				current.path = appendString(Cpath, current.x, current.y);
				current.pathCost = CurrCost + 3;
				current.actions = appendActions(actions, "Right");
				Gstack.push(current);
				grid[lastrow][lastcol + 1] = 'P';      //node pushed in stack
			}   //right 



			if (lastDepth <= depthLimit && (lastrow - 1) >= 0 && (lastcol) >= 0 && (lastrow - 1) < Grows && (lastcol < Gcols) && grid[lastrow - 1][lastcol] == '0')
			{
				int temp = CurrCost + 1;
				double value = getHeuristicValue(lastcol, Grows - lastrow - 1, goalX, Grows - goalY) + temp;
				current.y = lastrow - 1; current.x = lastcol; current.depth = value;
				current.path = appendString(Cpath, current.x, current.y);
				current.pathCost = CurrCost + 1;
				current.actions = appendActions(actions, "Up");
				Gstack.push(current);
				grid[lastrow - 1][lastcol] = 'P';      //node pushed in stack
			} //up

			if (lastDepth <= depthLimit && (lastrow - 1) >= 0 && (lastcol + 1) >= 0 && (lastrow - 1) < Grows && (lastcol + 1) < Gcols&&grid[lastrow - 1][lastcol + 1] == '0')
			{

				int temp = CurrCost + 2;
				double value = getHeuristicValue(lastcol + 1, Grows - lastrow - 1, goalX, Grows - goalY) + temp;
				current.y = lastrow - 1; current.x = lastcol + 1; current.depth = value;
				current.path = appendString(Cpath, current.x, current.y);
				current.pathCost = CurrCost + 2;
				current.actions = appendActions(actions, "Right Diagonal");
				Gstack.push(current);
				grid[lastrow - 1][lastcol + 1] = 'P';               //node pushed in queue
			}  //right diagonal
		}
	}
	return GoalCheck;
}


int main()
{
	try
	{
		readFile();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
		char option = '9';
		do
		{
			switch (option)
			{
			case '1':
				if (!GreedyBestfirstSearch())
					cout << "Goal Not Found Using Best First Search(BFS)\n";
				break;
			case '2':
				if (!AStar())
					cout << "Goal Not Found Using A* Search(BFS)\n";
				break;
			case '3':
				if (!IterativeDeepeningAStar())
					cout << "Goal Not Found Using Iterative Deepening A* Search\n";
				break;
			case '4':
				cout << "Current Start Coordinates are: (" << startX << "," << Grows - startY - 1 << ")\n";
				cout << "Enter x and y coordinates of start position: ";
				cin >> startX >> startY;
				while (startX > Gcols || startX<0 || startY>Grows || startY < 0 || temp[Grows - startY - 1][startX] == '1')
				{
					cout << "Invalid input,Enter x and y coordinates of start position: ";
					cin >> startX >> startY;
				}
				cout << "Current Goal Coordinates are: (" << goalX << "," << Grows - goalY - 1 << ")\n";
				cout << "Enter x and y coordinates of goal position: ";
				cin >> goalX >> goalY;
				while (goalX > Gcols || goalX<0 || goalY>Grows || goalY < 0 || temp[Grows - goalY - 1][goalX] == '1')
				{
					cout << "Invalid input,Enter x and y coordinates of goal position: ";
					cin >> goalX >> goalY;
				}
				changeCoordinates();
				break;
			default:
				cout << "Invalid Input\n";
				break;
			}
			cout << "Press\n1. Greedy Best First Search(GBFS)\n2. A* Search\n3. Iterative Deepening A* Search\n4. To change start and goal coordinates\n 0. Exit\n";
			cin >> option;

		} while (option != '0');
	}
	catch (exception ex)
	{
		cout << ex.what() << "\n";
	}
	system("Pause");
}