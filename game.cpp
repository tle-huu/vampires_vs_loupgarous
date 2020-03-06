#include <iostream>
#include "include/Map.h"
#include <string.h>
#include "utils.h"

using namespace std;

void play(Map* map);

int main(){
    MapGrid mg(10,10);
    mg.add_gentil(1,1,8);
    mg.add_vilain(9,9, 8);
    play(&mg);
}


void play(Map* map){
    cout <<"Saltu"<<endl;
    while(1){
        string move;
        cin >> move;
        Action act(move);

        map->result(act);
        for(int i = 0; i<map->columns(); i++){
            for(int j = 0; j < map->lines(); j++){
                cout<<map->getCell(i,j).element;
            }
            cout<<endl;
            for(int j = 0; j < map->lines(); j++){
                cout<<map->getCell(i,j).number;
            }
            cout<<endl;
        }

    }
}