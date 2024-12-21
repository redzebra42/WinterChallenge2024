#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <queue>
#include <unordered_map>

#include "utils.h"

using namespace std;

// TODO change old euclidian distance to accurateDistance

/**
 * A* algorithme (thanks ChatGPT)
 **/

// Structure to represent a node in the grid
struct Node {
    int x, y;       // Position on the grid
    float g, h;     // Cost from start (g) and heuristic cost to goal (h)
    Node* parent;   // Parent node for path reconstruction

    Node(int x, int y, float g, float h, Node* parent = nullptr)
        : x(x), y(y), g(g), h(h), parent(parent) {}

    // Calculate the total cost f = g + h
    float f() const { return g + h; }

    // Compare nodes to prioritize the one with the smallest f value
    bool operator>(const Node& other) const {
        return f() > other.f();
    }
};

// Manhattan distance heuristic
float heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// A* algorithm
vector<Node> aStar(vector<vector<Entity*>>& grid, Node start, Node goal) {
    int rows = grid.size();
    int cols = grid[0].size();

    priority_queue<Node, vector<Node>, greater<Node>> openList;  // Min-heap
    unordered_map<int, unordered_map<int, bool>> closedList;  // Closed list for visited nodes

    // Start node is pushed to open list
    openList.push(start);

    // Direction vectors for 4 directions (up, down, left, right)
    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    while (!openList.empty()) {
        Node current = openList.top();
        openList.pop();

        // If we reached the goal, reconstruct the path
        if (current.x == goal.x && current.y == goal.y) {
            vector<Node> path;
            Node* temp = &current;
            while (temp) {
                path.push_back(*temp);
                temp = temp->parent;
            }
            reverse(path.begin(), path.end());
            return path;
        }

        // Add current node to the closed list
        closedList[current.x][current.y] = true;

        // Explore the neighbors (4 directions)
        for (const auto& dir : directions) {
            int newX = current.x + dir.first;
            int newY = current.y + dir.second;

            // Skip out-of-bound or blocked cells or cells in the closed list
            if (newX < 0 || newX >= rows || newY < 0 || newY >= cols || grid[newX][newY]->owner != -1 || grid[newX][newY]->type == "WALL" || closedList[newX][newY]) {
                continue;
            }

            // Calculate g and h values for the neighbor
            float g = current.g + 1; // Cost from start
            float h = heuristic(newX, newY, goal.x, goal.y); // Heuristic cost to goal

            // Create the neighbor node
            Node neighbor(newX, newY, g, h, new Node(current));

            // Push the neighbor to open list
            openList.push(neighbor);
        }
    }

    return {}; // Return empty path if no path found
}

void printPath(const vector<Node>& path) {
    if (path.empty()) {
        cout << "No path found!" << endl;
        return;
    }

    cout << "Path from start to goal:" << endl;
    for (const Node& node : path) {
        cout << "(" << node.x << ", " << node.y << ") ";
    }
    cout << endl;
}

// direction for from_node to face to_node (they need to be side by side)
string faceDirection(Node from_node, Node to_node)
{
    vector<int> dir = {to_node.x - from_node.x, to_node.y - from_node.y};
    if (dir == vector<int>{1, 0}) { return "E"; }
    else if (dir == vector<int>{0, 1}) { return "S"; }
    else if (dir == vector<int>{-1, 0}) { return "W"; }
    else if (dir == vector<int>{0, -1}) { return "N"; }
    else { return "X"; }
}

