#include"sql_api.h"
int main()
{
	char method[1024];
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	string query_string;
	int content_length=0;
	if(getenv("REQUEST_METHOD"))
	{
		strcpy(method,getenv("REQUEST_METHOD"));
	}
	else
	{
		cout<<strerror(errno)<<endl;
		return -1;
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
	vector<string> ret;
	anly_query(query_string,ret);
	sql_api _tb;;
	_tb.connect_mysql();
	_tb.select_name_school(ret[1],ret[0]);
	return 0;
}
