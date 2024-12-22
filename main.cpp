#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <sstream>

#include "utils.h"

using namespace std;

// TODO change old euclidian distance to accurateDistance
// TODO do a 'no touch' list of tiles (for proteins used by a harvester)
// TODO refactor code for function declaration ...

// returns if a tile is free
bool isFree(pair<int, int> tile, vector<vector<Entity*>> room)
{
    if (room[tile.second][tile.first])
    {
        return room[tile.second][tile.first]->owner == -1 && room[tile.second][tile.first]->type != "WALL";
    }
    else 
    {
        return true;
    }
}

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
vector<Node> aStar(vector<vector<Entity*>> &grid, Node start, Node goal) {
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
            if (newX < 0 || newX >= rows || newY < 0 || newY >= cols || !isFree(pair<int, int>{newX, newY}, grid) || closedList[newX][newY]) {
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
    int diff_x = to_node.x - from_node.x;
    int diff_y = to_node.y - from_node.y;
    if (diff_x == 1 && diff_y == 0) { return "E"; }
    else if (diff_x == 0 && diff_y == 1) { return "S"; }
    else if (diff_x == -1 && diff_y == 0) { return "W"; }
    else if (diff_x == 0 && diff_y == -1) { return "N"; }
    else { return "X"; }
}

// return an empty space next to the organ, and {-1, -1} if there isn't one
pair<int, int> nextEmptySpace(vector<vector<Entity*>> room, map<string, vector<Entity*>> entities, Entity *&from_organ)
{
    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (Entity *organ : entities.at("MY_ORGAN"))
    {
        for (const pair<int, int> dir : directions)
        {
            pair<int, int> new_tile = {organ->x+dir.first, organ->y+dir.second};
            if (isFree(new_tile, room))
            {
                from_organ = organ;
                return new_tile;
            }
        }
    }
    return pair<int, int>{-1, -1};
}

//initialise the enities map with empty vectors
void initEntities(map<string, vector<Entity*>> &entities)
{
    for (string type : vector<string>{"WALL",
                                      "MY_ROOT",
                                      "MY_BASIC",
                                      "MY_TENTACLE",
                                      "MY_HARVESTER",
                                      "MY_SPORER",
                                      "OPP_ROOT",
                                      "OPP_BASIC",
                                      "OPP_TENTACLE",
                                      "OPP_HARVESTER",
                                      "OPP_SPORER",
                                      "MY_ORGAN",
                                      "OPP_ORGAN",
                                      "A",
                                      "B",
                                      "C",
                                      "D"})
        entities[type] = vector<Entity*>{};
}

string buildPath(vector<Node> path, vector<vector<Entity*>> room)
{

}

string fromPreviousOrgan(const string &action, pair<int, int> prev_pos, const vector<vector<Entity*>> &room);
pair<int, int> actionToPosition(const string &action);
queue<string> growHarvestor(std::vector<Node> &path, Entity *grow_from);
void pushQueue(queue<string> queue);

int codingameMain()
{
    int width;  // columns in the game grid
    int height; // rows in the game grid
    cin >> width >> height; cin.ignore();
    vector<vector<Entity*>> room(height, vector<Entity*>(width, nullptr));
    queue<string> action_queue;
    Entity *previous_entity;
    pair<int, int> previous_position =  pair<int, int>{-1, -1};

    // game loop
    while (1) {
        int entity_count;
        cin >> entity_count; cin.ignore();
        map<string, vector<Entity*>> entities;
        initEntities(entities);
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

        // type of protein we're looking for
        string protein_type = "A";

        // do an action only if the queue is empty
        if (action_queue.size() == 0)
        {
            // growing a HARVESTER if possible (for now only one)
            if (entities.at("MY_HARVESTER").size() == 0 && my_proteins[2] > 0 && my_proteins[3] > 0)
            {
                closestOrgan(grow_from, grow_to, 1, protein_type, entities);
                Node *start = new Node(grow_from->x, grow_from->y, 0, heuristic(grow_from->x, grow_from->y, grow_to->x, grow_to->y));
                Node *goal = new Node(grow_to->x, grow_to->y, 0, 0);
                vector<Node> path = aStar(room, *start, *goal);
                if (path.size() > 1)
                {
                    queue<string> herv_queue = growHarvestor(path, grow_from);
                    pushQueue(action_queue, herv_queue);
                }
                else
                {
                    // if the prootein is already neighbouring (could probably be improved by checking if surroudings are free, but too rare)
                    action_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(path[0].x) + " " + to_string(path[0].y) + " BASIC N");
                }
            }
            else if (my_proteins[0] > 0)
            {
                // find the best organ to make
                if (entities.at(protein_type).size() > 0)
                {
                    closestOrgan(grow_from, grow_to, 1, protein_type, entities);
                    action_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(grow_to->x) + " " + to_string(grow_to->y) + " BASIC " + "N");
                }
                else
                {
                    // if there are no more proteins, grow in any empty space (preferably closer to the enemy to block him before)
                    pair<int, int> grow_to_pos = nextEmptySpace(room, entities, grow_from);
                    action_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(grow_to_pos.first) + " " + to_string(grow_to_pos.second) + " BASIC " + "N");
                }
            }
        }

        // perform actions
        for (int i = 0; i < required_actions_count; i++)
        {
            if (action_queue.size() > 0)
            {
                string curr_action = action_queue.front();
                if (previous_position != pair<int, int>{-1, -1})
                {
                    curr_action = fromPreviousOrgan(curr_action, previous_position, room);
                }
                cout << curr_action << endl;
                previous_position = actionToPosition(curr_action);
                action_queue.pop();
            }
            else
            {
                cout << "WAIT" << endl;
            }
        }
    }
}

