#include<iostream>
#include<string>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
using namespace std;
void echo_html(int val[],int size,int sum)
{
	cout<<"<html>"<<endl;
	cout<<"<head>"<<endl;
	cout<<"<h1>Add Caculate</h1>"<<endl;
	cout<<"</head>"<<endl;
	cout<<"<body>"<<endl;
	cout<<"<p>"<<endl;
	for(int i=0;i<size;++i)
	{
		cout<<val[i];
		if(i!=size-1)
		{
			cout<<'+';
		}
	}
	cout<<'='<<sum<<"</p>"<<endl;
	cout<<"</body>"<<endl;
	cout<<"</html>"<<endl;

}
void cal_add()
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
	string data=query_string;
	int i=data.size();
	int nums[2];
	int index=0;
	int sum=0;
	for(;i>0;--i)//obtain
	{
		if(data[i-1]=='=')
		{
			nums[index]=atoi(&data[i]);//i-1+1
			sum+=nums[index];
			++index;
		}
		else if(data[i-1]=='&')
		{
			data[i-1]='\0';
		}
	}
//	for(i=0;i<index;++i)
//	{
//		cout<<nums[i]<<"  ";
//	}
//	cout<<endl;
	echo_html(nums,index,sum);
}
int main()
{
	cal_add();
	return 0;
}
