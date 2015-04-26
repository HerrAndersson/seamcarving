#pragma once
#include <string>
#include "stdafx.h"
using namespace std;

const int PNG =  1;
const int JPEG = 2;
const int JPG =  2;

const int NEG_INF = -2147000000;
const int INF     =  2147000000;

struct Point
{
	int x, y;

	Point()
	{

	}

	Point(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};

struct Pixel
{
	BYTE	r;
	BYTE	g;
	BYTE	b;
	int energy;

	Pixel()
	{
		r = 0;
		g = 0;
		b = 0;
		energy = NEG_INF;
	}
	Pixel(BYTE red, BYTE green, BYTE blue)
	{
		r = red;
		g = green;
		b = blue;
		energy = NEG_INF;
	}

	BYTE RGBtoGray()
	{
		return (BYTE)((int)(r + g + b) / 3);
	}

	BYTE EnergyToColor()
	{
		//return (BYTE)(energy / 255);
		return (BYTE)(energy / (255*3));  //255^2 - 0 ger max energy f�r varje f�rg, tre ggr ger delat med 255*3
	}
};