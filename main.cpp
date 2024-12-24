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

using namespace std;

// TODO change old euclidian distance to accurateDistance
// TODO do a 'protected_tiles' list of tiles (for proteins used by a harvester)
// TODO refactor code for function declaration ...

struct Entity
{
    int x, y, owner, organ_id, organ_parent_id, organ_root_id;
    string type, organ_dir;
    Entity() :
        x(0),
        y(0),
        type("NONE"),
        owner(0),
        organ_id(0),
        organ_dir("NONE"),
        organ_parent_id(0),
        organ_root_id(0) {}
    Entity(int _x, int _y, string _type, int _owner, int _organ_id, string _organ_dir, int _organ_parent_id, int _organ_root_id) : 
        x(_x),
        y(_y),
        type(_type),
        owner(_owner),
        organ_id(_organ_id),
        organ_dir(_organ_dir),
        organ_parent_id(_organ_parent_id),
        organ_root_id(_organ_root_id) {}
};

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

// read room input from file
vector<vector<Entity*>> readInputFromFile(int &entity_count, const string &file_name, int &width, int &height, vector<int> &my_proteins, vector<int> &opp_proteins, int &required_actions_count, map<string, vector<Entity*>> &entities);

// write room data to file
void writeRoomFile(int entity_count, const string &file_name, int width, int height, const vector<vector<Entity*>> &room, const vector<int> &my_proteins, const vector<int> &opp_proteins, int required_actions_count);

string entityToString(Entity *ent);

// print a room in the terminal
void printRoom(const vector<vector<Entity*>> &room, vector<int> my_proteins, vector<int> opp_proteins);

// Euclidian distance squared (because just used for comparing)
int euclDistance(Entity *ent1, Entity *ent2);

// Euclidian distance squared (because just used for comparing)
int euclDistance(pair<int , int> pos1, pair<int, int> ent2);

// a more accurate (but longer to calcuate) distance based on A*
int accurateDistance(const Entity &ent1, const Entity &ent2);

// organ and protein closest to each other (1:my, 0:opp)
int closestProtein(Entity *&closest_organ, Entity *&closest_protein, int player, const string &protein_type, const map<string, vector<Entity*>> &entities);

// modifies action to start from pres_pos organ
string fromPreviousOrgan(const string &action, pair<int, int> prev_pos, const vector<vector<Entity*>> &room);

pair<int, int> actionToPosition(const string &action);

// builds the queue to grow an harvestor
queue<string> growHarvestor(std::vector<Node> &path, Entity *grow_from);

// appends 2 queues
void pushQueue(queue<string> &main_queue, queue<string> &to_push_queue);

// returns if a tile is free
bool isFree(pair<int, int> tile, vector<vector<Entity*>> room);

// Manhattan distance heuristic
float heuristic(int x1, int y1, int x2, int y2);

// A* algorithm
vector<Node> aStar(vector<vector<Entity*>> &grid, Node start, Node goal);

void printPath(const vector<Node>& path);

// direction for from_node to face to_node (they need to be side by side)
string faceDirection(Node from_node, Node to_node);

// return an empty space next to the organ, and {-1, -1} if there isn't one
pair<int, int> nextEmptySpace(vector<vector<Entity*>> room, map<string, vector<Entity*>> entities, Entity *&from_organ, vector<pair<int, int>> protected_tiles);

//initialise the enities map with empty vectors
void initEntities(map<string, vector<Entity*>> &entities);

// closest entity to start_ent (returns the euclidian distance squared)
int closestEntity(pair<int, int> start_pos, Entity *&closest_entity, string entity_type, const map<string, vector<Entity*>> &entities);

// turns a pair such as {1, 0} into a direction such as "S"
string pairToDir(pair<int, int> dir_pair);

// checks if any protein is in the game
bool proteinLeft(const map<string, vector<Entity*>> &entities);

// checks if sporer is needed, and if so changes sporer_pos, spore_pos and dir
bool needSporer(Entity *closest_organ, Entity *closest_protein, string &dir, pair<int, int> &sporer_pos, pair<int, int> &spore_pos, const vector<vector<Entity*>> &room);

