SRC := src
BUILD := build
OBJ := $(BUILD)/obj
BIN := bin

CC = g++
CCFLAGS = -Wall -g
LD = g++
LDFLAGS = 
LIBS = -framework OpenGL -framework GLUT


OBJS = $(OBJ)/main.o


EXE = SchroedingersCat


all : dirs $(BIN)/$(EXE)


.PHONY: dirs
dirs:
	@mkdir -p $(OBJ)
	@mkdir -p $(BIN)


$(BIN)/$(EXE): $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $(LIBS) $^


$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $(CCFLAGS) $^


clean:
	rm -rf $(BUILD) $(BIN)

