#pragma once
#include "FileTypes.h"
#include "Picture.h"
class SeamCarver
{
private:

	Picture* pic;

	Point* FindVerticalSeam();
	Point* FindHorizontalSeam();

	int FindMin(int v1, int v2, int v3, int& index);
	int FindMin(int v1, int v2, int v3);

	int GetNumberOfDigits(int i);

public:

	SeamCarver(Picture* pic);
	virtual ~SeamCarver();

	void RemoveRowsAndColumns(int rowCount, int columnCount);
	void RemoveRows(int rowCount);
	void RemoveColumns(int columnCount);

	void ChangePicture(Picture* newPic);
};

