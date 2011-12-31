SRC := src
RESOURCE := resource
BUILD := build
OBJ := $(BUILD)/obj
BIN := bin
TOOLS := tools

CC = g++
LD = g++

ifeq ($(OSTYPE),linux-gnu)
CCFLAGS = -Wall -g
LDFLAGS = 
LIBS = -lGL -lGLU -lglut -ljpeg -lpng -ltiff
GAME = game-linux
else
CCFLAGS = -Wall -g -isysroot /Developer/SDKs/MacOSX10.6.sdk -I/opt/local/include
LDFLAGS = -headerpad_max_install_names -macosx_version_min=10.6 -Wl,-syslibroot,/Developer/SDKs/MacOSX10.6.sdk -L/opt/local/lib
LIBS = -framework OpenGL -framework GLUT -ljpeg -lpng -ltiff
GAME = game-osx
endif


OBJS = \
	$(OBJ)/drawing.o \
	$(OBJ)/gamedata.o \
	$(OBJ)/image.o \
	$(OBJ)/level.o \
	$(OBJ)/main.o \
	$(OBJ)/resource.o \
	$(OBJ)/vec2.o


NAME = SchroedingersCat
EXE = $(NAME)
ZIP = $(NAME).zip
APP = $(NAME).app


all : dirs $(GAME)


.PHONY: dirs
dirs:
	@mkdir -p $(OBJ)
	@mkdir -p $(BIN)


.PHONY: game-linux
game-linux: $(BIN)/$(EXE)
	cp -R $(RESOURCE) $(BIN)
	cd $(BIN) && zip -r $(ZIP) $(EXE) $(RESOURCE)


.PHONY: game-osx
game-osx: $(BIN)/$(EXE)
	$(TOOLS)/makeappbundle.sh $(BIN)/$(APP) $(BIN)/$(EXE) $(RESOURCE)


$(BIN)/$(EXE): $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $(LIBS) $^


$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $(CCFLAGS) $(INCLUDES) $^


clean:
	rm -rf $(BUILD) $(BIN)

