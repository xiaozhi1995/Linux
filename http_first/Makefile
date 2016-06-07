ROOT_PATH=$(shell pwd)
LDFLAGS=-lpthread
FLAGS=#_DEBUG1_
CC=gcc
BIN=httpd
SRC=$(shell ls *.c)
OBJ=$(SRC:.c=.o)

$(BIN):$(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
%.o:%.c
	$(CC) -c $<
.PHONY:clean
clean:
	rm -f $(OBJ) $(BIN)

.PHONY:debug
debug:
	@echo $(SRC)
	@echo $(OBJ)
