#include "Pathfinder.h"

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>

using std::vector;
using std::priority_queue;
using std::greater;

Node::Node(int _x, int _y) 
    : x(_x), y(_y), f(0), g(0), h(0) {}

bool Node::operator>(const Node& other) const {
    return f > other.f;
}

bool Node::operator==(const Node& other) const {
    return x == other.x && y == other.y;
}

vector<Node> FindPath(const vector<vector<int>>& graph, const Node& start, const Node& goal) {
    const int directionX[] = {-1, 0, 1, 0};
    const int directionY[] = {0, 1, 0, -1};

    priority_queue<Node, vector<Node>, greater<Node>> openList;
    vector<vector<bool>> closedList(graph.size(), vector<bool>(graph[0].size(), false));
    vector<vector<Node>> cameFrom(graph.size(), vector<Node>(graph[0].size(), Node(-1, -1)));

    Node startCopy = start;
    openList.push(startCopy);

    while (!openList.empty()) {
        Node current = openList.top();
        openList.pop();

        if (current == goal) {
            vector<Node> path;
            while (!(current == start)) {
                path.push_back(current);
                current = cameFrom[current.x][current.y];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            return path;
        }

        closedList[current.x][current.y] = true;

        for (int i = 0; i < 4; ++i) {
            int newX = current.x + directionX[i];
            int newY = current.y + directionY[i];

            if (newX >= 0 && newX < graph.size() && newY >= 0 && newY < graph[0].size()) {
                if (graph[newX][newY] == 0 && !closedList[newX][newY]) {
                    Node neighbor(newX, newY);
                    int newG = current.g + 1;

                    neighbor.g = newG;
                    neighbor.h = abs(newX - goal.x) + abs(newY - goal.y);
                    neighbor.f = neighbor.g + neighbor.h;

                    cameFrom[newX][newY] = current;
                    openList.push(neighbor);
                }
            }
        }
    }

    return vector<Node>(); // No path found
}

void PrintPath(const vector<Node>& path) {
    for (const Node& node : path) {
        std::cout << "(" << node.x << ", " << node.y << ") ";
    }
    std::cout << std::endl;
}
