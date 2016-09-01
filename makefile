LIB_DIR := ./gen-cpp/
GEN_SRC := $(LIB_DIR)LinuxFileClient_constants.cpp $(LIB_DIR)LinuxFileClient_types.cpp
GEN_OBJ := $(patsubst %.cpp, %.o, $(GEN_SRC))

THRIFT_DIR := /usr/local/include/
BOOST_DIR := /usr/local/include/

INC := -I$(THRIFT_DIR)

.PHONY: all clean

all: XSMBService_server FileShareService_server

%.o: %.cpp
	g++ -Wall $(INC) -c $< -o $@

FileShareService_server: FileShareService_server.o $(GEN_OBJ) $(LIB_DIR)FileShareService.o
	g++ $^ -o $@ -L/usr/local/lib -lthrift

XSMBService_server: XSMBService_server.o $(GEN_OBJ) $(LIB_DIR)XSMBService.o
	g++ $^ -o $@ -L/usr/local/lib -lthrift -lboost_filesystem -lboost_system

clean:
	rm $(LIB_DIR)*.o *.o FileShareService_server XSMBService_server
