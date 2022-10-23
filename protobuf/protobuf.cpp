#include "protobuf.h"

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
	return exType();
}


protobuf::protobuf()
{

	//����һ���ͺ���
	_data._bin = nullptr;

	_vtype = varType::v_null;
	_ntype = nodeType::n_obj; //Ĭ����obj
}

protobuf::protobuf(const protobuf& pb)
{
	//û�취 ��ϲ��͵����
	*this = pb;
}

protobuf::~protobuf()
{
	clearArrObj();
}

protobuf protobuf::parse(std::string buff) throw(protobufException)
{
	protobuf pb;
	protobuf temp;
	unsigned char* buffp = (unsigned char*) & buff[0];
	unsigned long long buffSize = buff.size();
	long long pos = 0;
	long long stag = -1;
	
	while (pos < buffSize)
	{
		//ȡ������
		int type = buffp[pos] & 0x7;
		varType types = varType::v_null;
		//ȡ��tag
		long long tag = ((buffp[pos] >> 3) & 0xf);
		int i = 1;
		if (buffp[pos] > 0x7f)
		{
			//�Ǿ��Ǻ��滹������
			do
			{
				pos++;
				unsigned int t = (buffp[pos] &0x7f) << (7 * i);
				tag += t;
				i++;
			} while (buffp[pos] > 0x7f);
		}
		if (tag <= 0 || tag > buffp[pos] || type > 0x5 || type == 3 || type == 4)
		{
			throw protobufException(protobufException::exType::parseError);
		}
		switch (type)
		{
			//�䳤int
			case varType::v_varint:
			{
				types = varType::v_varint;
				temp = deVarInt(buff,pos);
				break;
			}
			//�̶�64λ
			case varType::v_fixed64:
			{
				types = varType::v_fixed64;
				temp = *(double *) ( & buff.substr(pos, 8)[0]);
				pos += 8;
				break;
			}
			//bin ����Ƕ��
			case varType::v_bin:
			{
				types = varType::v_bin;
				
				std::string tstr;
				unsigned long long len = deVarInt(buff, pos);
				tstr = buff.substr(pos, len);
				//Ĭ����Ϊ����һ��msg ����ʧ����������bytes
				try
				{
					temp = protobuf::parse(tstr);
				}
				catch (const protobufException& e)
				{
					temp = tstr;
				}
				pos += len;
				break;
			}
			//�̶�32λ
			case varType::v_fixed32:
			{
				types = varType::v_fixed32;
				temp = *(float*)(&buff.substr(pos, 4)[0]);
				pos += 4;
				break;
			}
		}
		if (tag == stag)
		{
			auto pit =  pb._obj.find(std::to_string(tag));
			if (pit != pb._obj.end() && pit->second->_ntype != n_arr)
			{
				protobuf* tbuf = new protobuf;
				tbuf->_arr.insert(std::make_pair(tbuf->_arr.size(), pit->second));
				tbuf->_arr.insert(std::make_pair(tbuf->_arr.size(), new protobuf(temp)));
				tbuf->_ntype = nodeType::n_arr;
				tbuf->_vtype = types;
				pb._obj.erase(pit);
				pb._obj.insert(std::make_pair(std::to_string(tag),tbuf));
			}
			else if (pit != pb._obj.end() && pit->second->_ntype == n_arr)
			{
				pit->second->_arr.insert(std::make_pair(pit->second->_arr.size(), new protobuf(temp)));
			}

		}
		else
		{
			pb._obj.insert(std::make_pair(std::to_string(tag), new protobuf(temp)));
		}
		stag = tag;
	}
	return pb;
}


std::string protobuf::make()
{
	return make(0);
}

