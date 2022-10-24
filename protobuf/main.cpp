#include<iostream>
#include"protobuf.h"
#include<functional>


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
		if (thex[i] == ' ')
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
	//����Ҳ����
	//buf["1"] = 1;
	//buf["2"][0] = "77777";
	//buf["2"][1] = "nbnbnbnb";
	//buf["3"]["1"] = "66666";
	//buf["3"]["2"] = "777777";
	//buf["4"][0]["1"] = "test1";
	//buf["4"][0]["2"] = "test1";
	//buf["4"][1]["1"] = "test2";
	//buf["4"][1]["2"] = "test2";

	buf["1"]["2"].bin("At");
	buf["1"]["3"].varint(9359096);
	buf["4"]["5"].fixed64(4.211218);
	buf["4"]["6"].fixed32(15.01F);
	buf["7"].bin(u8"֧�� ����Ҽ�������HEX/{1,2,3} �� ����PB�����ļ�");
	buf["8"]["10"].bin(u8":1��[string]�ڱ�ʾ·���������[int]��ʾͬ·��ʱ�Ĳ�ͬ����");
	buf["8"]["11"].bin(u8":2��[]���(HEX)��ʾ��·����Ӧ��HEX  = ���ʾ����");
	buf["8"]["12"].bin(u8":3��(i)=���� (f)=С�� (d)=˫���� (b)=����");
	buf["8"]["13"].bin(u8":4��������������ʱ���������Զ�����zigzag�㷨ת������");
	buf["8"]["14"].bin(u8":5�������õ���������>0���������zigzag�㷨ת�ɸ��������˹��ж�");
	buf["15"].bin(u8":֧�� ����JCE����,һ�������������(��Ҫ���ģ��)");
	buf["16"].bin(u8":����-At-QQ-9359096");
	buf["17"].bin(u8":����ѧϰ������,����������ҵ���Ƿ�;��,���з��ɾ������������޹�.");


	//test Exception

	try
	{
		//std::string t = buf["1"]; //����varint���͵����� ����stringȥ����  һ�����쳣
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


}



int main()
{
	test1();
	return 0;
}