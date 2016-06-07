ROOT_PATH=$(shell pwd)
LDFLAGS=-lpthread
FLAGS=#_DEBUG1_
CC=gcc
BIN=httpd

CGI_BIN=htdocs/cgi_bin
SRC=$(shell ls *.c)
OBJ=$(SRC:.c=.o)

$(BIN):$(OBJ)
	@echo "Linking [$^] to [$@]"
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Linking done..."
%.o:%.c
	@echo "Compling [$<] to [$@]"
	@$(CC) -c $<
	@echo "Compling done..."
.PHONY:clean
clean:
	@rm -rf *.o $(BIN) output
	@for name in `echo $(CGI_BIN)`;\
	do\
		cd $$name;\
		make clean;\
		cd -;\
	done


.PHONY:cgi
cgi:
	@for name in `echo $(CGI_BIN)`;\
	do\
		cd $$name;\
		make;\
		cd -;\
	done

.PHONY:output
output:$(BIN) cgi
	@mkdir -p output/log
	@cp -rf htdocs output
	@mkdir -p output/htdocs/cgi_bin
	@cp -f httpd output/
	@cp -f start.sh output/
	@cp -rf conf output
	@for name in `echo $(CGI_BIN)`;\
	do\
		cd $$name;\
		make output;\
		cd -;\
	done
