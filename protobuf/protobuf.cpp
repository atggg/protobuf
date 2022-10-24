#include "protobuf.h"
#include<iostream>

t0::operator int()
{
	return _int;
}

t0::operator unsigned int()
{
	return _uint;
}

t0::operator long long()
{
	return _longlong;
}

t0::operator unsigned long long()
{
	return _ulonglong;
}

t0::operator bool()
{
	return _bool;
}

t1::operator double()
{
	return _double;
}

t1::operator int()
{
	return _int;
}

t1::operator unsigned int()
{
	return _uint;
}

t1::operator long long()
{
	return _longlong;
}

t1::operator unsigned long long()
{
	return _ulonglong;
}

t5::operator float()
{
	return _float;
}

t5::operator int()
{
	return _int;
}

t5::operator unsigned int()
{
	return _uint;
}

protobufException::protobufException(exType type)
{
	_type = type;
}

protobufException::exType protobufException::getExType()
{
	return _type;
}


protobuf::protobuf()
{

	//初始化一个就好了 一定要找占用空间最大的那个初始化
	_data._varint._longlong = 0;

	_vtype = varType::v_null;
	_ntype = nodeType::n_var; //默认是var
}

protobuf::protobuf(nodeType ntype, varType vtype)
{
	//初始化一个就好了 一定要找占用空间最大的那个初始化
	_data._varint._longlong = 0;

	_vtype = vtype;
	_ntype = ntype;
	if (_ntype == nodeType::n_arr)
	{
		_data._arr = new std::map<int, protobuf*>;
	}
	else if (_ntype == nodeType::n_obj)
	{
		_data._obj = new std::map<std::string, protobuf*>;
	}
	else if (_ntype == nodeType::n_var && _vtype == varType::v_bin)
	{
		_data._bin = new std::string();
	}
}

protobuf::protobuf(const protobuf& pb)
{
	//没办法 我喜欢偷懒啊
	_ntype = nodeType::n_var;
	_vtype = varType::v_null;
	*this = pb;
}

protobuf::~protobuf()
{
	clear();
}

void protobuf::parse(std::string buff) throw(protobufException)
{
	//初始化下本类
	clear();
	_ntype = nodeType::n_obj;
	_vtype = varType::v_null;
	_data._obj = new std::map<std::string, protobuf*>;


	protobuf temp;
	unsigned char* buffp = (unsigned char*) & buff[0];
	unsigned long long buffSize = buff.size();
	long long pos = 0;
	long long stag = -1;
	
	while (pos < buffSize)
	{
		//取出类型
		int type = buffp[pos] & 0x7;
		//取出tag
		long long tag = ((buffp[pos] >> 3) & 0xf);
		int i = 1;
		if (buffp[pos] > 0x7f)
		{
			//那就是后面还有数据
			do
			{
				pos++;
				unsigned int t = ((buffp[pos] &0x7f) << (4 * i));
				tag += t;
				i++;
				if (pos > buffSize)
				{
					throw protobufException(protobufException::exType::parseError);
				}
			} while (buffp[pos] > 0x7f);
		}
		if (tag <= 0 || (i== 1 && tag > buffp[pos]) || type > 0x5 || type == 3 || type == 4)
		{
			throw protobufException(protobufException::exType::parseError);
		}
		switch (type)
		{
			//变长int
			case varType::v_varint:
			{
				temp = deVarInt(buff,pos);
				break;
			}
			//固定64位
			case varType::v_fixed64:
			{
				temp = *(double *) ( & buff.substr(pos+1, 8)[0]);
				pos += 9;
				if (pos > buffSize)
				{
					throw protobufException(protobufException::exType::parseError);
				}
				break;
			}
			//bin 或者嵌套
			case varType::v_bin:
			{
				std::string tstr;
				unsigned long long len = deVarInt(buff, pos);
				//如果取出的长度比包含他的长度还大那就是异常了
				if (len > buffSize)
				{
					throw protobufException(protobufException::exType::parseError);
				}
				tstr = buff.substr(pos, len);
				//默认认为他是一个msg 解析失败了他就是bytes
				try
				{
					 temp.parse(tstr);
				}
				catch (const protobufException& e)
				{
					temp = tstr;
				}
				pos += len;
				break;
			}
			//固定32位
			case varType::v_fixed32:
			{
				temp = *(float*)(&buff.substr(pos+1, 4)[0]);
				pos += 5;
				if (pos > buffSize)
				{
					throw protobufException(protobufException::exType::parseError);
				}
				break;
			}
		}
		if (tag == stag)
		{
			auto pit =  (*this)._data._obj->find(std::to_string(tag));
			if (pit != (*this)._data._obj->end() && pit->second->_ntype != n_arr)
			{
				//创建一个数组类型的节点
				protobuf* tbuf = new protobuf(nodeType::n_arr, varType::v_null);
				tbuf->_data._arr->insert(std::make_pair(tbuf->_data._arr->size(), pit->second));

				protobuf* tempbuf = new protobuf;
				//把temp的资源转移给tempbuf
				temp.move(*tempbuf);
				tbuf->_data._arr->insert(std::make_pair(tbuf->_data._arr->size(), tempbuf));
				(*this)._data._obj->erase(pit);
				(*this)._data._obj->insert(std::make_pair(std::to_string(tag),tbuf));
			}
			else if (pit != (*this)._data._obj->end() && pit->second->_ntype == n_arr)
			{
				protobuf* tempbuf = new protobuf;
				//把temp的资源转移给tempbuf
				temp.move(*tempbuf);
				pit->second->_data._arr->insert(std::make_pair(pit->second->_data._arr->size(), tempbuf));
			}

		}
		else
		{
			protobuf* tempbuf = new protobuf;
			//把temp的资源转移给tbuf
			temp.move(*tempbuf);
			(*this)._data._obj->insert(std::make_pair(std::to_string(tag), tempbuf));
		}
		stag = tag;
	}
}


