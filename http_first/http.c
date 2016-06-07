#include"http.h"
static void printLog(const char* const str,const char* const fun,int line)
{
	printf("%s:%s:%d\n",str,fun,line);
}
void usage(const char*  const  proc)
{
	assert(proc);
	printLog(proc,__FUNCTION__,__LINE__);
}
int startup(char* ip,int port)
{
	assert(ip);
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		printLog(strerror(errno),__FUNCTION__,__LINE__);
		exit(1);
	}
	int opt=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(port);
	if(strncmp(ip,"any",3)==0)
	{
		local.sin_addr.s_addr= INADDR_ANY;
	}
	else
	{
		local.sin_addr.s_addr=inet_addr(ip);
	}
	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		printLog(strerror(errno),__FUNCTION__,__LINE__);
		exit(1);
	}
	if(listen(sock,5)<0)
	{
		printLog(strerror(errno),__FUNCTION__,__LINE__);
		exit(1);
	}
	return sock;
}
void response_err(int sock,int eno)
{
	char state_line[_SIZE_];
	char err_des[30];//error description
	memset(state_line,'\0',sizeof(state_line));
	memset(err_des,'\0',sizeof(err_des));
	switch(eno)
	{
		case 404:
			strcpy(err_des,"Not Found");
			break;
		default:
			break;
	}
	sprintf(state_line,"HTTP/1.0 %d %s\r\n\r\n",eno,err_des);
	send(sock,state_line,strlen(state_line),0);
}
void echo_error(int sock,int eno)
{
	switch(eno)
	{
		case 404:
			response_err(sock,eno);
			break;
		case 403:
			break;
		default:
			break;
	}
	return;
}
int get_line(int sock,char* buf,int size)
{
	assert(buf);
	int i=0;
	ssize_t _s=-1;
	char ch='\0';
//	printf("getLine");
	while(i<size-1&&ch!='\n')
	{
		_s=recv(sock,&ch,1,0);
		if(_s)
		{
			if(ch=='\r')
			{
				if((_s=recv(sock,&ch,1,MSG_PEEK)))
				{
					if(_s>0&&ch=='\n')
						recv(sock,&ch,1,0);
				}
			}
			buf[i++]=ch;
		}
		else
		{
			buf[i++]='\n';
			break;
		}
	}
//	printf("endddddddddddd");
	buf[i]='\0';
	return i;
}
void clear_head(int sock)
{
	char buf[_SIZE_];
	buf[0]='\0';
	ssize_t _s=1;
	while(_s>0&&strcmp(buf,"\n")!=0)
	{
		_s=get_line(sock,buf,sizeof(buf));
	}
}

