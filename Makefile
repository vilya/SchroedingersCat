SRC := src
RESOURCE := resource
BUILD := build
OBJ := $(BUILD)/obj
BIN := bin

CC = g++
LD = g++

ifeq ($(OSTYPE),linux-gnu)
CCFLAGS = -Wall -g
LDFLAGS = 
LIBS = -lGL -lglut -ljpeg -lpng -ltiff
else
CCFLAGS = -Wall -g -I/opt/local/include
LDFLAGS = -L/opt/local/lib
LIBS = -framework OpenGL -framework GLUT -ljpeg -lpng -ltiff
endif


OBJS = \
	$(OBJ)/drawing.o \
	$(OBJ)/gamedata.o \
	$(OBJ)/image.o \
	$(OBJ)/main.o \
	$(OBJ)/vec2.o


NAME = SchroedingersCat
EXE = $(NAME)
ZIP = $(NAME).zip


all : dirs game


.PHONY: dirs
dirs:
	@mkdir -p $(OBJ)
	@mkdir -p $(BIN)


.PHONY: game
game: $(BIN)/$(EXE)
	cp -R $(RESOURCE) $(BIN)
	cd $(BIN) && zip -r $(ZIP) $(EXE) $(RESOURCE)

$(BIN)/$(EXE): $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $(LIBS) $^


$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $(CCFLAGS) $(INCLUDES) $^


clean:
	rm -rf $(BUILD) $(BIN)