std::string protobuf::make()
{
	return make(0);
}

std::string protobuf::make(long long tag)
{
	//只有两种情况调用make 他是个数组或者是个对象 var 是不会调用make
	std::string makeStr;
	if (_ntype == nodeType::n_arr)
	{
		for (auto it = _data._arr->begin(); it != _data._arr->end(); it++)
		{
			switch (it->second->_ntype)
			{
				case nodeType::n_var:
				{
					makeStr+= makeVar(tag,it->second);
					break;
				}
				case nodeType::n_obj:
				{
					//这个节点是对象
					makeStr += makeHead(tag, varType::v_bin);
					std::string makes = it->second->make(0);
					makeStr += enVarInt(makes.size());
					makeStr += makes;
					break;
				}
			}
		}
	}
	else if (_ntype == nodeType::n_obj)
	{
		for (auto it = _data._obj->begin(); it != _data._obj->end(); it++)
		{
			if (it->second->_ntype == nodeType::n_obj)
			{
				makeStr += makeHead(std::stoll(it->first), varType::v_bin);
				std::string makes = it->second->make(0);
				makeStr += enVarInt(makes.size());
				makeStr += makes;
				
			}
			else
			{
				makeStr += makeVar(std::stoll(it->first), it->second);
			}
		}
	}
	return makeStr;

}


size_t protobuf::size()
{
	if (_ntype == nodeType::n_arr && _data._arr != nullptr)
	{
		return _data._arr->size();
	}
	return 0;
}

void protobuf::move(protobuf& pb)
{
	pb.clear();;
	pb._ntype = _ntype;
	pb._vtype = _vtype;
	pb._data = _data;
	//改变本类的类型 
	_data._varint._longlong = 0;
	_vtype = varType::v_null;
	_ntype = nodeType::n_var;
}

void protobuf::varint(int v)
{
	clear();
	_vtype = varType::v_varint;
	_ntype = nodeType::n_var;
	_data._varint._int = v;
}

void protobuf::varint(unsigned int v)
{
	clear();
	_vtype = varType::v_varint;
	_ntype = nodeType::n_var;
	_data._varint._uint = v;
}

void protobuf::varint(long long v)
{
	clear();
	_vtype = varType::v_varint;
	_ntype = nodeType::n_var;
	_data._varint._longlong = v;
}

void protobuf::varint(unsigned long long v)
{
	 clear();
	_vtype = varType::v_varint;
	_ntype = nodeType::n_var;
	_data._varint._ulonglong = v;
}

void protobuf::varint(bool v)
{
	clear();
	_vtype = varType::v_varint;
	_ntype = nodeType::n_var;
	_data._varint._bool = v;
}

void protobuf::fixed64(double v)
{
	clear();
	_vtype = varType::v_fixed64;
	_ntype = nodeType::n_var;
	_data._fixed64._double = v;
}

void protobuf::fixed64(int v)
{
	clear();
	_vtype = varType::v_fixed64;
	_ntype = nodeType::n_var;
	_data._fixed64._int = v;
}

void protobuf::fixed64(unsigned int v)
{
	clear();
	_vtype = varType::v_fixed64;
	_ntype = nodeType::n_var;
	_data._fixed64._uint = v;
}

void protobuf::fixed64(long long v)
{
	clear();
	_vtype = varType::v_fixed64;
	_ntype = nodeType::n_var;
	_data._fixed64._longlong = v;
}