void growBasicOrTentacle(std::pair<int, int> &grow_to_pos, std::vector<std::vector<Entity *>> &room, std::map<std::string, std::vector<Entity *>> &entities, Entity *&grow_from, std::vector<std::pair<int, int>> &protected_tiles, std::string &grow_type, std::string &str_dir);

int codingameMain()
{
    int width;  // columns in the game grid
    int height; // rows in the game grid
    cin >> width >> height; cin.ignore();
    vector<vector<Entity*>> room(height, vector<Entity*>(width, nullptr));
    queue<string> action_queue;
    Entity *previous_entity;
    pair<int, int> previous_position =  pair<int, int>{-1, -1};
    vector<pair<int, int>> protected_tiles;

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
            // reset previous_position (path is finished)
            previous_position = pair<int, int>{-1, -1};

            // growing a HARVESTER if possible (for now only one)
            if (proteinLeft(entities) && entities.at("MY_HARVESTER").size() == 0 && my_proteins[2] > 0 && my_proteins[3] > 0)
            {
                closestProtein(grow_from, grow_to, 1, protein_type, entities);
                Node *start = new Node(grow_from->x, grow_from->y, 0, heuristic(grow_from->x, grow_from->y, grow_to->x, grow_to->y));
                Node *goal = new Node(grow_to->x, grow_to->y, 0, 0);
                vector<Node> path = aStar(room, *start, *goal);
                protected_tiles.push_back(pair<int, int>{goal->x, goal->y});
                if (path.size() > 1)
                {
                    queue<string> harv_queue = growHarvestor(path, grow_from);
                    pushQueue(action_queue, harv_queue);
                }
                else
                {
                    // if the prootein is already neighbouring (could probably be improved by checking if surroudings are free, but too rare)
                    action_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(path[0].x) + " " + to_string(path[0].y) + " BASIC " +  "N");
                }
            }
            else if (my_proteins[0] > 0)
            {
                pair<int, int> grow_to_pos;
                string grow_type = "BASIC";
                string str_dir = "N";

                // if there are still 2B, 2D, 1A, 1C, at least and closest protien is far, then grow a sporer
                if (my_proteins[1] >= 2 && my_proteins[2] >= 1 && my_proteins[3] >= 2)
                {
                    Entity *closest_organ, *closest_protein;
                    pair<int, int> sporer_position, spore_position;
                    int dist = closestProtein(closest_organ, closest_protein, 1, protein_type, entities);
                    if (needSporer(closest_organ, closest_protein, str_dir, sporer_position, spore_position, room))
                    {
                        action_queue.push("GROW " + to_string(closest_organ->organ_id) + " " + to_string(sporer_position.first) + " " + to_string(sporer_position.second) + " SPORER " + str_dir);
                        action_queue.push("SPORE NONE " + to_string(spore_position.first) + " " + to_string(spore_position.second));
                    }
                    else
                    {
                        growBasicOrTentacle(grow_to_pos, room, entities, grow_from, protected_tiles, grow_type, str_dir);
                    }
                }
                else
                {
                    growBasicOrTentacle(grow_to_pos, room, entities, grow_from, protected_tiles, grow_type, str_dir);
                }

                // grow the organ
                action_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(grow_to_pos.first) + " " + to_string(grow_to_pos.second) + " " + grow_type + " " + str_dir);
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

void growBasicOrTentacle(std::pair<int, int> &grow_to_pos, std::vector<std::vector<Entity *>> &room, std::map<std::string, std::vector<Entity *>> &entities, Entity *&grow_from, std::vector<std::pair<int, int>> &protected_tiles, std::string &grow_type, std::string &str_dir)
{
    // if there are no more C or D proteins, grow in any empty space (preferably closer to the enemy to block him before)
    grow_to_pos = nextEmptySpace(room, entities, grow_from, protected_tiles);

    // check if enemy nearby to grow tentacles
    Entity *closest_enemy;
    int dist = closestEntity(grow_to_pos, closest_enemy, "OPP_ORGAN", entities);
    if (dist <= 4)
    {
        grow_type = "TENTACLE";
        pair<int, int> direction = pair<int, int>{(closest_enemy->x - grow_to_pos.first) / sqrt(dist), (closest_enemy->y - grow_to_pos.second) / sqrt(dist)};
        str_dir = pairToDir(direction);
        protected_tiles.push_back(pair<int, int>{grow_to_pos.first + direction.first, grow_to_pos.first + direction.first});
    }
}

