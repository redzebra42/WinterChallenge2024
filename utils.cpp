#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <cmath>

#include "utils.h" 

using namespace std;

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

// Euclidian distance squared (because just used for comparing)
int distance(const Entity &ent1, const Entity &ent2)
{
    return (pow(ent1.x - ent2.x, 2) + pow(ent1.y - ent2.y, 2));
}

// organ closest to protein (1:my, 0:opp)
Entity *closestOrgan(int player, const string &protein_type, const map<string, vector<Entity*>> &entities)
{
    Entity *closest_organ = nullptr;
    int dist_min = numeric_limits<int>::max();
    vector<Entity*> organs;
    if (player == 1)
    {
        organs = entities.at("MY_ORGAN"); // maybe error because of const
    }
    else
    {
        organs = entities.at("OPP_ORGAN"); // maybe error because of const
    }
    vector<Entity*> proteins = entities.at(protein_type);

    // TODO itterate through the things to find the smallest distance

    return closest_organ;
}