void protobuf::fixed64(unsigned long long v)
{
	clear();
	_vtype = varType::v_fixed64;
	_ntype = nodeType::n_var;
	_data._fixed64._ulonglong = v;
}

void protobuf::bin(std::string v)
{
	clear();
	_vtype = varType::v_bin;
	_ntype = nodeType::n_var;
	_data._bin = new std::string(v);
}


void protobuf::fixed32(float v)
{
	clear();
	_vtype = varType::v_fixed32;
	_ntype = nodeType::n_var;
	_data._fixed32._float = v;
}

void protobuf::fixed32(int v)
{
	clear();
	_vtype = varType::v_fixed32;
	_ntype = nodeType::n_var;
	_data._fixed32._int = v;
}

void protobuf::fixed32(unsigned int v)
{
	clear();
	_vtype = varType::v_fixed32;
	_ntype = nodeType::n_var;
	_data._fixed32._uint = v;
}

t0 protobuf::varint()
{
	if (_vtype == varType::v_null)
	{
		throw protobufException(protobufException::exType::nullError);
	}
	if (_vtype != varType::v_varint)
	{
		throw protobufException(protobufException::exType::valTypeError);
	}
	return _data._varint;
}

t1 protobuf::fixed64()
{
	if (_vtype == varType::v_null)
	{
		throw protobufException(protobufException::exType::nullError);
	}
	if (_vtype != varType::v_fixed64)
	{
		throw protobufException(protobufException::exType::valTypeError);
	}
	return _data._fixed64;
}

std::string protobuf::bin()
{
	if (_vtype == varType::v_null)
	{
		throw protobufException(protobufException::exType::nullError);
	}
	if (_vtype != varType::v_bin)
	{
		throw protobufException(protobufException::exType::valTypeError);
	}
	return *_data._bin;
}

t5 protobuf::fixed32()
{
	if (_vtype == varType::v_null)
	{
		throw protobufException(protobufException::exType::nullError);
	}
	if (_vtype != varType::v_fixed32)
	{
		throw protobufException(protobufException::exType::valTypeError);
	}
	return _data._fixed32;
}

bool protobuf::operator==(nullptr_t)
{
	if (_ntype == nodeType::n_var && _vtype != varType::v_null)
	{
		return false;
	}
	else if (_ntype == nodeType::n_arr && _data._arr != nullptr && _data._arr->size() != 0)
	{
		return false;
	}
	else if (_ntype == nodeType::n_obj && _data._obj != nullptr && _data._obj->size() != 0)
	{
		return false;
	}
	return true;
}

protobuf& protobuf::operator[](std::string index)
{
	if (_ntype != nodeType::n_obj)
	{
		clear();
		_data._obj = new std::map<std::string, protobuf*>;
		_vtype = varType::v_null;
		_ntype = nodeType::n_obj;
	}
	auto it = _data._obj->find(index);
	if (it != _data._obj->end())
	{
		return *it->second;
	}
	else
	{
		auto t = new protobuf;
		_data._obj->insert(std::make_pair(index,t));
		return *t;
	}
	
}

protobuf& protobuf::operator[](const char* index)
{
	return (*this)[std::string(index)];
}


protobuf& protobuf::operator[](int index)
{
	
	
	if (_ntype != nodeType::n_arr)
	{
		clear();
		_data._arr = new std::map<int, protobuf*>;
		_vtype = varType::v_null;
		_ntype = nodeType::n_arr;
	}
	auto it = _data._arr->find(index);
	if (it != _data._arr->end())
	{
		return *it->second;
	}
	else
	{
		auto t = new protobuf;
		_data._arr->insert(std::make_pair(index, t));
		return *t;
	}

}

protobuf& protobuf::operator=(const protobuf& pb)
{
	clear();
	_vtype = pb._vtype;
	_ntype = pb._ntype;
	//还不知道是什么类型 先直接拷贝数据
	_data = pb._data;
	//如果是数组
	if (_ntype == nodeType::n_arr)
	{
		_data._arr = new std::map<int, protobuf*>;
		for (auto it = pb._data._arr->begin(); it != pb._data._arr->end(); it++)
		{
			_data._arr->insert(std::make_pair(it->first, new protobuf(*it->second)));
		}
	}
	else if (pb._ntype == nodeType::n_obj)
	{
		_data._obj = new std::map<std::string, protobuf*>;
		for (auto it = pb._data._obj->begin(); it != pb._data._obj->end(); it++)
		{
			_data._obj->insert(std::make_pair(it->first, new protobuf(*it->second)));
		}
	}else if (pb._ntype == nodeType::n_var && pb._vtype == varType::v_bin)
	{
		_data._bin = new std::string(*pb._data._bin);
	}
	
	return *this;
}

