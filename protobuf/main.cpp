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
	//去除空格
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
		std::string t = buf["1"]; //他是varint类型的数据 我用string去接收  一定抛异常
	}
	catch (protobufException& e)
	{
		switch (e.getExType())
		{
			case protobufException::exType::nullError:
			{
				std::cout << "是null 没有存储任何数据" << std::endl;
				break;
			}
			case protobufException::exType::valTypeError:
			{
				std::cout << "类型错误" << std::endl;
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


	//parse test

	std::string bin = hextobin("0A 0C 12 04 37 69 61 6E 18 A8 EC B7 82 02 22 0E 29 5D 4E 09 88 49 D8 10 40 35 F6 28 70 41 3A 40 E6 94 AF E6 8C 81 20 E9 BC A0 E6 A0 87 E5 8F B3 E9 94 AE E6 93 8D E4 BD 9C E3 80 81 48 45 58 2F 7B 31 2C 32 2C 33 7D 20 E6 88 96 20 E6 8B 96 E5 85 A5 50 42 E6 95 B0 E6 8D AE E6 96 87 E4 BB B6 42 DF 02 52 44 31 E3 80 81 5B 5D E5 86 85 E8 A1 A8 E7 A4 BA E8 B7 AF E5 BE 84 EF BC 8C E9 87 8C E9 9D A2 E7 9A 84 28 29 E8 A1 A8 E7 A4 BA E5 90 8C E8 B7 AF E5 BE 84 E6 97 B6 E7 9A 84 E4 B8 8D E5 90 8C E7 B4 A2 E5 BC 95 5A 3F 32 E3 80 81 5B 5D E5 90 8E E7 9A 84 28 48 45 58 29 E8 A1 A8 E7 A4 BA E8 AF A5 E8 B7 AF E5 BE 84 E5 AF B9 E5 BA 94 E7 9A 84 48 45 58 20 20 3D 20 E5 90 8E E8 A1 A8 E7 A4 BA E9 95 BF E5 BA A6 62 32 33 E3 80 81 28 69 29 3D E6 95 B4 E6 95 B0 20 28 66 29 3D E5 B0 8F E6 95 B0 20 28 64 29 3D E5 8F 8C E7 B2 BE E5 BA A6 20 28 62 29 3D E6 95 B0 E6 8D AE 6A 49 34 E3 80 81 E7 94 9F E6 88 90 E6 95 B4 E6 95 B0 E6 B5 8B E8 AF 95 E6 97 B6 EF BC 8C E8 B4 9F E6 95 B0 E4 BC 9A E8 87 AA E5 8A A8 E6 A0 B9 E6 8D AE 7A 69 67 7A 61 67 E7 AE 97 E6 B3 95 E8 BD AC E6 88 90 E6 AD A3 E6 95 B0 72 57 35 E3 80 81 E8 A7 A3 E6 9E 90 E5 BE 97 E5 88 B0 E7 9A 84 E6 95 B4 E6 95 B0 E9 83 BD 3E 30 EF BC 8C E4 B8 8D E4 BC 9A E6 A0 B9 E6 8D AE 7A 69 67 7A 61 67 E7 AE 97 E6 B3 95 E8 BD AC E6 88 90 E8 B4 9F E6 95 B0 EF BC 8C E9 9C 80 E4 BA BA E5 B7 A5 E5 88 A4 E6 96 AD 7A 43 E6 94 AF E6 8C 81 20 E8 A7 A3 E6 9E 90 4A 43 45 E6 95 B0 E6 8D AE 2C E4 B8 80 E9 94 AE E7 94 9F E6 88 90 E7 BB 84 E5 8C 85 E4 BB A3 E7 A0 81 28 E9 9C 80 E8 A6 81 E7 BB 93 E5 90 88 E6 A8 A1 E5 9D 97 29 82 01 18 E4 BD 9C E8 80 85 2D 37 69 61 6E 2D 51 51 2D 35 34 31 39 38 30 32 30 30 8A 01 5D E4 BB 85 E4 BE 9B E5 AD A6 E4 B9 A0 E4 BA A4 E6 B5 81 E7 94 A8 2C E8 AF B7 E5 8B BF E7 94 A8 E4 BA 8E E5 95 86 E4 B8 9A E5 8F 8A E9 9D 9E E6 B3 95 E9 80 94 E5 BE 84 2C E5 A6 82 E6 9C 89 E6 B3 95 E5 BE 8B E7 BA A0 E7 BA B7 E5 88 99 E4 B8 8E E4 BD 9C E8 80 85 E6 97 A0 E5 85 B3 2E");

	protobuf pb2;
	pb2.parse(bin);

	std::string pb2buff = pb2.make();

	std::cout << std::endl;
	std::cout << bintohex(pb2buff) << std::endl;

}



int main()
{
	test1();
	return 0;
}