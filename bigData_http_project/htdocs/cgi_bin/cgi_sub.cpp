#include"cgi_math.h"
int main()
{
	string query_string;
	obtain_query_string(query_string);
	vector<string> v;
	analy_query((char*)query_string.c_str(),v);
	BigData b1(v[1].c_str());
	BigData b2(v[0].c_str());
	BigData ret=b1-b2;
	cout<<b1<<"-"<<b2<<"="<<ret<<endl;
}
