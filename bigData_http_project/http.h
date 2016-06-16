#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/sendfile.h>
#include<sys/stat.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<pthread.h>
#include<errno.h>
#define _DEF_PAGE_ "index.html"
#define _SIZE_ 1024
typedef struct bf
{
	char _buf[_SIZE_];
	int _fd;
	int _cgi;
	char _method[_SIZE_/2];
	char _path[_SIZE_];
	union
	{
		char* _query_string;
		int _st_size;
	};
	int _err;

}bf_t,*bf_p;
void printLog(const char* const str,const char* const fun,int line);
void usage(const char*  const  proc);
int startup(char* ip,int port);
void response_err(bf_p bf,int eno);
void echo_error(bf_p bf,int eno);
int get_line(int sock,char* buf,int size);
void clear_head(int sock);
void execute_cgi(bf_p bf);
void echo_html(bf_p bf);
void accept_request(void* ptr);
