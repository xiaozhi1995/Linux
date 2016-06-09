ROOT_PATH=$(shell pwd)
MYSQL_INCLUDE=$(ROOT_PATH)/lib/include
MYSQL_LIB=$(ROOT_PATH)/lib/lib

BIN_INSERT=sql_cgi_insert
BIN_SELECT=sql_cgi_select

INCLUDE=-I$(MYSQL_INCLUDE)
LIB=-L$(MYSQL_LIB)
LIB_LINK=-lmysqlclient

.PHONY:output
all:$(BIN_INSERT) $(BIN_SELECT)
$(BIN_INSERT):sql_api.cpp insert.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK) 
$(BIN_SELECT):sql_api.cpp select.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK) 

.PHONY:clean
clean:
	rm -f $(BIN_INSERT) $(BIN_SELECT)



