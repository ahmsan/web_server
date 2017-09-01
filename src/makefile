#$(CXX) -static  -o $@ $(OBJS) $(LIBPATH) $(LIB)
CXX			= g++
CFLAGS		= -g -Wall -O2

BASE_DIR	= ./
INC_DIR		= $(BASE_DIR)
SRC_DIR		= $(BASE_DIR)
OBJ_DIR		= $(BASE_DIR)/obj
RELEASE_DIR	= $(BASE_DIR)/bin

###############################################################

INC			= $(INC_DIR)
SRC  		= $(wildcard $(SRC_DIR)/*.cpp)
OBJS 		= $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

LIB = -Wl,--start-group -lpthread -lm \
	    -levent -lrt -ldl \
		-Wl,--end-group

.cpp.o:
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CFLAGS) ${INC} -c $< -o $(OBJ_DIR)/$@

TARGET = web_server

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CXX) -o $(RELEASE_DIR)/$@ $(OBJ_DIR)/.o $(LIBPATH) $(LIB)


.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)/*.o $(RELEASE_DIR)/$(TARGET)
	rm -rf $(OBJ_DIR) 