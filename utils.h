//#ifdef UTILS_H


#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

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

// read room input from file
vector<vector<Entity*>> readInputFromFile(int &entity_count, const string &file_name, int &width, int &height, vector<int> &my_proteins, vector<int> &opp_proteins, int &required_actions_count, map<string, vector<Entity*>> &entities);

// write room data to file
void writeRoomFile(int entity_count, const string &file_name, int width, int height, const vector<vector<Entity*>> &room, const vector<int> &my_proteins, const vector<int> &opp_proteins, int required_actions_count);

string entityToString(Entity *ent);

// print a room in the terminal
void printRoom(const vector<vector<Entity*>> &room);

// euclidian distance squared (because just used for comparing)
int distance(const Entity &ent1, const Entity &ent2);

// a more accurate (but longer to calcuate) distance based on A*
int accurateDistance(const Entity &ent1, const Entity &ent2);

// organ and protein closest to each other (1:my, 0:opp)
int closestOrgan(Entity *&closest_organ, Entity *&closest_protein, int player, const string &protein_type, const map<string, vector<Entity*>> &entities);

//#endif
