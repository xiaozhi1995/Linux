#include"http.h"
void printLog(const char* const str,const char* const fun,int line)
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
void response_err(bf_p bf,int eno)
{
	char state_line[_SIZE_];
	char err_des[30];//error description
	memset(state_line,'\0',sizeof(state_line));
	memset(err_des,'\0',sizeof(err_des));
	switch(eno)
	{
		case 301:
			strcpy(err_des,"Removed");
			break;
		case 404:
			strcpy(err_des,"Not Found");
			break;
		case 403:
			strcpy(err_des,"Forbid");
			break;
		case 500:
			strcpy(err_des,"Inner Error");
			break;
		case 501:
			strcpy(err_des,"Not execute");
			break;
		default:
			break;
	}
	sprintf(state_line,"HTTP/1.0 %d %s\r\n\r\n",eno,err_des);
	//printf("state_line:%s\n",state_line);
	strcpy(bf->_buf,state_line);
}
void echo_error(bf_p bf,int eno)
{
	switch(eno)
	{
		case 301:
			response_err(bf,eno);
			break;
		case 404:
			response_err(bf,eno);
			break;
		case 403:
			response_err(bf,eno);
			break;
		case 500:
			response_err(bf,eno);
			break;
		case 501:
			response_err(bf,eno);
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

void execute_cgi(bf_p bf)
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
	sprintf(method_env,"REQUEST_METHOD=%s",bf->_method);
	putenv(method_env);
	//printf("method:%s,path:%s,query_string:%s\n",method,path,query_string);
	if(strcasecmp(bf->_method,"GET")==0)
	{
		sprintf(query_env,"QUERY_STRING=%s",bf->_query_string);
	    putenv(query_env);
	}
	else
	{
		while((_s=get_line(bf->_fd,buf,sizeof(buf)))>1)
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
	printf("exxxxxxxxxxxxxxx");
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
		execl(bf->_path,bf->_path,NULL);
			
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
		if(strcasecmp(bf->_method,"POST")==0)
		{
			while(index<content_len&&read(bf->_fd,&ch,1)>0)
			{	
				vals[index++]=ch;
			}
		}
	//	printf("%s\n",vals);
		write(in_fds[1],vals,strlen(vals));
	//	printf("ffffffffffff");
		memset(bf->_buf,'\0',sizeof(bf->_buf));
		char* status_line="HTTP/1.0 200 ok\r\n\r\n";
		sprintf(bf->_buf,status_line,strlen(status_line));
		printf("response\n");
	    int i=strlen(bf->_buf);
		while(read(out_fds[0],&ch,1)>0)
		{
			bf->_buf[i++]=ch;
		}
		bf->_buf[i]='\0';
		close(in_fds[1]);
		close(out_fds[0]);
		waitpid(id);
	}
}
void echo_html(bf_p bf)
{
	char* status_line="HTTP/1.0  200 ok\r\n\r\n";
	sprintf(bf->_buf,status_line,strlen(status_line));
	//printf("%s!!!!!!!!!!!!!!!!\n",bf->_buf);
}

void accept_request(void* ptr)
{
	bf_p bf=(bf_p)ptr;
	bf->_err=0;
	int ret=-1;
	int i=0,j=0;
	char url[_SIZE_];
	memset(bf->_method,'\0',sizeof(bf->_method));
	memset(bf->_buf,'\0',sizeof(bf->_buf));
	memset(bf->_path,'\0',sizeof(bf->_path));
	memset(url,'\0',sizeof(url));

	if(get_line(bf->_fd,bf->_buf,sizeof(bf->_buf))==0)
	{
		printLog("errno",__FUNCTION__,__LINE__);
		return;
	}
	i=j=0;
	while('\0'!=bf->_buf[i]&&!isspace(bf->_buf[i])&&i<strlen(bf->_buf)&&j<sizeof(bf->_method)-1)
	{
		bf->_method[j]=bf->_buf[i];//get method
		++i;
		++j;
	}
	bf->_method[j]='\0';
	//printf("method:%s\n",bf->_method);	
	j=0;
	while('\0'!=bf->_buf[i]&&isspace(bf->_buf[i]))
	{
		++i;
	}
	while('\0'!=bf->_buf[i]&&!isspace(bf->_buf[i])&&i<strlen(bf->_buf)&&j<sizeof(url)-1)
	{
		url[j]=bf->_buf[i];
		++j;
		++i;
	}
	url[j]='\0';
	//printf("url:%s\n",url);
	bf->_cgi=0;
	if(strcasecmp(bf->_method,"POST")!=0&&strcasecmp(bf->_method,"GET")!=0)
	{
		echo_error(bf,500);
		bf->_err=1;
		//return;
	}
	bf->_query_string=NULL;
	if(strcasecmp(bf->_method,"POST")==0)
	{
		bf->_cgi=1;
		bf->_query_string=NULL;
	}
	if(strcasecmp(bf->_method,"GET")==0)
	{
		bf->_query_string=url;
		while(*bf->_query_string!='\0'&&*bf->_query_string!='?')
		{
			++bf->_query_string;
		}
		    
		//printf("query_string::::::::::::::::\n");		
		if(*bf->_query_string=='?')
		{
		   *bf->_query_string='\0';
		   bf->_cgi=1;
		   ++bf->_query_string;
		}
	}
	
	sprintf(bf->_path,"htdocs%s",url);
	struct stat st;
	if(stat(bf->_path,&st)<0)//not exist
	{
		echo_error(bf,404);
		bf->_err=1;
		//return;
	
	}
	else if(S_IFDIR&st.st_mode)//dir
	{
		if(strcmp(bf->_path,"htdocs/")!=0)
			strcpy(bf->_path,"htdocs/");
		strcat(bf->_path,_DEF_PAGE_);
	}
	else if((st.st_mode&S_IXUSR)||(st.st_mode&S_IXGRP)||(st.st_mode&S_IXOTH))
	{
		bf->_cgi=1;
	}
	printf("%d:%s\n",bf->_cgi,bf->_path);
	if(bf->_err==0)
	{
		if(bf->_cgi)
		{
			execute_cgi(bf);
		}
		else
		{
			clear_head(bf->_fd);
			bf->_st_size=st.st_size;
			echo_html(bf);
		}
	}
}
