#pragma once
#include<string>
#include<map>

class protobuf;

//����0 �䳤int
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
//����1 �̶�64λ 8�ֽ�
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
//����2 bin �� Ƕ���� Ƕ������ö��obj����
using t2 = std::string*;
//����3 4 �������� 

//����5 �̶�32λ 4�ֽ�
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
	//����ڵ���var������Щ
	t0 _varint;
	t1 _fixed64;
	t2 _bin;
	t5 _fixed32;

	//obj ����ڵ���obj���͵� �������
	std::map<std::string, protobuf*> *_obj;
	//arr ����ڵ����������͵� �������
	std::map<int, protobuf*> *_arr;
};
//�쳣��
class protobufException
{
public:
	enum exType
	{
		parseError,//��������
		valTypeError,//���ʹ��� Ҳ���ǻ�ȡֵ��ʱ�� ����������͵�
		nullError,//���ݲ�����
	};
public:
	protobufException(exType type);
	exType getExType();
private:
	exType _type;
};



class protobuf
{
	//ö��
public:
	//����ڵ������
	enum nodeType
	{
		node_varint = 0,
		node_fixed64 = 1,
		node_bin = 2,
		node_fixed32 = 5,
		node_obj,//��������
		node_arr,//��������
		node_null,
	};

public:
	protobuf();
	protobuf(nodeType ntype);
	protobuf(const protobuf& pb);
	~protobuf();
	//����
	void parse(std::string buff) throw(protobufException);
	//����
	std::string make();
	//��������Ĵ�С
	size_t size();
	//��Դת�� ����������� �ѱ������Դת�Ƹ�������� 
	void move(protobuf &pb);
	//������� ��ԭ��Ĭ�Ϲ���
	void clear();
	//�ж�����ڵ��ǲ���null
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

	
	//����
	pbdata _data;
	//�ڵ�����
	nodeType _nodetype; 
};

