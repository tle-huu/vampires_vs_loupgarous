VAMPIRES Vs WEREWOLVES

To link the C++ scripts to Python code on Windows :
 - "python setup.py build" to compile c++ code
 - "py -m pip install ." to build python library

To compile the c++ code : (not working)
g++ -Wall -std=c++17 -c src/Point.cpp -o obj/Point.o | g++ -Wall -std=c++17 -c src/Group.cpp -o obj/Group.o | g++ -Wall -std=c++17 -c src/Action.cpp -o obj/Action.o | g++ -Wall -std=c++17 -c src/Map.cpp -o obj/Map.o | g++ -Wall -std=c++17 -c src/State.cpp -o obj/State.o | g++ -Wall -std=c++17 -c src/utils.cpp -o obj/utils.o | g++ -Wall -std=c++17 -c src/minimax.cpp -o obj/minimax.o | g++ -Wall -std=c++17 -c src/main.cpp -o obj/main.o | g++ -Wall -std=c++17 -lws2_32 -c src/Client.cpp -o obj/Client.o | g++ -Wall -std=c++17 -lws2_32 -o bin/twilight obj/main.o obj/Point.o obj/Group.o obj/Action.o obj/Map.o obj/State.o obj/utils.o obj/minimax.o obj/Client.o
