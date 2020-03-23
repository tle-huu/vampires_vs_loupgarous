
BUILD_DIR := build_linux/

OBJECT := $(BUILD_DIR)game.o
OBJECT += $(BUILD_DIR)src/Action.o
OBJECT += $(BUILD_DIR)src/Group.o
OBJECT += $(BUILD_DIR)src/Map.o
OBJECT += $(BUILD_DIR)src/minimax.o
OBJECT += $(BUILD_DIR)src/Point.o
OBJECT += $(BUILD_DIR)src/utils.o
OBJECT += $(BUILD_DIR)src/State.o
OBJECT += $(BUILD_DIR)src/MapStruct.o

LDFLAGS := -lpython3.6m

IFLAGS := -I "/usr/include/python3.6" -I "include"

all: exec

exec: $(OBJECT)
	g++ $(OBJECT) -o exec $(LDFLAGS)

clean:
	rm -f $(OBJECT)
	rm -f exec

$(BUILD_DIR)%.o : %.cpp
	g++ -o $@ -c $< $(IFLAGS) -g