std::string protobuf::make(long long tag)
{
	//ֻ�������������make ���Ǹ���������Ǹ����� var �ǲ������make
	std::string makeStr;
	if (_ntype == nodeType::n_arr)
	{
		for (auto it = _arr.begin(); it != _arr.end(); it++)
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
					//����ڵ��Ƕ���
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
		for (auto it = _obj.begin(); it != _obj.end(); it++)
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

bool protobuf::parseObj(protobuf* pb, std::string& buff, unsigned long long& pos)
{
	return false;
}

size_t protobuf::size()
{
	return _arr.size();
}

void protobuf::varint(int v)
{
	clearArrObj();
	_vtype = varType::v_varint;
	_ntype = nodeType::n_var;
	_data._varint._int = v;
}

void protobuf::varint(unsigned int v)
{
	clearArrObj();
	_vtype = varType::v_varint;
	_ntype = nodeType::n_var;
	_data._varint._uint = v;
}

void protobuf::varint(long long v)
{
	clearArrObj();
	_vtype = varType::v_varint;
	_ntype = nodeType::n_var;
	_data._varint._longlong = v;
}

void protobuf::varint(unsigned long long v)
{
	clearArrObj();
	_vtype = varType::v_varint;
	_ntype = nodeType::n_var;
	_data._varint._ulonglong = v;
}

void protobuf::varint(bool v)
{
	clearArrObj();
	_vtype = varType::v_varint;
	_ntype = nodeType::n_var;
	_data._varint._bool = v;
}

void protobuf::fixed64(double v)
{
	clearArrObj();
	_vtype = varType::v_fixed64;
	_ntype = nodeType::n_var;
	_data._fixed64._double = v;
}

void protobuf::fixed64(int v)
{
	clearArrObj();
	_vtype = varType::v_fixed64;
	_ntype = nodeType::n_var;
	_data._fixed64._int = v;
}

void protobuf::fixed64(unsigned int v)
{
	clearArrObj();
	_vtype = varType::v_fixed64;
	_ntype = nodeType::n_var;
	_data._fixed64._uint = v;
}

void protobuf::fixed64(long long v)
{
	clearArrObj();
	_vtype = varType::v_fixed64;
	_ntype = nodeType::n_var;
	_data._fixed64._longlong = v;
}

void protobuf::fixed64(unsigned long long v)
{
	clearArrObj();
	_vtype = varType::v_fixed64;
	_ntype = nodeType::n_var;
	_data._fixed64._ulonglong = v;
}

void protobuf::bin(std::string v)
{
	clearArrObj();
	_vtype = varType::v_bin;
	_ntype = nodeType::n_var;
	_data._bin = new std::string(v);
}


void protobuf::fixed32(float v)
{
	clearArrObj();
	_vtype = varType::v_fixed32;
	_ntype = nodeType::n_var;
	_data._fixed32._float = v;
}

void protobuf::fixed32(int v)
{
	clearArrObj();
	_vtype = varType::v_fixed32;
	_ntype = nodeType::n_var;
	_data._fixed32._int = v;
}

void protobuf::fixed32(unsigned int v)
{
	clearArrObj();
	_vtype = varType::v_fixed32;
	_ntype = nodeType::n_var;
	_data._fixed32._uint = v;
}

t0 protobuf::varint()
{
	return _data._varint;
}

t1 protobuf::fiexid64()
{
	return _data._fixed64;
}

std::string protobuf::bin()
{
	return *_data._bin;
}

t5 protobuf::fixed32()
{
	return _data._fixed32;
}

protobuf& protobuf::operator[](std::string index)
{
	_ntype = nodeType::n_obj;
	auto it = _obj.find(index);
	//����ȷ��������ڵ���obj����
	if (it != _obj.end())
	{
		return *it->second;
	}
	else
	{
		auto t = new protobuf;
		_obj.insert(std::make_pair(index,t));
		return *t;
	}
	
}

protobuf& protobuf::operator[](int index)
{
	//����ȷ���� ����ڵ��������arr
	_vtype = varType::v_bin;
	_ntype = nodeType::n_arr;
	auto it = _arr.find(index);
	if (it != _arr.end())
	{
		return *it->second;
	}
	else
	{
		auto t = new protobuf;
		_arr.insert(std::make_pair(index, t));
		return *t;
	}

}

protobuf& protobuf::operator=(const protobuf& pb)
{
	clearArrObj();
	_vtype = pb._vtype;
	_ntype = pb._ntype;
	_data = pb._data;
	//���������Ϊ�˿���bin
	if (pb._ntype == nodeType::n_var && pb._vtype == varType::v_bin)
	{
		_data._bin = new std::string(*pb._data._bin);
		return *this;
	}
	else if (pb._ntype == nodeType::n_arr)
	{
		for (auto it = pb._arr.begin(); it != pb._arr.end(); it++)
		{
			_arr.insert(std::make_pair(it->first, new protobuf(*it->second)));
		}
	}
	else if (pb._ntype == nodeType::n_obj)
	{
		for (auto it = pb._obj.begin(); it != pb._obj.end(); it++)
		{
			_obj.insert(std::make_pair(it->first, new protobuf(*it->second)));
		}
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

protobuf::operator std::string()
{
	return *_data._bin;
}

std::string protobuf::makeHead(long long tag, int type)
{
	std::string restr;
	long long t = (tag << 3);
	t |= type;
	do
	{
		unsigned char l = t & 0x7f; //ȡ����7λ
		t = t >> 7;//���ƶ�7λ
		//�ж��Ƿ�=0
		if (t != 0)
		{
			l |= 0x80;//��8λ��1
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
		//��ȡ����7λ
		unsigned char c = temp & 0x7f;
		temp = temp >> 7; //���ƶ�7λ
		//�ж�temp�ǲ��� = 0
		if (temp != 0)
		{
			//�������0
			c |= 0x80;//�ѵ�8λ��1 ��ʾ���滹������
		}
		restr += c;
	} while (temp != 0);
	return restr;
}

long long protobuf::deVarInt(std::string& buff, long long &pos)
{
	pos++;
	long long ret = 0;
	long long i = 1;
	do
	{
		ret = ret + ((buff[pos] & 0x7f) * i);
		pos++;
		i = i * 0x80;
	} while ((unsigned char)buff[pos] > 0x7f);
	return ret;
}

void protobuf::clearArrObj()
{
	//Ч�ʵ�ʱ���Ż�
	for (auto it = _arr.begin(); it != _arr.end(); it++)
	{
		if (it->second != nullptr)
		{
			delete it->second;
		}
	}
	for (auto it = _obj.begin(); it != _obj.end(); it++)
	{
		if (it->second != nullptr)
		{
			delete it->second;
		}
	}
	if (_vtype == varType::v_bin && _ntype == nodeType::n_var && _data._bin != nullptr)
	{
		delete _data._bin;
		_data._bin = nullptr;
	}
	_arr.clear();
	_obj.clear();
}

std::string protobuf::makeVar(long long tag,protobuf* var)
{
	std::string makeStr;
	if (var->_ntype != nodeType::n_arr)
	{
		makeStr += makeHead(tag,var->_vtype);
	}
	switch (var->_vtype)
	{
		//�䳤int
		case varType::v_varint:
		{
			makeStr += enVarInt(var->_data._varint._longlong);
			break;
		}
		//�̶�8�ֽ�
		case varType::v_fixed64:
		{
			makeStr += std::string((char*)var->_data._fixed64._fixed64, 0x8);
			break;
		}
		//bin���� ������byte ������Ƕ��
		case varType::v_bin:
		{
			if (var->_ntype == nodeType::n_var)
			{
				//�ȵõ����ĳ���
				std::string len = enVarInt(var->_data._bin->size());
				//������ĳ���
				makeStr += len;
				//�������������
				makeStr.append(*var->_data._bin);
			}
			 else if (var->_ntype == nodeType::n_arr)
			{
				std::string smake = var->make(tag);
				makeStr += smake; //�������
			}
			else if (var->_ntype == nodeType::n_obj)
			{
				std::string smake = var->make();
				makeStr += enVarInt(smake.size()); //��ȡ����С
				makeStr += smake; //�������
			}
			break;
		}
		//�̶�4�ֽ�
		case varType::v_fixed32:
		{
			makeStr += std::string((char*)var->_data._fixed32._fixed32, 0x4);
			break;
		}
	}
	return makeStr;
}


