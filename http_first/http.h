#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/sendfile.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<pthread.h>
#include<errno.h>
#define _DEF_PAGE_ "index.html"
#define _SIZE_ 1024
static void printLog(const char* const str,const char* const fun,int line);
void usage(const char*  const  proc);
int startup(char* ip,int port);
void response_err(int sock,int eno)
void echo_error(int sock,int eno);
int get_line(int sock,char* buf,int size);
void clear_head(int sock);
void execute_cgi(int sock,const char* path,const char* method,char* query_string);
void echo_html(int sock,char* path,ssize_t size);
void accept_request(int sock);
void* handle_client(void* arg);
