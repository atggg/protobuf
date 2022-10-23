#pragma once
#include<string>
#include<map>


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
union data
{
	t0 _varint;
	t1 _fixed64;
	t2 _bin;
	t5 _fixed32;
};
//�쳣��
class protobufException
{
public:
	enum exType
	{
		parseError,//��������
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
	//��������
	enum varType
	{
		v_varint = 0,
		v_fixed64 = 1,
		v_bin = 2,
		v_fixed32 = 5,
		v_null = 6,
	};
	//����ڵ������
	enum nodeType
	{
		n_obj,//��������
		n_arr,//��������
		n_var,//��������
	};

public:
	protobuf();
	protobuf(const protobuf& pb);
	~protobuf();
	//����
	static protobuf parse(std::string buff) throw(protobufException);
	//����
	std::string make();
	//��������Ĵ�С
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

	
	//����
	data _data;
	//��������
	varType _vtype; //����ڵ����������
	//�ڵ�����
	nodeType _ntype;
	//obj ����ڵ���obj���͵� �������
	std::map<std::string, protobuf*> _obj;
	//arr ����ڵ����������͵� �������
	std::map<int, protobuf*> _arr;
};
