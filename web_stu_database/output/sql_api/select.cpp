#include"sql_api.h"
int main()
{
	sql_api _st;
	_st.connect_mysql();
	_st.select_mysql();
	return 0;
}
