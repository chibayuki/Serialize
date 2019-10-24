/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2019 chibayuki@foxmail.com

Serialize.Demo
Version 19.10.20.0000

This file is part of Serialize

Serialize is released under the GPLv3 license
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>

#include "Serialize.h"

using namespace std;

#define bool_to_str(val) ((val) ? ("true") : ("false"))

struct MyStruct
{
	double d1;
	int64_t l1;
};

int main()
{
	MyStruct ms;
	ms.d1 = 3.14159;
	ms.l1 = 1048576;

	int i1 = 255;
	char c1 = 'C';
	string s1 = "Hello world";
	bool b1 = true;

	ChunkRef cr;

	Serializer ser1;
	(ser1 << i1 << c1 << ms << s1 << b1).ToBinary(cr);

	ms.d1 = 0;
	ms.l1 = 0;
	i1 = 0;
	c1 = 0;
	s1 = "";
	b1 = false;

	Serializer ser2;
	ser2.FromBinary(cr) >> i1 >> c1 >> ms >> s1 >> b1;

	cout << i1 << endl;
	cout << c1 << endl;
	cout << ms.d1 << endl;
	cout << ms.l1 << endl;
	cout << s1.c_str() << endl;
	cout << bool_to_str(b1) << endl;
}