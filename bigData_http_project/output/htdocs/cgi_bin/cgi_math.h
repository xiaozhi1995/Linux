#include<iostream>
#include<string>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<vector>
#include<assert.h>
using namespace std;
#define INT_64 long long
#define UN_INIT  0xcccccccccccccccc//不能分开
#define MIN_INT (unsigned)0xffffffff
#define MAX_INT (unsigned)0x7fffffff
class BigData
{
public:
	BigData()
	:_data(0)
	,_pdata("")
	{}
	BigData(INT_64 data)
	:_data(data)
	{
		int tmp=_data;
		char symbol='+';
		if(tmp<0)
		{
			symbol='-';
			tmp=0-tmp;
		}
		_pdata.append(1,symbol);
		while(tmp)
		{
			_pdata.append(1,tmp%10+'0');
			tmp/=10;
		}
		char* str=(char*)_pdata.c_str()+1;
		while(*str=='0')
		{
			++str;
		}
		char* left=str;
		char* right=(char*)(_pdata.c_str()+_pdata.size()-1);
		char temp;
		while(left<right)
		{
			temp=*left;
			*left=*right;
			*right=temp;
			++left;
			--right;
		}
	}
	BigData(const char* str)
	{
		assert(str);
		//cout<<str<<endl;
		char* src=const_cast<char*>(str);
		INT_64 ret=0;
		char symbol='+';
		if(*src=='+'||*src=='-')
			symbol=*src++;
		_pdata.append(1,symbol);
		while('0'==*src)
			++src;
		while(*src>='0'&&*src<='9')
		{
			if (ret <= MAX_INT)
				ret = ret * 10 + *src-'0';
			_pdata.append(1,*src);
			++src;
		}
	//	cout<<_pdata<<endl;
		if(symbol=='-')
			ret=0-ret;
		_data=ret;
	}
	BigData operator+(BigData db)
	{
		if(!IsINT64OverFlow()&&!db.IsINT64OverFlow())
		{
			if(_pdata[0]!=db._pdata[0])
				return BigData(_data+db._data);
			else
			{
				if(_pdata[0]=='+'&&MAX_INT-_data>=db._data||_pdata[0]=='-'&&_data>=MIN_INT-db._data)
					return BigData(_data+db._data);
			}
		}
		BigData tmp(*this);
		return BigData(Add(tmp._pdata,db._pdata).c_str());
	}
	BigData operator-(BigData db)
	{
		if(!IsINT64OverFlow()&&!db.IsINT64OverFlow())
		{
			if(_pdata[0]==db._pdata[0])
				return BigData(_data-db._data);
			else
			{
				if(_pdata[0]=='+'&&MAX_INT+db._data>=_data||_pdata[0]=='-'&&_data>=MIN_INT+db._data)
					return BigData(_data-db._data);
			}
		}
		string ret;
		BigData tmp(*this);
		//cout<<tmp;
		if(_pdata[0]==db._pdata[0])
			ret=Sub(tmp._pdata,db._pdata);
		else
		{
			ret=Add(tmp._pdata,db._pdata);
			ret[0]=_pdata[0];
		}
		return BigData(ret.c_str());
	}
	BigData operator*(BigData db)
	{
		if(!IsINT64OverFlow()&&!db.IsINT64OverFlow())
		{
			if(_pdata[0]==db._pdata[0])
			{
				if(_pdata[0]=='+'&&_data<=MAX_INT/db._data||_pdata[0]=='-'&&_data>=MAX_INT/db._data)
					return BigData(_data*db._data);
			}
			else
			{
				if(_pdata[0]=='+'&&MIN_INT/_data<=db._data||_pdata[0]=='-'&&_data>=MIN_INT/db._data)
						return BigData(_data*db._data);
			}
		}
		BigData tmp(*this);
		return BigData(Mul(tmp._pdata,db._pdata).c_str());
	}
	BigData operator/(BigData db)
	{
		if(_data==0||db._data==0)
			return BigData((INT_64)0);
		if(!IsINT64OverFlow()&&!db.IsINT64OverFlow())
		{
			return BigData(_data/db._data);
		}
		BigData tmp(*this);
		return BigData(Div(tmp._pdata,db._pdata).c_str());
	}
	friend ostream& operator<<(ostream& out,BigData& db)
	{
		if(!db.IsINT64OverFlow())
		{
				out<<db._data;
				return out;
		}
		if(db._pdata[0]=='-')
			out<<&db._pdata[0];
		char* str=(char*)db._pdata.c_str()+1;
		while(*str=='0')
			++str;
		out<<str;
		return out;
	}
protected:
	bool IsINT64OverFlow()
	{
		if(_data<=MAX_INT&&_data>=MIN_INT)
			return false;
		return true;
	}
	string Add(string& s1,string& s2)
	{
		int leftSize=s1.size();
		int rightSize=s2.size();
		char symbol='+';
		string ret;
		if(s1[0]==s2[0])
		{
			if(s1[0]=='-')
				symbol='-';
		}
		else
		{
			if(s1[0]=='+'&&strcmp(s1.c_str()+1,s2.c_str()+1)<0||s1[0]=='-'&&strcmp(s1.c_str()+1,s2.c_str()+1)>0)
			{
				symbol='-';
			}
		}
		if(leftSize<rightSize)
		{
			swap(s1,s2);
			swap(leftSize,rightSize);
		}
		ret.resize(leftSize+1);
		memset((void*)ret.c_str(),'0',ret.size()*sizeof(char));
		ret[0]=symbol;
		char cret,cstep=0;
		for(int dx=1;dx<leftSize;++dx)
		{
			cret=s1[leftSize-dx]-'0'+cstep;
			if(dx<rightSize)
				cret+=(s2[rightSize-dx]-'0');
			cstep=cret/10;
			ret[leftSize-dx+1]=cret%10+'0';
		}
		ret[1]=cstep+'0';
		return ret;
	}
	string Sub(string& s1,string& s2)
	{
		int leftSize=s1.size();
		int rightSize=s2.size();
		char symbol=s1[0];
		if(leftSize<rightSize||leftSize==rightSize&&strcmp(s1.c_str()+1,s2.c_str()+1)<0)
		{
			swap(leftSize,rightSize);
			swap(s1,s2);
			if(symbol='+')
				symbol='-';
			else
				symbol='+';
		}
		//cout<<s1<<s2<<endl;
		string ret;
		ret.resize(leftSize);
		memset((void*)ret.c_str(),'0',ret.size()*sizeof(char));
		ret[0]=symbol;
		char cret;
		//cout<<ret<<endl;
		//cout<<s1<<s2<<endl;
		for(int dx=1;dx<leftSize;++dx)
		{
			cret=s1[leftSize-dx]-'0';
			if(dx<rightSize)
			{
				cret-=(s2[rightSize-dx]-'0');
			}
			if(cret<0)
			{
				s1[leftSize-dx-1]-=1;
				cret+=10;
			}
		//	cout<<cret+'0';
			ret[leftSize-dx]=cret+'0';
		}
		return ret;
	}
	string Mul(string& s1,string& s2)
	{
		int leftSize=s1.size();
		int rightSize=s2.size();
		if(leftSize<rightSize)
		{
			swap(leftSize,rightSize);
			swap(s1,s2);
		}
		string ret;
		ret.resize(leftSize+rightSize-1);
		memset((void*)ret.c_str(),'0',ret.size()*sizeof(char));
		char symbol='+';
		if(s1[0]!=s2[0])
			symbol='-';
		ret[0]=symbol;
		int dataLen=ret.size();
		int offSet=0;
		for(int dx=1;dx<rightSize;++dx)
		{
			char c1=s2[rightSize-dx]-'0';
			char cstep=0;
			if(c1==0)
			{
				++offSet;
				continue;
			}
			for(int ldx=1;ldx<leftSize;++ldx)
			{
				char cret=c1*(s1[leftSize-ldx]-'0');
				cret+=(cstep+ret[dataLen-ldx-offSet]-'0');
				ret[dataLen-ldx-offSet]=cret%10+'0';
				cstep=cret/10;
			}
			ret[dataLen-offSet-leftSize]=cstep+'0';
			++offSet;
		}
	//	cout<<ret.c_str();
		return ret;
	}
	string Div(string& s1,string& s2)
	{
		int leftSize=s1.size();
		int rightSize=s2.size();
		char symbol='+';
		if(s1[0]!=s2[0])
			symbol='-';
		if(leftSize<rightSize||leftSize==rightSize&&strcmp(s1.c_str()+1,s2.c_str()+1)<0)
		{
			return "0";
		}
		else if(s2=="1"||s2=="-1")
		{
			s1[0]=symbol;
			return s1;
		}
		char* left=(char*)s1.c_str()+1;
		char* right=(char*)s2.c_str()+1;
		int dataLen=rightSize-1;
		string ret;
		ret[0]=symbol;
	//	cout<<left<<right<<endl;
		for(int dx=0;dx<leftSize-rightSize+1;++dx)
		{
			if(!leftStrRight(left,dataLen,right,rightSize-1))
			{
				++dataLen;
				ret.append(1,'0');
				continue;
			}
			else
			{
				ret.append(1,subLoop(left,dataLen,right,rightSize-1));
				dataLen+=1;
			}
		}
		//cout<<ret.c_str()<<"RRRR";
		return ret;
	}
	char subLoop(char*& left,int& leftSize,char* right,int rightSize)
	{
		assert(left&&right);
		char cret='0';
		while(1)
		{
			if (!leftStrRight(left, leftSize, right, rightSize))
				break;
			int lLen=leftSize-1;
			int rLen=rightSize-1;
			while(lLen>=0&&rLen>=0)
			{
				if(left[lLen]<right[rLen])
				{
					if(lLen<1)
						break;
					left[lLen-1]-=1;
					left[lLen]+=10;
				}
				left[lLen]-=right[rLen];
				left[lLen]+='0';
				--lLen;
				--rLen;
			}
			while(*left=='0')
			{
				++left;
				--leftSize;
			}
			cret++;
		}
		return cret;
	}
	bool leftStrRight(char* left,int leftLen,char* right,int rightLen)
	{
		assert(left&&right);
		if(leftLen>rightLen||leftLen==rightLen&&strcmp(left,right)>0)
			return true;
		return false;
	}
private:
	INT_64 _data;
	string _pdata;		
};
void analy_query(char * query_string,vector<string>& v);
void obtain_query_string(string& query_string);
