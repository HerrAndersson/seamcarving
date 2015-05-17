#pragma once
#include "stdafx.h"
#include <string>
#include "FileTypes.h"
#include "windows.h"

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

	void SavePNG(string path);
	void SaveJPEG(string path);

	void CreateImageArray(int dimX, int dimY);
	void CalculateFullEnergy();

	int CalculatePixelEnergy(Pixel* p1, Pixel* p2);

public:

	Picture(string filename, int filetype);
	virtual ~Picture();

	void DeleteRow(Point* positions);
	void DeleteColumn(Point* positions);

	Pixel GetPixel(int x, int y);
	int GetWidth();
	int GetHeight();
	Pixel** GetImage();

	//Row = true, column = false
	void ShowSeam(Point* positions, bool rowOrColumn);

	void AutoResize();

	void Save(string path, int type);
};

