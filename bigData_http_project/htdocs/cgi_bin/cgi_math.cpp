#include"cgi_math.h"
void analy_query(char * query_string,vector<string>& v)
{
	int i=strlen(query_string);
	for(;i>0;--i)//obtain
	{
		if(query_string[i-1]=='=')
		{
			v.push_back(&query_string[i]);//i-1+1
		}
		else if(query_string[i-1]=='&')
		{
			query_string[i-1]='\0';
		}
	}
}
void obtain_query_string(string& query_string)
{
	char method[1024];
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	int content_length=0;
	if(getenv("REQUEST_METHOD"))
	{
		strcpy(method,getenv("REQUEST_METHOD"));
	}
	else
	{
		cout<<strerror(errno)<<endl;
		return;
	}
	if(strcasecmp(method,"GET")==0)
	{
		query_string+=getenv("QUERY_STRING");
	}
	else//POST
	{
		ssize_t _s=-1;
		if((_s=read(0,buf,sizeof(buf)))>0)
		{
			buf[_s]='\0';
			query_string+=buf;
		}
	}
	//cout<<query_string;
	//num1=100&num2=200
}