void execute_cgi(int sock,const char* path,const char* method,char* query_string)
{
	int content_len=0;//post method content-length
	ssize_t _s=-1;
	char buf[_SIZE_];
	char method_env[30];
	char query_env[_SIZE_];
	char content_len_env[30];
	memset(buf,'\0',sizeof(buf));
	memset(method_env,'\0',sizeof(method_env));
	memset(query_env,'\0',sizeof(query_env));
	memset(content_len_env,'\0',sizeof(content_len_env));
	//if method=get,query_string not null,if method=post,query_string is null
	sprintf(method_env,"REQUEST_METHOD=%s",method);
	putenv(method_env);
	//printf("method:%s,path:%s,query_string:%s\n",method,path,query_string);
	if(strcasecmp(method,"GET")==0)
	{
		//GET
		//printf("ggggggggetttttttttt\n");
		sprintf(query_env,"QUERY_STRING=%s",query_string);
	    putenv(query_env);
	}
	else
	{
		while((_s=get_line(sock,buf,sizeof(buf)))>1)
		{
			if(strncasecmp(buf,"Content-Length:",15)==0)
			{
				//printf("length::::::::::%s\n",buf);
				content_len=atoi(buf+15);
			//	break;//not break!!!!!!!!!!!!!!!
			}
		}
		//printf("Content-Length:%d\n",content_len);
		sprintf(content_len_env,"CONTENT_LENGTH=%d",content_len);
		putenv(content_len_env);
	}
	//printf("ready env\n");
	//ready
	pid_t id;
	int in_fds[2];
	int out_fds[2];
	if(pipe(in_fds)<0)
	{
		printLog(strerror(errno),__FUNCTION__,__LINE__);
		exit(1);
	}
	if(pipe(out_fds)<0)	
	{
		printLog(strerror(errno),__FUNCTION__,__LINE__);
		exit(1);
	}
	if((id=fork())<0)
	{
		printLog(strerror(errno),__FUNCTION__,__LINE__);
		exit(1);
	}
	else if(id==0)//child
	{
		close(in_fds[1]);
		close(out_fds[0]);
		dup2(in_fds[0],0);
		dup2(out_fds[1],1);
		execl(path,path,NULL);
			
		close(in_fds[0]);
		close(out_fds[1]);
	}
	else//father
	{
		close(in_fds[0]);
		close(out_fds[1]);
		char vals[1024];
		memset(vals,'\0',sizeof(vals));
		ssize_t _s=-1;
		int index=0;
		char ch='0';
		if(strcasecmp(method,"POST")==0)
		{
			while(index<content_len&&read(sock,&ch,1)>0)
			{	
				vals[index++]=ch;
			}
		}
	//	printf("%s\n",vals);
		write(in_fds[1],vals,strlen(vals));
	//	printf("ffffffffffff");
		char* status_line="HTTP/1.0 200 ok\r\n\r\n";
	//	printf("response\n");
		send(sock,status_line,strlen(status_line),0);
		while(read(out_fds[0],&ch,1)>0)
		{
		//	printf("%c\n",ch);
			send(sock,&ch,1,0);
		}
		close(in_fds[1]);
		close(out_fds[0]);
		waitpid(id);
	}
}
void echo_html(int sock,char* path,ssize_t size)
{
//	printf("echo_htmlllllllllll\n");
//	printf("path:%s\n",path);
	int fd=open(path,O_RDONLY);
	if(fd<0)
	{
		printLog(strerror(errno),__FUNCTION__,__LINE__);
		exit(1);
	}
//	printf("fd:%d\n",fd);
	char* status_line="HTTP/1.0  200 ok\r\n\r\n";
	send(sock,status_line,strlen(status_line),0);
//	printf("sock  success,size:%d\n",size);
	if(sendfile(sock,fd,NULL,size)<0)
	{
		printf("error");
	}
	//printf("sendfile  success");
	close(fd);
}
void accept_request(int sock)
{
	char buf[_SIZE_];
//	int ret;	
//	while((ret=get_line(sock,buf,sizeof(buf)))>0)
//	{
//		if(ret==0)
//		{
//			printLog(strerror(errno),__FUNCTION__,__LINE__);
//			break;
//		}
//		printf("%s",buf);
//	}
	int ret=-1;
	int i=0,j=0;
	char method[_SIZE_/2];
	char url[_SIZE_];
	char path[_SIZE_];
	memset(method,'\0',sizeof(method));
	memset(buf,'\0',sizeof(buf));
	memset(path,'\0',sizeof(path));
	memset(url,'\0',sizeof(url));

	if(get_line(sock,buf,sizeof(buf))==0)
	{
		printLog("errno",__FUNCTION__,__LINE__);
		return;
	}
	//printf("%s",buf);
	i=j=0;
	while('\0'!=buf[i]&&!isspace(buf[i])&&i<strlen(buf)&&j<sizeof(method)-1)
	{
		method[j]=buf[i];//get method
		++i;
		++j;
	}
	method[j]='\0';
//	printf("method:%s\n",method);	
	j=0;
	while('\0'!=buf[i]&&isspace(buf[i]))
	{
		++i;
	}
	while('\0'!=buf[i]&&!isspace(buf[i])&&i<strlen(buf)&&j<sizeof(url)-1)
	{
		url[j]=buf[i];
		++j;
		++i;
	}
	url[j]='\0';
//	printf("url:%s\n",url);
	int cgi=0;
	if(strcasecmp(method,"POST")!=0&&strcasecmp(method,"GET")!=0)
	{
		printf("error:%d\n",__LINE__);
	//	echoErr(sock,1);
		return;
	}
	char* query_string=NULL;
//	printf("success\n");
	if(strcasecmp(method,"POST")==0)
	{
		cgi=1;
		query_string=NULL;
	}
	if(strcasecmp(method,"GET")==0)
	{
//		printf("getttttttttttttttttttt\n");
		query_string=url;
		while(*query_string!='\0'&&*query_string!='?')
		{
			++query_string;
		}
		    
			
		if(*query_string=='?')
		{
		   *query_string='\0';
		   cgi=1;
		   ++query_string;
		}
	}
	
	if(strcmp(url,"/")==0)
		sprintf(path,"htdocs%s",url);
	else
		strcpy(path,url+1);
	struct stat st;
	if(stat(path,&st)<0)//not exist
	{
		echo_error(sock,404);
	//	return;
	
		//strcpy(path,"htdocs/");
		//strcat(path,_DEF_PAGE_);
	}
	else if(S_IFDIR&st.st_mode)//dir
	{
//		printf("dirrrrrrrrrrr\n");
		if(strcmp(path,"htdocs/")!=0)
			strcpy(path,"htdocs/");
		strcat(path,_DEF_PAGE_);
	}
	else if((st.st_mode&S_IXUSR)||(st.st_mode&S_IXGRP)||(st.st_mode&S_IXOTH))
	{
//		printf("xxxxxxxxxxxxxxxxxx\n");
		cgi=1;
	}
	//printf("path:%smethod:%squery_string:%scgi:%d\n",path,method,query_string,cgi);
	if(cgi)
	{
		execute_cgi(sock,path,method,query_string);
	}
	else
	{
//		printf("echo_html\n");
		fflush(stdout);
		clear_head(sock);
		echo_html(sock,path,st.st_size);
	}
	close(sock);
}
void* handle_client(void* arg)
{
	int sock=(int)arg;
	accept_request(sock);
	return NULL;
}
