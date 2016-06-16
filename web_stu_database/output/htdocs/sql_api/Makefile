ROOT_PATH=$(shell pwd)
MYSQL_INCLUDE=$(ROOT_PATH)/lib/include
MYSQL_LIB=$(ROOT_PATH)/lib/lib

BIN_INSERT=sql_cgi_insert
BIN_SELECT=sql_cgi_select
BIN_SELECT_NAME=sql_cgi_select_name
BIN_UPDATA=sql_cgi_updata
BIN_DELETE=sql_cgi_delete

INCLUDE=-I$(MYSQL_INCLUDE)
LIB=-L$(MYSQL_LIB)
LIB_LINK=-lmysqlclient

.PHONY:output
output:$(BIN_INSERT) $(BIN_SELECT) $(BIN_SELECT_NAME) $(BIN_UPDATA) $(BIN_DELETE)
$(BIN_INSERT):sql_api.cpp insert.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK) 
$(BIN_SELECT):sql_api.cpp select.cpp
	 g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK) 
$(BIN_SELECT_NAME):sql_api.cpp select_name.cpp
	 g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK) 
$(BIN_UPDATA):sql_api.cpp updata.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK) 
$(BIN_DELETE):sql_api.cpp delete.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK) 


.PHONY:clean
clean:
	rm -f $(BIN_INSERT) $(BIN_SELECT) $(BIN_SELECT_NAME) $(BIN_UPDATA) $(BIN_DELETE)


