SRC := src
BUILD := build
OBJ := $(BUILD)/obj
BIN := bin

CC = g++
CCFLAGS = -Wall -g -I/opt/local/include
LD = g++
LDFLAGS = -L/opt/local/lib
LIBS = -framework OpenGL -framework GLUT -ljpeg


OBJS = \
	$(OBJ)/drawing.o \
	$(OBJ)/gamedata.o \
	$(OBJ)/image.o \
	$(OBJ)/main.o \
	$(OBJ)/shader.o \
	$(OBJ)/vec2.o


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

