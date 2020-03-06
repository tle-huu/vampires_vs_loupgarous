#include <iostream>
#include "include/Map.h"
#include <string.h>
#include "utils.h"

using namespace std;

void play(Map* map);

int main(){
    play(NULL);
}


void play(Map* map){
    cout <<"Saltu"<<endl;
    while(1){
        string move;
        cin >> move;
        std::vector<string> sub = split(move, 'x');
        for(int i = 0; i<sub.size(); i++){
            cout<<sub[i]<<endl;
        }
    }
}