// checks if sporer is needed, and if so changes sporer_pos, spore_pos and dir
bool needSporer(Entity *closest_organ, Entity *closest_protein, string &dir, pair<int, int> &sporer_pos, pair<int, int> &spore_pos, const vector<vector<Entity*>> &room)
{
    bool res;
    int dist_x = closest_protein->x - closest_organ->x;
    int dist_y = closest_protein->y - closest_organ->y;
    res = abs(dist_x) > 5 || abs(dist_y) > 5;
    if (res)
    {
        if (abs(dist_x) >= abs(dist_y))
        {
            int sign_x = (dist_x > 0) - (dist_x < 0);

            // the tile for the sporer should be free of my_organ type, but check for other
            if (isFree(pair<int, int>{closest_organ->x + sign_x, closest_organ->y}, room))
            {
                sporer_pos = pair<int, int>{closest_organ->x + sign_x, closest_organ->y};
                spore_pos = pair<int, int>{closest_protein->x - sign_x, closest_organ->y};
                dir = pairToDir(pair<int, int>{sign_x, 0});
            }
            else
            {
                // if space not free, abort
                res = false;
            }
        }
        else
        {
            int sign_y = (dist_y > 0) - (dist_y < 0);

            // the tile for the sporer should be free of my_organ type, but check for other
            if (isFree(pair<int, int>{closest_organ->x, closest_organ->y + sign_y}, room))
            {
                sporer_pos = pair<int, int>{closest_organ->x, closest_organ->y + sign_y};
                spore_pos = pair<int, int>{closest_organ->x, closest_protein->y - sign_y};
                dir = pairToDir(pair<int, int>{0, sign_y});
            }
            else
            {
                // if space not free, abort
                res = false;
            }
        }
    }
    return res;
}

// checks if any protein is in the game
bool proteinLeft(const map<string, vector<Entity*>> &entities)
{
    return entities.at("A").size() > 0 && entities.at("B").size() > 0 && entities.at("C").size() > 0 && entities.at("D").size() > 0;
}

