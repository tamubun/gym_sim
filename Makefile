ODE_DIR=../ode-0.5

USER_SETTINGS=$(ODE_DIR)/config/user-settings
include $(USER_SETTINGS)
PLATFORM_MAKEFILE=$(ODE_DIR)/config/makefile.$(PLATFORM)
include $(PLATFORM_MAKEFILE)

LIBPATH=$(ODE_DIR)/lib
INCLUDE=-I$(ODE_DIR)/include
ODE_LIB_NAME=ode
ODE_LIB=$(LIBPATH)/libode.a

SRC_DIR=./src
DS_DIR=$(SRC_DIR)/drawstuff
FLAGS=-O3
DRAWSTUFF_SRC = $(DS_DIR)/drawstuff.cpp $(DS_DIR)/x11.cpp
DRAWSTUFF_OBJECTS=$(DRAWSTUFF_SRC:%.cpp=%.o)
COMMON_OBJECTS= \
  $(addprefix $(SRC_DIR)/,\
    addition.o object.o gymnast.o movement.o files.o record.o)

all: hineri.exe sharin.exe

hineri.exe: $(SRC_DIR)/hineri.o $(DRAWSTUFF_OBJECTS)
	$(CC) -o $@ $^ $(ODE_LIB) $(LINK_OPENGL) $(LINK_MATH)

sharin.exe: $(SRC_DIR)/sharin.o $(SRC_DIR)/mode.o \
		$(COMMON_OBJECTS) $(DRAWSTUFF_OBJECTS)
	$(CC) -o $@ $^ $(ODE_LIB) $(LINK_OPENGL) $(LINK_MATH)

$(DS_DIR)/%.o: $(DS_DIR)/%.cpp
	$(CC) $(C_FLAGS) $(INCLUDE) $(INC_OPCODE) $(DEFINES) $(C_OPT)$(OPT) -o $@ $<

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(C_FLAGS) $(INCLUDE) $(INC_OPCODE) $(DEFINES) $(FLAGS) -o $@ $<

$(DS_DIR)/drawstuff.o: $(SRC_DIR)/addition.h

clean:
	rm -f *.exe *.bak *~ $(SRC_DIR)/*.o $(SRC_DIR)/*~\
	$(DS_DIR)/*.o $(DS_DIR)/*~

depend:
	makedepend -Y $(DEFINES) $(SRC_DIR)/*.cpp


# DO NOT DELETE

./src/addition.o: ./src/addition.h
./src/files.o: ./src/record.h
./src/gymnast.o: ./src/object.h ./src/gymnast.h ./src/movement.h
./src/mode.o: ./src/addition.h ./src/gymnast.h ./src/mode.h
./src/movement.o: ./src/movement.h ./src/gymnast.h
./src/object.o: ./src/object.h ./src/addition.h
./src/record.o: ./src/files.h ./src/record.h
./src/sharin.o: ./src/addition.h ./src/object.h ./src/gymnast.h
./src/sharin.o: ./src/record.h ./src/mode.h
