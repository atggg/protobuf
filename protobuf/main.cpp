#include<iostream>
#include"protobuf.h"

std::string tohexstr(const char* data, int len)
{
	std::string s;
	char d[10] = { 0 };
	for (int i = 0; i < len; i++)
	{
		if ((unsigned char)data[i] <= 0xf)
		{
			s.append("0");
		}
		s.append(_itoa((unsigned char)data[i], d, 16));
		s += " ";
	}
	return s;
}

void test1()
{
	protobuf buf;
	//buf["1"] = 1;
	//buf["2"][0] = "77777";
	//buf["2"][1] = "nbnbnbnb";
	//buf["3"]["1"] = "66666";
	//buf["3"]["2"] = "777777";
	//buf["4"][0]["1"] = "test1";
	//buf["4"][0]["2"] = "test1";
	//buf["4"][1]["1"] = "test2";
	//buf["4"][1]["2"] = "test2";
	buf["1"].varint(1);
	buf["2"][0].bin("77777");
	buf["2"][1].bin("nbnbnbnb");
	buf["3"]["1"].bin("66666");
	buf["3"]["2"].bin("777777");
	buf["4"][0]["1"].bin("test1");
	buf["4"][0]["2"].bin("test1");
	buf["4"][1]["1"].bin("test2");
	buf["4"][1]["2"].bin("test2");


	//test Exception

	try
	{
		std::string t = buf["1"]; //����varint���͵����� ����stringȥ����  һ�����쳣
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
	std::cout << tohexstr(pbstr.c_str(), pbstr.size()) << std::endl;
	protobuf pb = protobuf::parse(pbstr);
	pbstr.clear();
	pbstr = pb.make();
	std::cout << std::endl;
	std::cout << tohexstr(pbstr.c_str(), pbstr.size()) << std::endl;
}



int main()
{
	test1();
	return 0;
}