// turns a pair such as {1, 0} into a direction such as "S"
string pairToDir(pair<int, int> dir_pair)
{
    if (dir_pair == pair<int, int>{1, 0}) { return "E"; }
    else if (dir_pair == pair<int, int>{-1, 0}) { return "W"; }
    else if (dir_pair == pair<int, int>{0, 1}) { return "S"; }
    else if (dir_pair == pair<int, int>{0, -1}) { return "N"; }
    else 
    {
        cerr << "the given direction pair isn't correct" << endl;
        return "X";
    }
}

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
pair<int, int> nextEmptySpace(vector<vector<Entity*>> room, map<string, vector<Entity*>> entities, Entity *&from_organ, vector<pair<int, int>> protected_tiles)
{
    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (Entity *organ : entities.at("MY_ORGAN"))
    {
        for (const pair<int, int> dir : directions)
        {
            pair<int, int> new_tile = {organ->x+dir.first, organ->y+dir.second};
            if (isFree(new_tile, room) && count(protected_tiles.begin(), protected_tiles.end(), new_tile) == 0)
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

string fromPreviousOrgan(const string &action, pair<int, int> prev_pos, const vector<vector<Entity*>> &room)
{
    string new_action;
    stringstream act_stream(action);
    string tmp, action_type;
    act_stream >> action_type; // action type
    new_action = action_type + " ";
    act_stream >> tmp; // id
    new_action += to_string(room[prev_pos.second][prev_pos.first]->organ_id) + " ";
    act_stream >> tmp; // x
    new_action += tmp + " ";
    act_stream >> tmp; // y
    new_action += tmp + " ";
    if (action_type == "GROW")
    {
        act_stream >> tmp; // organ type
        new_action += tmp + " ";
        act_stream >> tmp; // direction
        new_action += tmp;
    }
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

queue<string> growHarvestor(std::vector<Node> &path, Entity *grow_from)
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

string entityToString(Entity *ent)
{
    if (ent == nullptr) {return " ";}
    else if (ent->type == "WALL") {return "X";}
    else if (ent->type == "ROOT" || ent->type == "BASIC") {return to_string(ent->owner);}
    else {return ent->type;}
}

// print a room in the terminal
void printRoom(const vector<vector<Entity*>> &room, vector<int> my_proteins, vector<int> opp_proteins)
{
    int height = room.size();
    int width = room[0].size();
    cout << "my prot: " << my_proteins[0] << " " << my_proteins[1] << " " << my_proteins[2] << " " << my_proteins[3] << endl;
    cout << "opp prot:" << opp_proteins[0] << " " << opp_proteins[1] << " " << opp_proteins[2] << " " << opp_proteins[3] << endl;
    cout << "-";
    for (int i=0; i<width; i++)
    {
        cout << "----";
    }
    cout << endl;
    for (int j=0; j<height; j++)
    {
        cout << "|";
        for (int i=0; i<width; i++)
        {
            cout << " " << entityToString(room[j][i]) << " |";
        }
        cout << endl << "-";
        for (int k=0; k<width; k++)
        {
            cout << "----";
        }
        cout << endl;
    }
}

// Euclidian distance squared (because just used for comparing)
int euclDistance(Entity *ent1, Entity *ent2)
{
    if (ent1 && ent2)
    {
        return (pow(ent1->x - ent2->x, 2) + pow(ent1->y - ent2->y, 2));
    }
    else
    {
        cerr << "nullptr in eulclDistance not valid" << endl;
        return 1000;
    }
}

// Euclidian distance squared (because just used for comparing)
int euclDistance(pair<int, int> pos1, pair<int, int> pos2)
{
    return (pow(pos1.first - pos2.first, 2) + pow(pos1.second - pos2.second, 2));
}

// a more accurate (but longer to calcuate) distance based on A*
int accurateDistance(const Entity &ent1, const Entity &ent2)
{
    return 1;// TODO
}

// organ and protein closest to each other (1:my, 0:opp)
int closestProtein(Entity *&closest_organ, Entity *&closest_protein, int player, const string &protein_type, const map<string, vector<Entity*>> &entities)
{
    int dist_min = abs(numeric_limits<int>::max());
    vector<Entity*> organs;
    if (player == 1)
    {
        organs = entities.at("MY_ORGAN"); // maybe error because of const
    }
    else if (player == 0)
    {
        organs = entities.at("OPP_ORGAN"); // maybe error because of const
    }
    else 
    {
        cerr << "player should be 0 or 1" << endl;
    }
    vector<Entity*> proteins = entities.at(protein_type);
    for (Entity *organ : organs)
    {
        for (Entity *protein : proteins)
        {
            int new_dist = euclDistance(organ, protein);
            if (new_dist < dist_min)
            {
                dist_min = new_dist;
                closest_organ = organ;
                closest_protein = protein;
            }
        }
    }
    return dist_min;
}

// closest entity to start_ent (returns the euclidian distance squared)
int closestEntity(pair<int, int> start_pos, Entity *&closest_entity, string entity_type, const map<string, vector<Entity*>> &entities)
{
    int dist_min = abs(numeric_limits<int>::max());
       
    vector<Entity*> target_entities = entities.at(entity_type);
    for (Entity *ent : target_entities)
    {
        int new_dist = euclDistance(start_pos, pair<int, int>{ent->x, ent->y});
        if (new_dist < dist_min)
        {
            dist_min = new_dist;
            closest_entity = ent;
        }
    }
    return dist_min;
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
    Entity *previous_entity;
    pair<int, int> previous_position =  pair<int, int>{-1, -1};
    vector<pair<int, int>> protected_tiles;

    printRoom(room, my_proteins, opp_proteins);

    // type of protein we're looking for
    string protein_type = "A";

    // do an action only if the queue is empty
    if (action_queue.size() == 0)
    {
        // growing a HARVESTER if possible (for now only one)
        if (entities.at("MY_HARVESTER").size() == 0 && my_proteins[2] > 0 && my_proteins[3] > 0)
        {
            closestProtein(grow_from, grow_to, 1, protein_type, entities);
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
                closestProtein(grow_from, grow_to, 1, protein_type, entities);
                action_queue.push("GROW " + to_string(grow_from->organ_id) + " " + to_string(grow_to->x) + " " + to_string(grow_to->y) + " BASIC " + "N");
            }
            else
            {
                // if there are no more proteins, grow in any empty space (preferably closer to the enemy to block him before)
                pair<int, int> grow_to_pos = nextEmptySpace(room, entities, grow_from, protected_tiles);
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

vector<vector<Entity*>> readInputFromFile(int &entity_count, const string &file_name, int &width, int &height, vector<int> &my_proteins, vector<int> &opp_proteins, int &required_actions_count, map<string, vector<Entity*>> &entities)
{
    ifstream room_file (file_name);
    if (room_file)
    {
        room_file >> width >> height;
        vector<vector<Entity*>> room(height, vector<Entity*>(width, nullptr));
        room_file >> entity_count;
        for (int i = 0; i < entity_count; i++) {
            int x;
            int y; // grid coordinate
            string type; // WALL, ROOT, BASIC, TENTACLE, HARVESTER, SPORER, A, B, C, D
            int owner; // 1 if your organ, 0 if enemy organ, -1 if neither
            int organ_id; // id of this entity if it's an organ, 0 otherwise
            string organ_dir; // N,E,S,W or X if not an organ
            int organ_parent_id;
            int organ_root_id;
            room_file >> x >> y >> type >> owner >> organ_id >> organ_dir >> organ_parent_id >> organ_root_id;
            Entity* new_entity = new Entity(x, y, type, owner, organ_id, organ_dir, organ_parent_id, organ_root_id);
            if (owner == 1)
            {
                entities["MY_ORGAN"].push_back(new_entity);
            }
            else if (owner == 0)
            {
                entities["OPP_ORGAN"].push_back(new_entity);
            }
            else
            {
                entities[type].push_back(new_entity);
            }
            room[y][x] = new_entity;
        }
        room_file >> my_proteins[0] >> my_proteins[1] >> my_proteins[2] >> my_proteins[3];
        room_file >> opp_proteins[0] >> opp_proteins[1] >> opp_proteins[2] >> opp_proteins[3];
        room_file >> required_actions_count;
        return room;
    }
    else
    {
        vector<vector<Entity*>> room;
        cerr << "File didn't open" << endl;
        return room;
    }
}

void writeRoomFile(int entity_count, const string &file_name, int width, int height, const vector<vector<Entity*>> &room, const vector<int> &my_proteins, const vector<int> &opp_proteins, int required_actions_count)
{
    ofstream room_file (file_name);
    if (room_file)
    {
        room_file << width << " " << height << endl;
        room_file << entity_count << endl;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                if (room[j][i])
                {
                    room_file << i << " " << j << " " << room[j][i]->type << " " << room[j][i]->owner << " " << room[j][i]->organ_id << " " << room[j][i]->organ_dir << " " << room[j][i]->organ_parent_id << " " << room[j][i]->organ_root_id << endl;
                }
            }
        }
        room_file << my_proteins[0] << " " << my_proteins[1] << " " << my_proteins[2] << " " << my_proteins[3] << endl;
        room_file << opp_proteins[0] << " " << opp_proteins[1] << " " << opp_proteins[2] << " " << opp_proteins[3] << endl;
        room_file << required_actions_count << endl;
    }
    else
    {
        cerr << "File didn't open (in the write function)" << endl;
    }
}