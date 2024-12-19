#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

/**
 * TODO refactor code with utils for fonctions not useful in the codingame version
 **/

struct Entity
{
    int x, y, id, owner, organ_id, organ_parent_id, organ_root_id;
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


int codingameMain()
{
    int width; // columns in the game grid
    int height; // rows in the game grid
    cin >> width >> height; cin.ignore();

    // game loop
    while (1) {
        int entity_count;
        cin >> entity_count; cin.ignore();
        for (int i = 0; i < entity_count; i++) {
            int x;
            int y; // grid coordinate
            string type; // 0 WALL, 1 ROOT, 2 BASIC, 3 TENTACLE, 4 HARVESTER, 5 SPORER, 6 A, 7 B, 8 C, 9 D
            int owner; // 1 if your organ, 0 if enemy organ, -1 if neither
            int organ_id; // id of this entity if it's an organ, 0 otherwise
            string organ_dir; // N,E,S,W or X if not an organ
            int organ_parent_id;
            int organ_root_id;
            cin >> x >> y >> type >> owner >> organ_id >> organ_dir >> organ_parent_id >> organ_root_id; cin.ignore();
        }
        int my_a;
        int my_b;
        int my_c;
        int my_d; // your protein stock
        cin >> my_a >> my_b >> my_c >> my_d; cin.ignore();
        int opp_a;
        int opp_b;
        int opp_c;
        int opp_d; // opponent's protein stock
        cin >> opp_a >> opp_b >> opp_c >> opp_d; cin.ignore();
        int required_actions_count; // your number of organisms, output an action for each one in any order
        cin >> required_actions_count; cin.ignore();
        for (int i = 0; i < required_actions_count; i++) {

            // Write an action using cout. DON'T FORGET THE "<< endl"
            // To debug: cerr << "Debug messages..." << endl;

            cout << "WAIT" << endl;
        break;
        }
    }
}

vector<vector<Entity*>> readInputFromFile(int &entity_count, const string &file_name, int &width, int &height, vector<int> &my_proteins, vector<int> &opp_proteins, int &required_actions_count)
{
    ifstream room_file (file_name);
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
        room[y][x] = new_entity;
    }
    room_file >> my_proteins[0] >> my_proteins[1] >> my_proteins[2] >> my_proteins[3];
    room_file >> opp_proteins[0] >> opp_proteins[1] >> opp_proteins[2] >> opp_proteins[3];
    room_file >> required_actions_count;
    return room;
}

void writeRoomFile(int entity_count, const string &file_name, int width, int height, const vector<vector<Entity*>> &room, const vector<int> &my_proteins, const vector<int> &opp_proteins, int required_actions_count)
{
    ofstream room_file (file_name);
    room_file << width << height << endl;
    room_file << entity_count << endl;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (room[j][i])
            {
                room_file << i << j << room[j][i]->type << room[j][i]->owner << room[j][i]->organ_id << room[j][i]->organ_dir << room[j][i]->organ_parent_id << room[j][i]->organ_root_id << endl;
            }
        }
    }
    room_file << my_proteins[0] << my_proteins[1] << my_proteins[2] <<my_proteins[3] << endl;
    room_file << opp_proteins[0] << opp_proteins[1] << opp_proteins[2] << opp_proteins[3] << endl;
    room_file << required_actions_count << endl;
}

int main(int argc, char **argv)
{
    int entity_count, width, height, required_actions_count;
    vector<int> my_proteins(0, 4), opp_proteins(0, 4);
    vector<vector<Entity*>> room = readInputFromFile(entity_count, "input_room.txt", width, height, my_proteins, opp_proteins, required_actions_count);
    writeRoomFile(entity_count, "input_room_copy", width, height, room, my_proteins, opp_proteins, required_actions_count);
    return 0;
}