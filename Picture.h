#pragma once
#include "stdafx.h"
#include <string>
#include "FileTypes.h"

using namespace std;

class Picture
{

private:

	string filename;
	int height, width;
	int actualHeight, actualWidth;
	int newHeight, newWidth;
	Pixel** image;

	void LoadPNG(string filename);
	void LoadJPEG(string filename);

	void CreateImageArray(int dimX, int dimY);
	void CalculateFullEnergy();

public:

	Picture(string filename, int filetype);
	virtual ~Picture();

	void DeleteRow(Point* positions);
	void DeleteColumn(Point* positions);

	Pixel GetPixel(int x, int y);
	int GetWidth();
	int GetHeight();
	Pixel** GetImage();

	void AutoResize();
};

