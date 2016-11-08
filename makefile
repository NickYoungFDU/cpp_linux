LIB_DIR := ./gen-cpp/
GEN_SRC := $(LIB_DIR)cpp_linux_constants.cpp $(LIB_DIR)cpp_linux_types.cpp $(LIB_DIR)FileShareService.cpp $(LIB_DIR)XSMBService.cpp infrastructure.cpp UnitTest.cpp
GEN_OBJ := $(patsubst %.cpp, %.o, $(GEN_SRC))

THRIFT_DIR := /usr/local/include/
BOOST_DIR := /usr/local/include/

INC := -I$(THRIFT_DIR) -I.

.PHONY: all clean

all: server

%.o: %.cpp
	g++ -Wall $(INC) -c $< -o $@ -std=c++11 -DBOOST_LOG_DYN_LINK 

server: server.o FileShareServiceHandler.o XSMBServiceHandler.o $(GEN_OBJ) 
	g++ $^ -o $@ -std=c++11 -L/usr/local/lib -lthrift -lboost_filesystem -lboost_system -lthriftnb -levent -lboost_log -lboost_log_setup -lboost_date_time -lboost_thread -lpthread

clean:
	rm $(LIB_DIR)*.o *.o server