int codingameMain()
{
    int width;  // columns in the game grid
    int height; // rows in the game grid
    cin >> width >> height; cin.ignore();
    vector<vector<Entity*>> room(height, vector<Entity*>(width, nullptr));

    // game loop
    while (1) {
        int entity_count;
        cin >> entity_count; cin.ignore();
        map<string, vector<Entity*>> entities;
        for (int i = 0; i < entity_count; i++)
        {
            int x;
            int y;            // grid coordinate
            string type;      // 0 WALL, 1 ROOT, 2 BASIC, 3 TENTACLE, 4 HARVESTER, 5 SPORER, 6 A, 7 B, 8 C, 9 D
            int owner;        // 1 if your organ, 0 if enemy organ, -1 if neither
            int organ_id;     // id of this entity if it's an organ, 0 otherwise
            string organ_dir; // N,E,S,W or X if not an organ
            int organ_parent_id;
            int organ_root_id;
            cin >> x >> y >> type >> owner >> organ_id >> organ_dir >> organ_parent_id >> organ_root_id; cin.ignore();
            Entity *new_entity = new Entity(x, y, type, owner, organ_id, organ_dir, organ_parent_id, organ_root_id);
            if (owner == 1)
            {
                entities["MY_ORGAN"].push_back(new_entity);
                entities["MY_" + type].push_back(new_entity);
            }
            else if (owner == 0)
            {
                entities["OPP_ORGAN"].push_back(new_entity);
                entities["OPP_" + type].push_back(new_entity);
            }
            else
            {
                entities[type].push_back(new_entity);
            }
            room[y][x] = new_entity;
        }
        vector<int> my_proteins(4, 0), opp_proteins(4, 0);
        cin >> my_proteins[0] >> my_proteins[1] >> my_proteins[2] >> my_proteins[3]; cin.ignore();     // your protein stock
        cin >> opp_proteins[0] >> opp_proteins[1] >> opp_proteins[2] >> opp_proteins[3]; cin.ignore(); // opponent's protein stock
        int required_actions_count; // your number of organisms, output an action for each one in any order
        cin >> required_actions_count; cin.ignore();
        Entity *grow_from, *grow_to;
        string action = "WAIT";

        // type of protein we're looking for
        string protein_type = "A";

        // growing a HARVESTER if possible (for now only one)
        if (entities.at("MY_HARVESTER").size() == 0 && my_proteins[2] > 0 && my_proteins[3] > 0)
        {
            closestOrgan(grow_from, grow_to, 1, protein_type, entities);
            Node *start = new Node(grow_from->x, grow_from->y, 0, heuristic(grow_from->x, grow_from->y, grow_to->x, grow_to->y));
            Node *goal = new Node(grow_to->x, grow_to->y, 0, 0);
            vector<Node> path = aStar(room, *start, *goal);
            Node harvester_pos = path[path.size()-2];
            string direction = faceDirection(harvester_pos, path[path.size()]);
            action = "GROW " + to_string(grow_from->organ_id) + " " + to_string(harvester_pos.x) + " " + to_string(harvester_pos.y) + " " + direction;
        }
        else if (my_proteins[0] > 0)
        {
            // find the best organ to make
            if (entities.at(protein_type).size() > 0)
            {
                closestOrgan(grow_from, grow_to, 1, protein_type, entities);
            }
            else
            {
                // if there are no more proteins, grow in any empty space (preferably closer to the enemy to block him before)
                // TODO
            }
            action = "GROW " + to_string(grow_from->organ_id) + " " + to_string(grow_to->x) + " " + to_string(grow_to->y) + " " + "N";
        }

        // perform actions
        for (int i = 0; i < required_actions_count; i++)
        {
            cout << action << endl;
        }
    }
}

int main(int argc, char **argv)
{
    int entity_count, width, height, required_actions_count;
    vector<int> my_proteins(4, 0), opp_proteins(4, 0);
    map<string, vector<Entity*>> entities;
    vector<vector<Entity*>> room = readInputFromFile(entity_count, "input_room.txt", width, height, my_proteins, opp_proteins, required_actions_count, entities);
    writeRoomFile(entity_count, "input_room_copy.txt", width, height, room, my_proteins, opp_proteins, required_actions_count);

    printRoom(room);

    // find the best organ to make
    Entity *closest_organ, *closest_protein;
    closestOrgan(closest_organ, closest_protein, 1, "A", entities);

    // grow organ with the chosen organ and protein
    cout << "GROW " << closest_organ->organ_id << " " << closest_protein->x << " " << closest_protein->y << " BASIC" << endl;
    
    printRoom(room);

    return 0;
}