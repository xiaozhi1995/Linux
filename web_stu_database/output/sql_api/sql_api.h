#include<iostream>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<vector>
#include<mysql.h>
using namespace std;
class sql_api
{
public:
	sql_api();
	~sql_api();
	bool connect_mysql();
	bool close_mysql();
	bool insert_mysql(string name,string school,string hobby);
	bool delete_mysql();
	bool select_mysql();
	bool updata_mysql();
private:
	bool _select_mysql();
	bool _op_sql(string _sql);
private:
	MYSQL* _conn_fd;
	string _host;
	string _user;
	string _passwd;
	string _db;
	short _port;
	
	MYSQL_RES* _res;	
};
void anly_query(string& query,vector<string>& ret);