void protobuf::operator=(std::string v)
{
	bin(v);
}

void protobuf::operator=(int v)
{
	varint(v);
}

void protobuf::operator=(long long v)
{
	varint(v);
}

void protobuf::operator=(double v)
{
	fixed64(v);
}

void protobuf::operator=(float v)
{
	fixed32(v);
}

protobuf::operator int()
{
	return varint();
}

protobuf::operator unsigned int()
{
	return varint();
}

protobuf::operator long long()
{
	return varint();
}

protobuf::operator unsigned long long()
{
	return varint();
}

protobuf::operator bool()
{
	return varint();
}

protobuf::operator double()
{
	return fixed64();
}

protobuf::operator std::string()
{
	return bin();
}

protobuf::operator float()
{
	return fixed32();
}

std::string protobuf::makeHead(long long tag, int type)
{
	std::string restr;
	long long t = (tag << 3);
	t |= type;
	do
	{
		unsigned char l = t & 0x7f; //取出低7位
		t = t >> 7;//右移动7位
		//判断是否！=0
		if (t != 0)
		{
			l |= 0x80;//第8位置1
		}
		restr += l;
	} while (t != 0);
	return restr;
}

std::string protobuf::enVarInt(long long v)
{
	long long temp = v;
	std::string restr;
	do
	{
		//先取出低7位
		unsigned char c = temp & 0x7f;
		temp = temp >> 7; //右移动7位
		//判断temp是不是 = 0
		if (temp != 0)
		{
			//如果不是0
			c |= 0x80;//把第8位置1 表示后面还有数据
		}
		restr += c;
	} while (temp != 0);
	return restr;
}

long long protobuf::deVarInt(std::string& buff, long long &pos) throw(protobufException)
{
	long long ret = 0;
	long long i = 1;
	do
	{

		pos++;
		if (pos > buff.size())
		{
			throw protobufException(protobufException::exType::parseError);
		}
		ret = ret + ((buff[pos] & 0x7f) * i);
		i = i * 0x80;
		
	} while ((unsigned char)buff[pos] > 0x7f);
	pos++;
	return ret;
}

void protobuf::clear()
{
	//这个节点是个数组 
	if (_ntype == nodeType::n_arr && _data._arr != nullptr)
	{
		for (auto it = _data._arr->begin(); it != _data._arr->end(); it++)
		{
			if (it->second != nullptr)
			{
				delete it->second;
			}
		}
		_data._arr->clear();
		delete _data._arr;
		_data._arr = nullptr;
	}
	if (_ntype == nodeType::n_obj && _data._obj != nullptr)
	{
		for (auto it = _data._obj->begin(); it != _data._obj->end(); it++)
		{
			if (it->second != nullptr)
			{
				delete it->second;
			}
		}
		_data._obj->clear();
		delete _data._obj;
		_data._obj = nullptr;
	}
	//这个节点是val 并且他是 bin类型的
	if (_vtype == varType::v_bin && _ntype == nodeType::n_var && _data._bin != nullptr)
	{
		delete _data._bin;
		_data._bin = nullptr;
	}
	_ntype = nodeType::n_var;
	_vtype = varType::v_null;
	_data._varint._longlong = 0;
}

std::string protobuf::makeVar(long long tag,protobuf* var)
{
	std::string makeStr;
	if (var->_ntype != nodeType::n_arr)
	{
		makeStr += makeHead(tag,var->_vtype);
	}
	else if (var->_ntype == nodeType::n_arr)
	{
		std::string smake = var->make(tag);
		makeStr += smake; //添加数据
	}
	switch (var->_vtype)
	{
		//变长int
		case varType::v_varint:
		{
			makeStr += enVarInt(var->_data._varint._longlong);
			break;
		}
		//固定8字节
		case varType::v_fixed64:
		{
			makeStr += std::string((char*)var->_data._fixed64._fixed64, 0x8);
			break;
		}
		//bin类型 可能是byte 或者是嵌套
		case varType::v_bin:
		{
			if (var->_ntype == nodeType::n_var)
			{
				//先得到他的长度
				std::string len = enVarInt(var->_data._bin->size());
				//添加他的长度
				makeStr += len;
				//再添加他的数据
				makeStr.append(*var->_data._bin);
			}
			else if (var->_ntype == nodeType::n_obj)
			{
				std::string smake = var->make();
				makeStr += enVarInt(smake.size()); //先取出大小
				makeStr += smake; //添加数据
			}
			break;
		}
		//固定4字节
		case varType::v_fixed32:
		{
			makeStr += std::string((char*)var->_data._fixed32._fixed32, 0x4);
			break;
		}
	}
	return makeStr;
}