string fromPreviousOrgan(const string &action, pair<int, int> prev_pos, const vector<vector<Entity*>> &room)
{
    string new_action;
    stringstream act_stream(action);
    string tmp;
    act_stream >> tmp; // action type
    new_action = tmp + " ";
    act_stream >> tmp; // id
    new_action += to_string(room[prev_pos.second][prev_pos.first]->organ_id) + " ";
    act_stream >> tmp; // x
    new_action += tmp + " ";
    act_stream >> tmp; // y
    new_action += tmp + " ";
    act_stream >> tmp; // organ type
    new_action += tmp + " ";
    act_stream >> tmp; // direction
    new_action += tmp;
    return new_action;
}

pair<int, int> actionToPosition(const string &action)
{
    stringstream act_stream(action);
    int x, y;
    string tmp;
    act_stream >> tmp; // action type
    act_stream >> tmp; // id
    act_stream >> x;   // x
    act_stream >> y;   // y
    return pair<int, int>{x, y};
}

queue<string> growHarvestor(std::vector<Node> &path, const Entity *&grow_from)
{
    queue<string> act_queue;
    Node harvester_pos = path[path.size() - 2];
    string direction = faceDirection(harvester_pos, path[path.size() - 1]);
    if (path.size() > 2)
    {
        // grow the basic path before putting the harvester
        for (int i = 1; i < path.size() - 2; i++)
        {
            act_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(path[i].x) + " " + to_string(path[i].y) + " BASIC N");
        }
    }
    // grow the harvester
    act_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(harvester_pos.x) + " " + to_string(harvester_pos.y) + " HARVESTER " + direction);
    return act_queue;
}

void pushQueue(queue<string> &main_queue, queue<string> &to_push_queue)
{
    while (to_push_queue.size() > 0)
    {
        main_queue.push(to_push_queue.front());
        to_push_queue.pop();
    }
}

int main(int argc, char **argv)
{
    int entity_count, width, height, required_actions_count;
    vector<int> my_proteins(4, 0), opp_proteins(4, 0);
    map<string, vector<Entity*>> entities;
    initEntities(entities);
    vector<vector<Entity*>> room = readInputFromFile(entity_count, "input_room.txt", width, height, my_proteins, opp_proteins, required_actions_count, entities);
    writeRoomFile(entity_count, "input_room_copy.txt", width, height, room, my_proteins, opp_proteins, required_actions_count);
    Entity *grow_from, *grow_to;
    queue<string> action_queue;

    printRoom(room, my_proteins, opp_proteins);

    // type of protein we're looking for
    string protein_type = "A";

    // do an action only if the queue is empty
    if (action_queue.size() == 0)
    {
        // growing a HARVESTER if possible (for now only one)
        if (entities.at("MY_HARVESTER").size() == 0 && my_proteins[2] > 0 && my_proteins[3] > 0)
        {
            closestOrgan(grow_from, grow_to, 1, protein_type, entities);
            Node *start = new Node(grow_from->x, grow_from->y, 0, heuristic(grow_from->x, grow_from->y, grow_to->x, grow_to->y));
            Node *goal = new Node(grow_to->x, grow_to->y, 0, 0);
            vector<Node> path = aStar(room, *start, *goal);
            cout << start->x << " " << start->y << "  " << goal->x << " " << goal->y << endl;
            printPath(path);
            if (path.size() > 1)
            {
                Node harvester_pos = path[path.size()-2];
                string direction = faceDirection(harvester_pos, path[path.size()-1]);
                if (path.size() > 2)
                {
                    // grow the basic path before putting the harvester
                    for (int i=1; i<path.size()-2; i++)
                    {
                        action_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(path[i].x) + " " + to_string(path[i].y) + " BASIC N");
                    }
                }
                // grow the harvester
                action_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(harvester_pos.x) + " " + to_string(harvester_pos.y) + " HARVESTER " + direction);
            }
            else
            {
                // if the prootein is already neighbouring (could probably be improved by checking if surroudings are free, but too rare)
                action_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(path[0].x) + " " + to_string(path[0].y) + " BASIC N");
            }
        }
        else if (my_proteins[0] > 0)
        {
            // find the best organ to make
            if (entities.at(protein_type).size() > 0)
            {
                closestOrgan(grow_from, grow_to, 1, protein_type, entities);
                action_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(grow_to->x) + " " + to_string(grow_to->y) + " BASIC " + "N");
            }
            else
            {
                // if there are no more proteins, grow in any empty space (preferably closer to the enemy to block him before)
                pair<int, int> grow_to_pos = nextEmptySpace(room, entities, grow_from);
                action_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(grow_to_pos.first) + " " + to_string(grow_to_pos.second) + " BASIC " + "N");
            }
        }
    }

    // perform actions
    for (int i = 0; i < required_actions_count; i++)
    {
        if (action_queue.size() > 0)
        {
            cout << action_queue.front() << endl;
            action_queue.pop();
        }
        else
        {
            cout << "WAIT" << endl;
        }
    }
    

    printRoom(room, my_proteins, opp_proteins);

    return 0;
}