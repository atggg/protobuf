#pragma once
#include<string>
#include<map>


//类型0 变长int
union t0
{
	operator int();
	operator unsigned int();
	operator long long();
	operator unsigned long long();
	operator bool();

	int _int;
	unsigned _uint;
	long long _longlong;
	unsigned long long _ulonglong;
	bool _bool;
};
//类型1 固定64位 8字节
union t1
{
	operator double();
	operator int();
	operator unsigned int();
	operator long long();
	operator unsigned long long();

	double _double;
	int _int;
	unsigned int _uint;
	long long _longlong;
	unsigned long long _ulonglong;
	unsigned char _fixed64[8];
};
//类型2 bin 型 嵌套型 嵌套型用枚举obj代替
using t2 = std::string*;
//类型3 4 被弃用了 

//类型5 固定32位 4字节
union t5
{
	operator float();
	operator int();
	operator unsigned int();

	float _float;
	int _int;
	unsigned int _uint;
	unsigned char _fixed32[4];
};
union data
{
	t0 _varint;
	t1 _fixed64;
	t2 _bin;
	t5 _fixed32;
};
//异常类
class protobufException
{
public:
	enum exType
	{
		parseError,//解析出错
	};
public:
	protobufException(exType type);
	exType getExType();
private:
	exType _type;
};



class protobuf
{
	//枚举
public:
	//变量类型
	enum varType
	{
		v_varint = 0,
		v_fixed64 = 1,
		v_bin = 2,
		v_fixed32 = 5,
		v_null = 6,
	};
	//这个节点的类型
	enum nodeType
	{
		n_obj,//对象类型
		n_arr,//数组类型
		n_var,//变量类型
	};

public:
	protobuf();
	protobuf(const protobuf& pb);
	~protobuf();
	//解析
	static protobuf parse(std::string buff) throw(protobufException);
	//生成
	std::string make();
	//返回数组的大小
	size_t size();

	//int32, int64, uint32, uint64, sint32, sint64, bool, enum
	void varint(int v);
	void varint(unsigned int v);
	void varint(long long v);
	void varint(unsigned long long v);
	void varint(bool v);
	

	//fixed64, sfixed64, double
	void fixed64(double v);
	void fixed64(int v);
	void fixed64(unsigned int v);
	void fixed64(long long v);
	void fixed64(unsigned long long v);

	//string, bytes, embedded messages, packed repeated fields
	void bin(std::string v);

	//fixed32, sfixed32, float
	void fixed32(float v);
	void fixed32(int v);
	void fixed32(unsigned int v);

	t0 varint();
	t1 fiexid64();
	std::string bin();
	t5 fixed32();

	protobuf& operator[](std::string index);
	protobuf& operator[](int index);
	protobuf& operator=(const protobuf& pb);


	void operator=(std::string v);
	void operator=(int v);
	void operator=(long long v);
	void operator=(double v);
	void operator=(float v);


	operator std::string();

private:
	void clearArrObj();
	std::string makeVar(long long tag,protobuf * var);
	std::string makeHead(long long tag,int type);
	static std::string enVarInt(long long v);
	static long long deVarInt(std::string &buff,long long &pos);
	std::string make(long long tag);
	static bool parseObj(protobuf *pb,std::string &buff,unsigned long long & pos);
private:

	
	//数据
	data _data;
	//数据类型
	varType _vtype; //这个节点的数据类型
	//节点类型
	nodeType _ntype;
	//obj 这个节点是obj类型的 就用这个
	std::map<std::string, protobuf*> _obj;
	//arr 这个节点是数组类型的 就用这个
	std::map<int, protobuf*> _arr;
};

