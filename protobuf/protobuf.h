#pragma once
#include<string>
#include<map>

class protobuf;

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
union pbdata
{
	//这个节点是var就用这些
	t0 _varint;
	t1 _fixed64;
	t2 _bin;
	t5 _fixed32;

	//obj 这个节点是obj类型的 就用这个
	std::map<std::string, protobuf*> *_obj;
	//arr 这个节点是数组类型的 就用这个
	std::map<int, protobuf*> *_arr;
};
//异常类
class protobufException
{
public:
	enum exType
	{
		parseError,//解析出错
		valTypeError,//类型错误 也就是获取值的时候 不是这个类型的
		nullError,//内容不存在
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
	//这个节点的类型
	enum nodeType
	{
		node_varint = 0,
		node_fixed64 = 1,
		node_bin = 2,
		node_fixed32 = 5,
		node_obj,//对象类型
		node_arr,//数组类型
		node_null,
	};

public:
	protobuf();
	protobuf(nodeType ntype);
	protobuf(const protobuf& pb);
	~protobuf();
	//解析
	void parse(std::string buff) throw(protobufException);
	//生成
	std::string make();
	//返回数组的大小
	size_t size();
	//资源转移 会清空两个类 把本类的资源转移给传入的类 
	void move(protobuf &pb);
	//清空数据 还原到默认构造
	void clear();
	//判断这个节点是不是null
	bool  operator==(nullptr_t);
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

	//string, bytes
	void bin(std::string v);

	//fixed32, sfixed32, float
	void fixed32(float v);
	void fixed32(int v);
	void fixed32(unsigned int v);

	t0 varint() throw (protobufException);
	t1 fixed64() throw (protobufException);
	std::string bin() throw (protobufException);
	t5 fixed32() throw (protobufException);

	protobuf& operator[](std::string index);
	protobuf& operator[](const char* index);
	protobuf& operator[](int index);
	protobuf& operator=(const protobuf& pb);


	void operator=(std::string v);
	void operator=(int v);
	void operator=(long long v);
	void operator=(double v);
	void operator=(float v);


	
	//varint
	operator int();
	operator unsigned int();
	operator long long();
	operator unsigned long long();
	operator bool();
	//fixed64
	operator double();
	//bin
	operator std::string();
	//fixed32
	operator float();

private:
	std::string makeVar(long long tag,protobuf * var);
	std::string makeHead(long long tag,int type);
	static std::string enVarInt(long long v);
	static long long deVarInt(std::string &buff,long long &pos) throw(protobufException);
	std::string make(long long tag);
private:

	
	//数据
	pbdata _data;
	//节点类型
	nodeType _nodetype; 
};

