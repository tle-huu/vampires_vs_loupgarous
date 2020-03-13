#include <iostream>
#include <iomanip>

#include "include/Map.h"
#include "include/MapStruct.h"
#include <string.h>
#include "utils.h"

using namespace std;

void play(Map* map);

void printMap(Map* map){
    for(int i = 0; i<map->columns(); i++){
        for(int j = 0; j < map->lines(); j++){
            if(map->getCell(i,j).number==0){
                cout<<"    N";
            }
            else{
                cout<<" "<<setw(3)<<map->getCell(i,j).number<<setw(1)<<map->getCell(i,j).element;
            }
        }
        cout<<endl;
    }
}

int main(){
    MapStruct mg(10,10);
    mg.add_gentil(1,1,16);
    mg.add_vilain(2,2, 8);
    play(&mg);
}



void play(Map* map){
    cout <<"Saltu"<<endl;
    while(1){
        printMap(map);
        string move;
        cin >> move;
        Action act(move);
        
        map->result(act);
        

    }
}