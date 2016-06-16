#include"cgi_math.h"
int main()
{
	string query_string;
	obtain_query_string(query_string);
	vector<string> v;
	analy_query((char*)query_string.c_str(),v);
	BigData num1(v[1].c_str());
	BigData num2(v[0].c_str());
	//BigData num1(1234);
	//BigData num2(12);
	BigData num4=num1/num2;
	//cout<<num4<<endl;
	BigData num5=num4*num2;
	//cout<<num5<<endl;
	BigData num6=num1-num5;
	//cout<<num3<<num5;
		
	cout<<num1<<"%"<<num2<<"="<<num6<<endl;
}
