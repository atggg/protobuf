#include<iostream>
#include"protobuf.h"
#include<fstream>


std::string bintohex(std::string& bin)
{
	std::string ret;
	const unsigned char* bp = (unsigned char*) & bin[0];
	size_t len = bin.size();
	char d[10] = { 0 };
	for (size_t i = 0; i < len; i++)
	{
		if (bp[i] <= 0xf)
		{
			ret.append("0");
		}
		ret.append(_itoa(bp[i],d,0x10));
		ret.append(" ");
	}
	return ret;
}

unsigned char tobin(unsigned char c)
{
	if (c > 0x2f && c < 0x3A)
	{
		return c - 0x30;
	}
	else if (c > 0x40 && c < 0x47)
	{
		return c - 0x37;
	}
	else if (c > 0x60 && c < 0x67)
	{
		return c - 0x57;
	}
}

std::string hextobin(std::string hex)
{
	std::string thex = hex;
	std::string ret;
	//ȥ���ո�
	for (size_t i = 0; i < thex.size(); i++)
	{
		if (thex[i] == ' ' || thex[i] == '\n' || thex[i] == '\t' || thex[i] == '\r')
		{
			thex.erase(i,1);
		}
	}
	for (size_t i = 0; i < thex.size();)
	{
		unsigned char t = 0;
		unsigned char h = thex[i];
		unsigned char l = thex[i + 1];
		h = tobin(h);
		l = tobin(l);
		t = h << 4;
		t |= l;
		ret += t;
		i += 2;
	}
	return ret;
}


void test1()
{
	protobuf buf;

	buf["1"]["1"] = u8"At";
	buf["1"]["2"] = 1;
	buf["2"]["1"] = 3.1415926;
	buf["2"]["2"] = 5.20F;
	buf["3"] = u8"[string] ����Tag [int]���������±� ";
	buf["4"] = u8"����ʹ������Ƕ������ ����buf[\"1\"][0][0].bind(\"test\") �ǷǷ��� buf[\"1\"][0][\"0\"] ���ǺϷ���";
	buf["5"] = u8"���ڵ�һ��Ҫ�Ƕ���  ����buf[0][\"\"].bin(\"test\")  buf[0].bin(\"test\") �ǷǷ���    buf[\"1\"][0].bin(\"test\")     buf[\"1\"].bin(\"test\")  ���ǺϷ���";
	buf["6"]["1"][0] = u8"varint (�䳤int) ���Դ洢  int32, int64, uint32, uint64,  bool, enum";
	buf["6"]["2"][0] = u8"fixed64 (�̶�64λ Ҳ����8�ֽ�) ���Դ洢 double ,int32, int64, uint32, uint64";
	buf["6"]["3"][0] = u8"bin (bytes ��������std::string ȥ����) ���Դ洢 bytes string messageǶ��  �����Ҫ���õ���message��ϢǶ�׵Ļ� ֻ���� (=) ���� ����(buf[\"1\"] = test[\"2\"])";
	buf["6"]["4"][0] = u8"fixed32 (�̶�32λ Ҳ����4�ֽ�) ���Դ洢 float ,int32 ,uint32";
	buf["6"]["1"][1] = u8"varint (�䳤int) ���Դ洢  int32, int64, uint32, uint64,  bool, enum";
	buf["6"]["2"][1] = u8"fixed64 (�̶�64λ Ҳ����8�ֽ�) ���Դ洢 double ,int32, int64, uint32, uint64";
	buf["6"]["3"][1] = u8"bin (bytes ��������std::string ȥ����) ���Դ洢 bytes string messageǶ��  �����Ҫ���õ���message��ϢǶ�׵Ļ� ֻ���� (=) ���� ����(buf[\"1\"] = test[\"2\"])";
	buf["6"]["4"][1] = u8"fixed32 (�̶�32λ Ҳ����4�ֽ�) ���Դ洢 float ,int32 ,uint32";

	//test Exception

	try
	{
		std::string t = buf["1"]; //����msgǶ�����͵����� ����stringȥ����  һ�����쳣
		//std::string t = buf["1"].make(); //���ҵ�������make��ʽ��û��
	}
	catch (protobufException& e)
	{
		switch (e.getExType())
		{
			case protobufException::exType::nullError:
			{
				std::cout << "��null û�д洢�κ�����" << std::endl;
				break;
			}
			case protobufException::exType::valTypeError:
			{
				std::cout << "���ʹ���" << std::endl;
				break;
			}
		}
	}
	


	//test make parse
	std::string pbstr = buf.make();
	std::cout << bintohex(pbstr) << std::endl;
	protobuf pb;
	pb.parse(pbstr);
	pbstr.clear();
	pbstr = pb.make();
	std::cout << std::endl;
	std::cout << bintohex(pbstr) << std::endl;

	/*std::ofstream ofs("./buf", std::ios::ate | std::ios::out);

	ofs.write(pbstr.c_str(), pbstr.size());*/
	std::system("chcp 65001");
	std::string test = pb.print(false);
	std::cout << test << std::endl;
}

void test2()
{
	std::system("chcp 65001");
	std::ifstream ifo("C:\\Users\\At\\Desktop\\1.bin",std::ios::ate,std::ios::binary);
	size_t size = ifo.tellg();
	ifo.seekg(std::ios::beg);
	std::string buff(size,0);
	ifo.read(&buff[0],size);
	ifo.close();
	std::string binstr = hextobin(buff);
	protobuf buf;
	buf.parse(binstr);
	std::string pstr = buf.print(false);
	std::cout << pstr << std::endl;
}
int main()
{
	//test1();
	test2();
	return 0;
}