#include"sql_api.h"
sql_api::sql_api()
:_conn_fd(NULL),
_host("127.0.0.1"),
_user("root"),
_passwd(""),
_db("9_class"),
_port(3306),
_res(NULL)
{}
sql_api::~sql_api()
{
	if(_res)
	{
		free(_res);
	}
	close_mysql();
}
bool  sql_api::connect_mysql()
{
	_conn_fd=mysql_init(NULL);
	mysql_real_connect(_conn_fd,_host.c_str(),_user.c_str(),_passwd.c_str(),_db.c_str(),_port,NULL,0);
	return true;
}
bool sql_api::close_mysql()
{
	mysql_close(_conn_fd);
	return true;
}
bool sql_api::insert_mysql(string name,string school,string hobby)
{
	string _sql="INSERT INTO stu ";
	_sql+="(name,school,hobby) ";
	_sql+="VALUES ('";
	_sql+=name;
	_sql+="','";
	_sql+=school;
	_sql+="','";
	_sql+=hobby;
	_sql+="')";
//	cout<<_sql<<endl;
//	cout<<"insert\n"<<endl;
	return _op_sql(_sql);
}
bool sql_api::delete_mysql(string id,string name)
{
	bool ret=false;
	string _sql="delete from stu where id=";
	_sql+=id;
	_sql+=" and name='";
	_sql+=name;
	_sql+="'";
	//cout<<_sql<<endl;
	return _op_sql(_sql);
}
bool sql_api::select_mysql()
{
	string _sql="select* from stu";//must
	_op_sql(_sql);
	return _select_mysql();
}
bool sql_api::select_name_school(string name,string school)
{
	bool ret=false;
	string _sql="select* from stu where name='";
	_sql+=name;
	_sql+="' and school='";
	_sql+=school;
	_sql+="'";
	//cout<<_sql<<endl;
	_op_sql(_sql);
	return _select_mysql();
}

bool sql_api::updata_mysql(string id,string name,string school,string hobby)
{
	bool ret=false;
	//updata stu set something='1';
	string _sql="update stu set name='";
	_sql+=name;
	_sql+="',school='";
	_sql+=school;
	_sql+="',hobby='";
	_sql+=hobby;
	_sql+="' where id=";
	_sql+=id;
	//cout<<id<<_sql<<endl;
	return _op_sql(_sql);
}
bool sql_api::_select_mysql()
{
	int _row=0;
	int _field=0;
	//_res=(MYSQL_RES*)malloc(sizeof(MYSQL_RES));//not require
	//memset(_res,'\0',2048);//not require
	_res=mysql_store_result(_conn_fd);
	if(_res)
	{
	//	cout<<"KKKKKKKKKK"<<endl;
		_row=mysql_num_rows(_res);
		_field=mysql_num_fields(_res);
		cout<<"row:"<<_row<<"field:"<<_field<<endl;
		MYSQL_FIELD* _fd;
		for(;_fd=mysql_fetch_field(_res);)
		{
			cout<<_fd->name<<'\t';
		}
		cout<<endl;
	}
	//cout<<"select no;\n";
	//cout<<"row:"<<_row<<"field:"<<_field<<endl;
	MYSQL_ROW row_line;
	while(_row)
	{
		row_line=mysql_fetch_row(_res);
		int i=0;
		for(;i<_field;++i)
		{
			cout<<row_line[i]<<'\t';
		}
		cout<<endl;
		--_row;
	}
}
bool sql_api::_op_sql(string _sql)
{
	bool ret=false;
	if(0==mysql_query(_conn_fd,_sql.c_str()))
	{
		ret=true;
		cout<<_sql<<" success"<<endl;
	}
	return ret;
}
void anly_query(string& query,vector<string>& ret)
{
	int index=query.size()-1;
	string str=query;
	while(index>=0)
	{
		if(str[index]=='=')
		{
			ret.push_back(&str[index+1]);
		}
		if(str[index]=='&')
		{
			str[index]='\0';
		}
		--index;
	}
}
