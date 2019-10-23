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

int main()
{
	int a = 5;
	double b = 12.7;
	char c = 'A';
	bool d = true;
	long e = 15987;

	ChunkRef cr;

	Serializer sr;
	sr << a << b << c << d << e >> cr;

	a = 0;
	b = 0;
	c = 0;
	d = 0;
	e = 0;

	sr << cr >> a >> b >> c >> d >> e;

	cout << a << endl;
	cout << b << endl;
	cout << c << endl;
	cout << d << endl;
	cout << e << endl;

}