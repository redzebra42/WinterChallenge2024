#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Grow and multiply your organisms to end up larger than your opponent.
 **/

struct Entity
{
    int x, y, id, owner, organ_id, organ_parent_id, organ_root_id;
    string type, organ_dir;
    Entity(int _x, int _y, string _type, int _owner, int _organ_id, string _organ_dir, int _organ_parent_id, int _organ_root_id) : 
    {
        x = _x;
        y = _y;
        type = _type;
        owner = _owner;
        organ_id = _organ_id;
        organ_dir = _organ_dir;
        organ_parent_id = _organ_parent_id;
        organ_root_id = _organ_root_id;
    }
}


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

void readInputFromFile(string &fileName, int &width, int &heigth, vector<int>)

int main(int argc, char **argv)
{
    int width; // columns in the game grid
    int height; // rows in the game grid
    cin >> width >> height; cin.ignore();
}